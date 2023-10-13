#include <iostream>
#include <string>
#include <algorithm>
#include "../src/libleet.hpp"
#include "../src/libleet.cpp"

int main(int argc, char** argv) {
    leet::MatrixOptions options;
    leet::User::Credentials cred;
    leet::User::CredentialsResponse resp;

    options.Homeserver = "https://matrix.org";
    cred.Type = TPassword;
    cred.Username = "speedie";
    cred.DeviceID = "libleet test client 2";

    std::getline(std::cin, cred.Password);

    leet::setSettings(&options);
    leet::saveCredentials(&cred);

    /* Attempt a login */
    resp = leet::connectHomeserver();

    if (leet::errorCode == 0) {
        leet::clearUserCredentials();
    } else { /* Oh no, failed to login */
        std::cerr << "Failed to authenticate. Invalid username/password (" << leet::Error << ")\n";
        return 1;
    }

    /* Set the initial transaction ID to 0.
     * loadTransID will load it from a file if it exists. We immediately save after,
     * and we're going to save every time this transaction ID is changed.
     */
    leet::TransID = 0;
    leet::loadTransID("/tmp/basic-chat-program-transid");
    leet::saveTransID("/tmp/basic-chat-program-transid");

    std::string myRoom{};
    std::cout << "Enter a matrix channel:\n> ";
    std::getline(std::cin, myRoom);

    /* This finds the room ID used internally (i.e. #speedie:matrix.org -> !somelongstringoftext) */
    leet::Room::Room room;
    room.RoomID = leet::findRoomID(myRoom);

    if (leet::errorCode != 0) {
        std::cerr << "Invalid channel: '" << myRoom << "'\n";
        return 1;
    }

    /* Finally, set the active room to the room specified by the user */
    leet::setRoom(&room);

    /* Read user messages in a loop */
    for (;;) {
        std::cout << "\033[2J\033[1;1H";
        std::vector<leet::Message::Message> messages = leet::returnMessages(&resp, 500);
        std::reverse(messages.begin(), messages.end());

        int i{1};
        for (auto &message : messages) {
            std::cout << i << ". " << message.Sender << " - " << message.messageText << std::endl;
            ++i;
        }

        std::string myMessage;

        std::cout << "> ";
        std::getline(std::cin, myMessage);

        /* Exiting, so we need to save the transaction ID */
        if (!myMessage.compare(":q")) {
            leet::saveTransID("/tmp/basic-chat-program-transid");
            std::exit(0);
        }

        /* Message class, this will contain message information, and the message itself */
        leet::Message::Message msg;
        msg.messageText = myMessage;

        /* Send the message */
        leet::sendMessage(&resp, &msg);

        /* We save it here as well, so that if the program is closed
         * using other means, we can restore.
         */
        leet::saveTransID("/tmp/basic-chat-program-transid");

        /* Exiting, so we need to save the transaction ID */
        if (leet::errorCode != 0) {
            leet::saveTransID("/tmp/basic-chat-program-transid");
            std::exit(1);
        }
    }
}
