/* libleet parse URL
 * ======================
 * This is a simple libleet example, which allows you to simply parse a URL.
 * It is quite useless, and only really serves as a test for the network request
 * wrapper.
 */
#include <iostream>
#include <string>
#include <libleet/libleet.hpp>
#include <libleet/net/Request.hpp>

int main() {
    std::string targetURL{};
    std::cout << "Enter a URL to parse:\n> ";
    std::getline(std::cin, targetURL);

    leetRequest::URL url;
    url.parseURLFromString(targetURL);

    std::cout << "Host:     " << url.Host << "\n";
    std::cout << "Endpoint: " << url.Endpoint << "\n";
    std::cout << "Query:    " << url.Query << "\n";

    if (url.Protocol == leetRequest::LEET_REQUEST_PROTOCOL_HTTP) {
        std::cout << "Protocol: HTTP\n";
    } else {
        std::cout << "Protocol: HTTPS\n";
    }

    std::cout << "Port:     " << url.Port << "\n";

    return 0;
}
