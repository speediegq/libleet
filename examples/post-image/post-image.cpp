#include <iostream>
#include <string>
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

    leet::User::CredentialsResponse resp;

    if (leet::checkIfUsernameIsAvailable(cred.Username)) {
        resp = leet::registerAccount(cred);
    } else {
        resp = leet::loginAccount(cred);
    }

    cred.clearCredentials();

    if (!leet::checkError()) { // Yeah, appears something went wrong.
        return 1;
    }

    leet::transID = leet::returnUnixTimestamp();

    std::string myRoom{""};
    std::cout << "\033[2J\033[1;1H\n";
    std::vector<leet::Room::Room> vector = leet::returnRooms(resp, 9999);

    for (auto it : vector) {
        const std::string Alias = it.Alias.compare("") ? it.Alias : it.roomID;
        const std::string Topic = it.Topic.compare("") ? it.Topic : "No room topic specified.";
        std::cout << Alias << " - " << Topic << "\n";
    }

    std::cout << "\n";

    std::cout << "Enter a matrix channel:\n> ";
    std::getline(std::cin, myRoom);

    leet::Room::Room room;
    room.roomID = leet::findRoomID(myRoom);

    room = leet::returnRoom(resp, room);

    if (!leet::checkError()) { // Yeah, appears something went wrong.
        std::cout << "Are you stupid? That isn't a valid channel... I think.\n";
        return 1;
    }

    leet::Event::Message msg;

    msg.messageText = "test.png";
    msg.messageType = "m.image";

    leet::Attachment::Attachment attachment = leet::uploadFile(resp, "test.png");

    msg.attachmentURL = attachment.URL;

    leet::transID = leet::returnUnixTimestamp();

    leet::sendMessage(resp, room, msg);

    if (leet::errorCode != 0) {
        std::exit(1);
    }
}
