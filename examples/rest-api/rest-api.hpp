const std::string generateResponseFromEndpoint(const std::string& Endpoint, const std::string& Body);

class Session : public std::enable_shared_from_this<Session> {
    public:
        explicit Session(boost::asio::ip::tcp::socket Socket) : exampleAPISocket(std::move(Socket)) {}

        void Start() {
            readRequest();
        }
    private:
        boost::asio::ip::tcp::socket exampleAPISocket;
        boost::beast::flat_buffer exampleAPIBuffer;
        boost::beast::http::request<boost::beast::http::string_body> exampleAPIRequest;
        boost::beast::http::response<boost::beast::http::string_body> exampleAPIResponse;

        void readRequest() {
            auto self = shared_from_this();
            boost::beast::http::async_read(
                exampleAPISocket,
                exampleAPIBuffer,
                exampleAPIRequest,
                [self](boost::beast::error_code ec, std::size_t transferredBytes) {
                    self->onRead(ec, transferredBytes);
                }
            );
        }

        void onRead(boost::beast::error_code ec, std::size_t transferredBytes) {
            if (!ec) {
                handleReq();
            }
        }

        void handleReq() {
            if (exampleAPIRequest.method() == boost::beast::http::verb::options) {
                exampleAPIResponse.result(boost::beast::http::status::no_content);
                exampleAPIResponse.set(boost::beast::http::field::allow, "GET, HEAD, OPTIONS");
                exampleAPIResponse.set(boost::beast::http::field::access_control_allow_origin, "*");
                exampleAPIResponse.set(boost::beast::http::field::access_control_allow_headers, "Content-Type");
            } else {
                std::string json_response = generateResponseFromEndpoint(exampleAPIRequest.target(), exampleAPIRequest.body());

                exampleAPIResponse.result(boost::beast::http::status::ok);
                exampleAPIResponse.set(boost::beast::http::field::content_type, "application/json");
                exampleAPIResponse.set(boost::beast::http::field::access_control_allow_origin, "*");
                exampleAPIResponse.body() = std::move(json_response);
            }

            auto self = shared_from_this();
            boost::beast::http::async_write(
                exampleAPISocket,
                exampleAPIResponse,
                [self](boost::beast::error_code ec, std::size_t transferredBytes) {
                    self->onWrite(ec);
                }
            );
        }

        void onWrite(boost::beast::error_code ec) {
            if (!ec) {
                boost::beast::error_code close_ec;
                exampleAPISocket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, close_ec);
            }
        }
};

class Listener {
    public:
        explicit Listener(boost::asio::io_context& ioc, boost::asio::ip::tcp::endpoint Endpoint)
            : apiIoc(ioc), apiAcceptor(ioc, Endpoint) {}

        void Run() {
            acceptReq();
        }

    private:
        boost::asio::io_context& apiIoc;
        boost::asio::ip::tcp::acceptor apiAcceptor;

        void acceptReq() {
            apiAcceptor.async_accept(
                [this](boost::beast::error_code ec, boost::asio::ip::tcp::socket Socket) {
                    if (!ec) {
                        std::make_shared<Session>(std::move(Socket))->Start();
                    }
                    acceptReq();
                }
            );
        }
};
