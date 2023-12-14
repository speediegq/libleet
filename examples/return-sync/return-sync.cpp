#include <iostream>
#include <string>
#include <algorithm>
#include <filesystem>
#include <libleet/libleet.hpp>

int main() {
    leet::User::Credentials cred;

    cred.Identifier = LEET_IDENTIFIER_USERID;
    cred.Type = LEET_TYPE_PASSWORD;

    std::cout << "\033[2J\033[1;1H";
    std::cout << "Enter a Matrix username (@<username>:<home server>)\n> ";
    std::getline(std::cin, cred.Username);

    std::cout << "\033[2J\033[1;1H";
    std::cout << "Enter a Matrix password\n> ";
    std::getline(std::cin, cred.Password);

    cred.deviceID = "libleet test client";
    cred.Homeserver = leet::returnServerDiscovery(leet::returnHomeServerFromString(cred.Username));

    leet::User::credentialsResponse resp;

    resp = leet::loginAccount(&cred);

    cred.clearCredentials();

    if (!leet::checkError()) {
        return false;
    }

    leet::transID = leet::returnUnixTimestamp();

    leet::Sync::Sync sync = leet::returnSync(&resp);

    std::cout << sync.theRequest << "\n";
}
