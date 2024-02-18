#include <iostream>
#include <string>
#include <fstream>
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

    leet::User::CredentialsResponse resp;

    resp = leet::loginAccount(cred);

    cred.clearCredentials();

    leet::transID = leet::returnUnixTimestamp();

    leet::Sync::SyncConfiguration conf;
    leet::Sync::Sync sync = leet::returnSync(resp, conf);

    std::ofstream file("output.txt");

    std::cout << "Writing to file..\n";

    for (auto it : sync.megolmSessions) {
        file << "Algorithm:   " << it.Algorithm << "\n";
        file << "Sender Key:  " << it.senderKey << "\n";
        file << "Sender:      " << it.Sender << "\n";
        file << "Type:        " << it.Type << "\n";
        file << "Cipher Type: " << it.cipherType << "\n";
        file << "Cipher Text: " << it.cipherText << "\n";
	    file << "\n";
    }

    for (auto it : sync.roomEvents.Invites) {
	    file << "Room ID:      " << it.roomID << "\n";
	    file << "User ID:      " << it.userID << "\n";
	    file << "Event ID:     " << it.eventID << "\n";
	    file << "Avatar URL:   " << it.avatarURL << "\n";
	    file << "Display name: " << it.displayName << "\n";
	    file << "Creator:      " << it.Creator << "\n";
	    file << "Join Rule:    " << it.joinRule << "\n";
	    file << "Room Name:    " << it.roomName << "\n";
	    file << "Room Topic:   " << it.roomTopic << "\n";
	    file << "Room Version: " << it.roomVersion << "\n";
	    file << "Encrypted:    " << (it.Encrypted ? "Yes" : "No") << "\n";
	    file << "\n";
    }

    file.close();
}
