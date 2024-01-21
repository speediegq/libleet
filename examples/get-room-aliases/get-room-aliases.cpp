#include <iostream>
#include <string>
#include <libleet/libleet.hpp>

int main() {
    std::string roomID{};
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
    std::cout << "Enter a Matrix room ID\n> ";
    std::getline(std::cin, roomID);

    cred.deviceID = "libleet test client";
    cred.Homeserver = leet::returnServerDiscovery(leet::returnHomeServerFromString(cred.Username));

    leet::User::credentialsResponse resp;

    resp = leet::loginAccount(cred);

    cred.clearCredentials();

    if (roomID[0] == '#') {
	    std::cout << "Room aliases: " << roomID << "\n";
    } else {
	    std::vector<std::string> Aliases = leet::findRoomAliases(resp, roomID);
	    std::cout << "Room aliases:\n";

	    for (auto it : Aliases) {
		    std::cout << it << "\n";
	    }
    }
}
