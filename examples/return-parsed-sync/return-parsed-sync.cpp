#include <iostream>
#include <string>
#include <algorithm>
#include <filesystem>
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

    /*
    for (const auto& it : sync.megolmSessions) {
        std::cout << "Algorithm:   " << it.Algorithm << "\n";
        std::cout << "Sender Key:  " << it.senderKey << "\n";
        std::cout << "Sender:      " << it.Sender << "\n";
        std::cout << "Type:        " << it.Type << "\n";
        std::cout << "Cipher Type: " << it.cipherType << "\n";
        std::cout << "Cipher Text: " << it.cipherText << "\n";
	    std::cout << "\n";
    }
    */

    for (const auto& it : sync.roomEvents.Invites) {
	    std::cout << "Room ID:      " << it.roomID << "\n";
	    std::cout << "User ID:      " << it.userID << "\n";
	    std::cout << "Invite ID:    " << it.inviteID << "\n";
	    std::cout << "Avatar URL:   " << it.avatarURL << "\n";
	    std::cout << "Display name: " << it.displayName << "\n";
	    std::cout << "Creator:      " << it.Creator << "\n";
	    std::cout << "Type:         " << it.Type << "\n";
	    std::cout << "Join Rule:    " << it.joinRule << "\n";
	    std::cout << "Room Name:    " << it.roomName << "\n";
	    std::cout << "Room Topic:   " << it.roomTopic << "\n";
	    std::cout << "Room Version: " << it.roomVersion << "\n";
	    std::cout << "Encrypted:    " << (it.Encrypted ? "Yes" : "No") << "\n";
	    std::cout << "\n";
    }
}
