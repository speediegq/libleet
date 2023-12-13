#include <iostream>
#include <string>
#include <algorithm>
#include <filesystem>
#include "../../include/libleet.hpp"
#include "../../src/libleet.cpp"

int checkError() {
    if (leet::errorCode != 0) {
        std::cerr << "Failed. " << leet::friendlyError << " (" << leet::Error << ")\n";
        return 1;
    }
    return 0;
}

int main() {
    leet::User::Credentials cred;
    leet::Attachment::Attachment attachment;
    std::string outputPath{};

    cred.Identifier = LEET_IDENTIFIER_USERID;
    cred.Type = LEET_TYPE_PASSWORD;

    std::cout << "\033[2J\033[1;1H";
    std::cout << "Enter a Matrix username (@<username>:<home server>)\n> ";
    std::getline(std::cin, cred.Username);

    std::cout << "\033[2J\033[1;1H";
    std::cout << "Enter a Matrix password\n> ";
    std::getline(std::cin, cred.Password);

    std::cout << "\033[2J\033[1;1H";
    std::cout << "Enter a Matrix mxc:// URL\n> ";
    std::getline(std::cin, attachment.URL);

    std::cout << "\033[2J\033[1;1H";
    std::cout << "Enter an output file\n> ";
    std::getline(std::cin, outputPath);

    cred.deviceID = "libleet test client";
    cred.Homeserver = leet::returnServerDiscovery(leet::returnHomeServerFromString(cred.Username));

    leet::User::credentialsResponse resp;

    resp = leet::loginAccount(&cred);

    cred.clearCredentials();

    if (checkError() == true) {
        return false;
    }

    leet::transID = leet::returnUnixTimestamp();

    if (leet::downloadFile(&resp, &attachment, outputPath)) {
        std::cout << "File downloaded and saved to '" << outputPath << "'\n";
        return 0;
    } else {
        std::cerr << "Failed to download file.\n";
        return 1;
    }
}
