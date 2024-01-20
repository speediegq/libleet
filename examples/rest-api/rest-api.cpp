#include <iostream>
#include <string>
#include <vector>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include <libleet/libleet.hpp>

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
            (void)transferredBytes; // prevents a unused parameter warning
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
                std::string json_response = generateResponseFromEndpoint(std::string(exampleAPIRequest.target().data(), exampleAPIRequest.target().size()), exampleAPIRequest.body());

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
                    (void)transferredBytes; // prevents a unused parameter warning
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

const std::string returnRooms(const std::string& Body) {
    if (!Body.compare("")) {
        nlohmann::json ErrorResponse;
        ErrorResponse["error"] = "API_NO_BODY";
        return ErrorResponse.dump();
    }

    nlohmann::json Incoming;

    try {
        Incoming = { nlohmann::json::parse(Body) };
    } catch (const nlohmann::json::parse_error& e) {
        nlohmann::json ErrorResponse;
        ErrorResponse["error"] = "API_INVALID_UTF8";
        return ErrorResponse.dump();
    }

    leet::User::credentialsResponse resp;

    if (nlohmann::json::accept(Body)) {
        for (auto& it : Incoming) {
            if (it["token"].is_string()) resp.accessToken = it["token"].get<std::string>();
            if (it["devid"].is_string()) resp.deviceID = it["devid"].get<std::string>();
            if (it["refreshtoken"].is_string()) resp.refreshToken = it["refreshtoken"].get<std::string>();
            if (it["userid"].is_string()) resp.userID = it["userid"].get<std::string>();
            if (it["homeserver"].is_string()) resp.Homeserver = it["homeserver"].get<std::string>();
        }
    }

    bool Error = false;
    std::string theError{""};

    if (!resp.accessToken.compare("")) {
        Error = true;
        theError = "API_NO_ACCESS_TOKEN";
    } else if (!resp.deviceID.compare("")) {
        Error = true;
        theError = "API_NO_DEVICE_ID";
    } else if (!resp.userID.compare("")) {
        Error = true;
        theError = "API_NO_USER_ID";
    } else if (!resp.Homeserver.compare("")) {
        Error = true;
        theError = "API_NO_HOME_SERVER";
    }

    // return an error
    if (Error) {
        nlohmann::json ErrorResponse;
        ErrorResponse["error"] = theError;
        return ErrorResponse.dump();
    }

    std::vector<leet::Room::Room> vector = leet::returnRooms(&resp, 9999);

    if (leet::errorCode != 0 || leet::friendlyError.compare("") || leet::Error.compare("")) {
        nlohmann::json ErrorResponse;
        ErrorResponse["error"] = "API_FAILED_TO_RETURN_ROOMS";
        return ErrorResponse.dump();
    }

    nlohmann::json roomList;

    for (auto& it : vector) {
        roomList[it.roomID]["roomid"] = it.roomID;
        roomList[it.roomID]["roomtype"] = it.roomType;
        roomList[it.roomID]["roomname"] = it.Name;
        roomList[it.roomID]["roomavatar"] = it.avatarURL;
        roomList[it.roomID]["guestcanjoin"] = it.guestCanJoin;
        roomList[it.roomID]["worldreadable"] = it.worldReadable;
        roomList[it.roomID]["roomalias"] = it.Alias;
        roomList[it.roomID]["roomtopic"] = it.Topic;
        roomList[it.roomID]["joinrule"] = it.joinRule;
        roomList[it.roomID]["membercount"] = it.memberCount;
    }

    return roomList.dump();
}

const std::string attemptLogin(const std::string& Body) {
    if (!Body.compare("")) {
        nlohmann::json ErrorResponse;
        ErrorResponse["error"] = "API_NO_BODY";
        return ErrorResponse.dump();
    }

    nlohmann::json Incoming;

    try {
        Incoming = { nlohmann::json::parse(Body) };
    } catch (const nlohmann::json::parse_error& e) {
        nlohmann::json ErrorResponse;
        ErrorResponse["error"] = "API_INVALID_UTF8";
        return ErrorResponse.dump();
    }

    leet::User::Credentials cred;

    if (nlohmann::json::accept(Body)) {
        for (auto& output : Incoming) {
            if (output["username"].is_string()) cred.Username = output["username"].get<std::string>();
            if (output["password"].is_string()) cred.Password = output["password"].get<std::string>();
            if (output["devid"].is_string()) cred.deviceID = output["devid"].get<std::string>();
        }
    }

    cred.Homeserver = leet::Homeserver;

    bool Error = false;
    std::string theError{""};

    if (!cred.Username.compare("")) {
        Error = true;
        theError = "API_NO_USERNAME";
    } else if (!cred.Password.compare("")) {
        Error = true;
        theError = "API_NO_PASSWORD";
    }

    if (Error) {
        nlohmann::json ErrorResponse;
        ErrorResponse["error"] = theError;
        return ErrorResponse.dump();
    }

    if (!cred.Username.compare("")) {
        cred.Homeserver = leet::returnServerDiscovery(leet::returnHomeServerFromString(cred.Username));
    }

    leet::User::credentialsResponse resp;
    resp = leet::loginAccount(&cred);

    nlohmann::json jsonResponse;

    if (leet::errorCode != 0 || leet::Error.compare("")) {
        jsonResponse["error"] = leet::Error;
        jsonResponse["friendlyerror"] = leet::friendlyError;
    } else {
        jsonResponse["token"] = resp.accessToken;
        jsonResponse["refreshtoken"] = resp.refreshToken;
        jsonResponse["devid"] = resp.deviceID;
        jsonResponse["homeserver"] = resp.Homeserver;
        jsonResponse["userid"] = resp.userID;
        jsonResponse["expiration"] = resp.Expiration;
    }

    return jsonResponse.dump();
}

const std::string generateResponseFromEndpoint(const std::string& Endpoint, const std::string& Body) {
    if (!Endpoint.compare("")) {
        nlohmann::json ErrorResponse;
        ErrorResponse["error"] = "API_NO_ENDPOINT_SPECIFIED";
        return ErrorResponse.dump();
    } else if (!Endpoint.compare("/api/login")) { // login
        return attemptLogin(Body);
    } else if (!Endpoint.compare("/api/return_rooms")) { // return rooms
        return returnRooms(Body);
    } else {
        nlohmann::json ErrorResponse;
        ErrorResponse["error"] = "API_INVALID_ENDPOINT_SPECIFIED";
        return ErrorResponse.dump();
    }

    return "";
}

class Program {
    private:
    public:
        Program(const int Port) {
            boost::asio::io_context ioc;
            boost::asio::ip::tcp::endpoint Endpoint(boost::asio::ip::tcp::v4(), Port);

            std::cerr << "[NOTICE]: rest-api backend is running on port " << Port << ".\n";

            Listener listener(ioc, Endpoint);
            listener.Run();
            ioc.run();
        }
};

int main() {
    try {
        Program program{8080};
    } catch (const std::exception& e) {
        std::cerr << "[WARNING] An error occured: " << e.what() << "\nProgram not halted.\n";
        Program program{8080};
    }
}
