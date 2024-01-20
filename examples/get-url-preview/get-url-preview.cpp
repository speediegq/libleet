#include <iostream>
#include <string>
#include <libleet/libleet.hpp>

int main() {
    std::string URL{};
    leet::User::Credentials cred;

    cred.Identifier = leet::LEET_IDENTIFIER_USERID;
    cred.Type = leet::LEET_TYPE_PASSWORD;

    std::cout << "\033[2J\033[1;1H";
    std::cout << "Enter a Matrix username (@<username>:<home server>)\n> ";
    std::getline(std::cin, cred.Username);

    std::cout << "\033[2J\033[1;1H";
    std::cout << "Enter a Matrix password\n> ";
    std::getline(std::cin, cred.Password);

    std::cout << "\033[2J\033[1;1H";
    std::cout << "Enter a URL\n> ";
    std::getline(std::cin, URL);

    cred.deviceID = "libleet test client";
    cred.Homeserver = leet::returnServerDiscovery(leet::returnHomeServerFromString(cred.Username));

    leet::User::credentialsResponse resp;

    if (leet::checkIfUsernameIsAvailable(cred.Username)) {
        resp = leet::registerAccount(&cred);
    } else {
        resp = leet::loginAccount(&cred);
    }

    cred.clearCredentials();

    if (!leet::checkError()) { // Yeah, appears something went wrong.
        return 1;
    }

    leet::transID = leet::returnUnixTimestamp();

    leet::URL::urlPreview prev = leet::getURLPreview(&resp, URL, 0);
    std::cout << "\033[2J\033[1;1H";

    std::cout << "URL:          " << prev.URL << "\n";
    std::cout << "Time:         " << prev.Time << "\n";
    std::cout << "Image Size:   " << prev.imageSize << "\n";
    std::cout << "Image Width:  " << prev.imageWidth << "\n";
    std::cout << "Image Height: " << prev.imageHeight << "\n";
    std::cout << "Image URL:    " << prev.imageURL << "\n";
    std::cout << "Title:        " << prev.Title << "\n";
    std::cout << "Description:  " << prev.Description << "\n";
    std::cout << "Type:         " << prev.Type << "\n";

    return 0;
}
