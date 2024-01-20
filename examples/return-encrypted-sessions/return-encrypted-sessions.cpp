#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <libleet/libleet.hpp>

int main() {
    leet::User::Credentials cred;

    cred.Identifier = leet::LEET_IDENTIFIER_USERID;
    cred.Type = leet::LEET_TYPE_PASSWORD;

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

    leet::transID = leet::returnUnixTimestamp();

    leet::Sync::syncConfiguration conf;
    leet::Sync::Sync sync = leet::returnSync(&resp, &conf);

    for (const auto& it : sync.megolmSessions) {
        std::cout << "Algorithm:   " << it.Algorithm << "\n";
        std::cout << "Sender Key:  " << it.senderKey << "\n";
        std::cout << "Sender:      " << it.Sender << "\n";
        std::cout << "Type:        " << it.Type << "\n";
        std::cout << "Cipher Type: " << it.cipherType << "\n";
        std::cout << "Cipher Text: " << it.cipherText << "\n\n";
    }
}
