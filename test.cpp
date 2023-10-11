#include <iostream>
#include <string>
#include <libleet/libleet.hpp>

int main(int argc, char** argv) {
    const std::string myRoom { "!OGNyGIKFSskVdwouMg:matrix.org" };
    const std::string myMessage { argv[1] };

    leet::MatrixOptions options;
    leet::User::Credentials cred;

    options.Homeserver = "https://matrix.org";
    cred.Type = TPassword;
    cred.Username = "speedie";
    cred.DeviceID = "libleet test client";
    std::getline(std::cin, cred.Password);

    leet::setSettings(&options);
    leet::saveCredentials(&cred);

    leet::connectHomeserver();

    if (leet::errorCode == 0) {
        leet::clearUserCredentials();
    } else {
        std::cerr << "Failed to authenticate. Invalid username/password (" << leet::ServerResponse.Error << ")\n";
        return 1;
    }

    leet::TransID = std::atoi(argv[2]);

    leet::setRoom(myRoom);
    leet::sendSimpleMessage(myMessage);

    if (leet::errorCode == 1) {
        std::cerr << resp.Error;
    } else {
        std::cout << "Successfully sent message '" << myMessage << "' to '" << myRoom << "'";
    }
}
