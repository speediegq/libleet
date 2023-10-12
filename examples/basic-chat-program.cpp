#include <iostream>
#include <string>
#include "../src/libleet.hpp"
#include "../src/libleet.cpp"

int main(int argc, char** argv) {
    leet::MatrixOptions options;
    leet::User::Credentials cred;
    leet::User::CredentialsResponse resp;

    options.Homeserver = "https://matrix.org";
    cred.Type = TPassword;
    cred.Username = "speedie";
    cred.DeviceID = "libleet test client";

    std::getline(std::cin, cred.Password);

    leet::setSettings(&options);
    leet::saveCredentials(&cred);

    resp = leet::connectHomeserver();

    if (leet::errorCode == 0) {
        leet::clearUserCredentials();
    } else {
        std::cerr << "Failed to authenticate. Invalid username/password (" << leet::Error << ")\n";
        return 1;
    }

    std::string TransID{};
    leet::TransID = 0;

    std::cout << "Enter the initial transaction ID\n> ";
    std::getline(std::cin, TransID);

    leet::TransID = std::atoi(TransID.c_str());

    std::string myRoom{};
    std::cout << "Enter a matrix channel:\n> ";
    std::getline(std::cin, myRoom);

    myRoom = leet::findRoomID(myRoom);

    if (leet::errorCode != 0) {
        std::cerr << "Invalid channel.";
        return 1;
    }

    leet::setRoom(myRoom);

    for (;;) {
        std::string myMessage;

        std::cout << "> ";
        std::getline(std::cin, myMessage);

        if (!myMessage.compare(":q")) {
            std::exit(0);
        }

        leet::sendSimpleMessage(&resp, myMessage);

        if (leet::errorCode != 0) {
            std::exit(1);
        }
    }
}
