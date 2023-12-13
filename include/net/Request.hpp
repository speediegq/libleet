/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU Affero General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <string>
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

namespace leetRequest {
    enum { /* supported protocols */
        LEET_REQUEST_PROTOCOL_HTTP,
        LEET_REQUEST_PROTOCOL_HTTPS,
    };
    enum { /* types of supported request types */
        LEET_REQUEST_REQTYPE_GET,
        LEET_REQUEST_REQTYPE_POST,
        LEET_REQUEST_REQTYPE_PUT,
    };
    /**
     * @brief  Class representing a parsed URL
     */
    class URL { /* useful for parsing a URL */
        private:
        public:
            std::string Host{};
            std::string Endpoint{};
            std::string Query{};
            int Protocol{LEET_REQUEST_PROTOCOL_HTTP};
            int Port{80};

            /**
             * @brief  Separate the components of a URL in the form of a string
             * @param  URL The URL to parse. The components can be accessed from the URL object.
             */
            void parseURLFromString(const std::string& URL);
            /**
             * @brief  Assemble a URL from specified parts
             * @return Returns a full URL based on the parts
             */
            const std::string assembleURLFromParts();
    };
    /**
     * @brief  Class representing the response after making a network request
     */
    class Response { /* the response */
        private:
        public:
            int statusCode{200};
            std::string Body{};
    };
    /**
     * @brief  Class representing a network request
     */
    class Request { /* the request */
        private:
        public:
            std::string Host{};
            std::string Endpoint{};
            std::string Query{};
            std::string userAgent{"LIBLEET_USER_AGENT"};
            std::string Body{};
            int Port{80};
            int Protocol{LEET_REQUEST_PROTOCOL_HTTP};
            int Type{LEET_REQUEST_REQTYPE_GET};

            std::vector<std::string> headerName{};
            std::vector<std::string> headerData{};
            std::string authenticationHeaderData{};
            std::string contentTypeHeaderData{};
            bool Authentication{false};

            std::string Filename{};
            std::string outputFile{};

            /**
             * @brief  Set an HTTP header
             * @param  Header The header to set
             * @param  Data The data to set the header to
             */
            void setHeader(const std::string& Header, const std::string& Data);
            /**
             * @brief  Set the Authentication header
             * @param  Data The data to set the header to
             */
            void setAuthenticationHeader(const std::string& Data);
            /**
             * @brief  Set the Content-Type header
             * @param  Data The data to set the header to
             */
            void setContentTypeHeader(const std::string& Data);
            /**
             * @brief  Make a network request
             * @return Returns a Response object
             */
            Response makeRequest();
            const bool downloadFile();
    };

    std::string userCert{}; // User-specified root certificate string

    const std::string getRootCertificates();
    void applyRootCertificates(boost::asio::ssl::context& ctx, const std::string& cert);
}

#endif
