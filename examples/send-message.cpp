#include <iostream>
#include <libleet/libleet.hpp>

int main(int argc, char** argv) {
    const std::string myRoom { "!OGNyGIKFSskVdwouMg:matrix.org" };
    const std::string myMessage { argv[1] };

    leet::MatrixOptions options;
    leet::User::Credentials cred;
    leet::User::CredentialsResponse resp;

    options.Homeserver = "https://matrix.org";
    cred.Type = TPassword;
    cred.Username = "speedie";
    cred.deviceID = "libleet test client";
    std::getline(std::cin, cred.Password);

    leet::setSettings(&options);
    leet::saveCredentials(&cred);

    resp = leet::loginAccount();

    if (leet::errorCode == 0) {
        leet::clearUserCredentials();
    } else {
        std::cerr << "Failed to authenticate. Invalid username/password (" << leet::Error << ")\n";
        return 1;
    }

    leet::TransID = std::atoi(argv[2]);

    leet::setRoom(myRoom);
    leet::sendSimpleMessage(&resp, myMessage);

    if (leet::errorCode == 1) {
        std::cerr << leet::Error;
    } else {
        std::cout << "Successfully sent message '" << myMessage << "' to '" << myRoom << "'";
    }
}
