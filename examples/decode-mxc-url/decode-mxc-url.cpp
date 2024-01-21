#include <iostream>
#include <string>
#include <libleet/libleet.hpp>

int main() {
    leet::User::Credentials cred{};
    leet::Attachment::Attachment attachment{};

    cred.Identifier = leet::LEET_IDENTIFIER_USERID;
    cred.Type = leet::LEET_TYPE_PASSWORD;

    std::cout << "\033[2J\033[1;1H";
    std::cout << "Enter a Matrix username (@<username>:<home server>)\n> ";
    std::getline(std::cin, cred.Username);

    std::cout << "\033[2J\033[1;1H";
    std::cout << "Enter a Matrix password\n> ";
    std::getline(std::cin, cred.Password);

    std::cout << "\033[2J\033[1;1H";
    std::cout << "Enter a Matrix mxc:// URL\n> ";
    std::getline(std::cin, attachment.URL);

    cred.deviceID = "libleet test client";
    cred.Homeserver = leet::returnServerDiscovery(leet::returnHomeServerFromString(cred.Username));

    leet::User::credentialsResponse resp;

    resp = leet::loginAccount(cred);

    cred.clearCredentials();

    if (!leet::checkError()) { // Yeah, appears something went wrong.
        return 1;
    }

    leet::transID = leet::returnUnixTimestamp();

    std::cout << leet::decodeFile(resp, attachment) << "\n";

    if (leet::errorCode != 0) {
        std::exit(1);
    }
}
