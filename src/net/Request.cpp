/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU Affero General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

#include <iostream>
#include <string>
#include <regex>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ssl/rfc2818_verification.hpp>
#include <boost/asio/ssl/host_name_verification.hpp>
#include <openssl/ssl.h>
#include "../../include/net/Request.hpp"
#include "Cert.cpp"

void leetRequest::URL::parseURLFromString(const std::string& URL) {
    std::regex urlReg("(http|https)://([^/ :]+):?([^/ ]*)(/?[^ #?]*)\\x3f?([^ #]*)#?([^ ]*)");
    std::smatch Match;

    if (std::regex_match(URL, Match, urlReg)) {
        if (!Match[1].str().compare("https")) {
            Protocol = leetRequest::LEET_REQUEST_PROTOCOL_HTTPS;
            Port = 443;
        } else {
            Protocol = leetRequest::LEET_REQUEST_PROTOCOL_HTTP;
            Port = 80;
        }

        Host = Match[2].str();

        if (Match[3].str().compare("")) {
            Port = std::stoi(Match[3].str());
        }

        Endpoint = Match[4].str();

        if (Match[5].str().compare("")) {
            Query = "?" + Match[5].str();
        }
    }
}

const std::string leetRequest::URL::assembleURLFromParts() {
    std::string ret{};
    if (Protocol == leetRequest::LEET_REQUEST_PROTOCOL_HTTPS) {
        ret += "https://" + Host;
    } else {
        ret += "http://" + Host;
    }

    if (!(Protocol == leetRequest::LEET_REQUEST_PROTOCOL_HTTPS && Port == 443) &&
        !(Protocol == leetRequest::LEET_REQUEST_PROTOCOL_HTTP && Port == 80)) {
        ret += ":" + std::to_string(Port);
    }

    ret += Endpoint + Query;

    return ret;
}

void leetRequest::Request::setHeader(const std::string& Header, const std::string& Data) {
    headerName.push_back(Header);
    headerData.push_back(Data);
}

void leetRequest::Request::setAuthenticationHeader(const std::string& Data) {
    authenticationHeaderData = Data;
    Authentication = true;
}

void leetRequest::Request::setContentTypeHeader(const std::string& Data) {
    contentTypeHeaderData = Data;
}

leetRequest::Response leetRequest::Request::makeRequest() {
    leetRequest::Response resp;

    try {
        boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12_client);

        const std::string_view cert{leetRequest::getRootCertificates()};
	
	boost::system::error_code ec;

	ctx.add_certificate_authority(boost::asio::buffer(cert.data(), cert.size()), ec);

	if (ec) {
	    throw boost::system::system_error{ec};
	}

        boost::asio::io_context ioc;
        boost::asio::ip::tcp::resolver resolver(ioc);

        ctx.set_verify_mode(boost::asio::ssl::verify_peer);
        ctx.set_verify_callback(boost::asio::ssl::host_name_verification(Host));

        boost::beast::ssl_stream<boost::beast::tcp_stream> stream(ioc, ctx);

        if (!SSL_set_tlsext_host_name(stream.native_handle(), Host.c_str())) {
		boost::system::error_code ssl_ec{static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()};
            throw boost::beast::system_error{ssl_ec};
        }

        const auto results { resolver.resolve(Host, std::to_string(Port)) };

        boost::beast::get_lowest_layer(stream).connect(results);

        stream.handshake(boost::asio::ssl::stream_base::client);

        boost::beast::http::verb theVerb;

        switch (Type) {
            case leetRequest::LEET_REQUEST_REQTYPE_GET:
                theVerb = boost::beast::http::verb::get;
                break;
            case leetRequest::LEET_REQUEST_REQTYPE_POST:
                theVerb = boost::beast::http::verb::post;
                break;
            case leetRequest::LEET_REQUEST_REQTYPE_PUT:
                theVerb = boost::beast::http::verb::put;
                break;
            default:
                break;
        }

        boost::beast::http::request<boost::beast::http::string_body> httpRequest{theVerb, Host + std::to_string(Port), 11};
        httpRequest.set(boost::beast::http::field::host, Host);

        if (userAgent.compare("")) httpRequest.set(boost::beast::http::field::user_agent, userAgent);
        if (contentTypeHeaderData.compare("")) httpRequest.set(boost::beast::http::field::content_type, contentTypeHeaderData);
        if (Body.compare("")) httpRequest.body() = Body;
        if (Authentication) httpRequest.set(boost::beast::http::field::authorization, authenticationHeaderData);

        for (int it{0}; it < static_cast<int>(headerName.size()); ++it) {
            if (!headerName[it].compare("") || !headerData[it].compare("")) {
                continue;
            }

            httpRequest.set(headerName[it], headerData[it]);
        }

        if (Filename.compare("")) {
            std::filesystem::path fileName = Filename;

            if (std::filesystem::exists(fileName)) {
                std::ostringstream fileBody;

                fileBody << std::ifstream(fileName, std::ios::binary).rdbuf();

                std::string theBody{ std::move(fileBody).str() };
                httpRequest.body() = theBody;
                httpRequest.set(boost::beast::http::field::content_length, std::to_string(theBody.size()));
            }
        }

        httpRequest.target(Endpoint + Query);

        httpRequest.prepare_payload();

        boost::beast::http::write(stream, httpRequest);

        boost::beast::flat_buffer flat_buffer;
        boost::beast::http::response<boost::beast::http::dynamic_body> res;
        boost::beast::http::read(stream, flat_buffer, res);

        resp.statusCode = res.result_int();
        resp.Body = boost::beast::buffers_to_string(res.body().data());

        stream.next_layer().cancel();
        stream.shutdown(ec);
        stream.next_layer().close();

        if (ec == boost::asio::error::eof) {
            ec = {};
        }
        if (ec == boost::asio::ssl::error::stream_truncated) {
            return resp;
        }
        if (ec) {
            throw boost::beast::system_error{ec};
        }

        return resp;
    } catch (boost::wrapexcept<boost::system::system_error> const &e) {
        std::cout << e.what();
    }

    return resp;
}

const bool leetRequest::Request::downloadFile() {
    if (!outputFile.compare("")) return false;

    leetRequest::Response resp = makeRequest();

    std::ofstream of(outputFile, std::ios::binary);
    of << resp.Body;

    if (resp.statusCode == 200) {
        return true;
    }

    return false;
}
