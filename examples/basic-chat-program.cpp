#include <iostream>
#include <string>
#include <algorithm>
#include <filesystem>
#include "../src/libleet.hpp"
#include "../src/libleet.cpp"

int main(int argc, char** argv) {
    leet::MatrixOptions options;
    leet::User::Credentials cred;
    leet::User::CredentialsResponse resp;

    cred.Type = TPassword;
    cred.Username = "@speedie:matrix.org";
    cred.Password = "myPassword";
    cred.DeviceID = "libleet test client 3";

    options.Homeserver = leet::defaultHomeserver;

    /* Now let's try discovery */
    options.Homeserver = leet::returnServerDiscovery(leet::returnHomeServerFromString(cred.Username));

    /* Create a proper username. If we don't do this, leet::defaultHomeserver will be used as a fallback.
     * We don't want this, because it means only accounts registered under matrix.org will be functional.
     */
    if (cred.Username[0] != '@') {
        cred.Username = "@" + cred.Username + ":" + options.Homeserver;
    }

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
            std::cout << "\033[0;31m" << i << ". \033[0m" << message.Sender << " - " << message.messageText << std::endl;
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
        msg.messageType = "m.text";

        /* We upload a file if it exists
         * Please note that this is a terrible implementation, ideally you
         * should check if the file is an audio file, video file, image, and finally a generic file if all else fails.
         *
         * But for the simplicity of this example, we're going to use m.file, which will tell Matrix that it is a generic file.
         * Below is an example of uploading a video, though.
         */
        std::filesystem::path file{ myMessage };
        if (std::filesystem::exists(file)) {
            msg.messageType = "m.file";
            msg.messageText = file.filename();
            msg.attachmentURL = leet::uploadFile(&resp, myMessage);

            if (leet::errorCode != 0) { /* Something went wrong */
                continue;
            }
        }

        /* Example of uploading a video
        msg.messageText = "test.mp4"; // Text, doesn't matter that much but Element does this so we'll copy it
        msg.messageType = "m.video"; // m.audio, m.video, m.image, m.text, m.file, ...
        msg.attachmentURL = leet::uploadFile(&resp, "/home/speedie/test.mp4"); // uploadFile uploads the file and returns a mxc:// URL
        */

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
