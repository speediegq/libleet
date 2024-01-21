#include <iostream>
#include <string>
#include <libleet/libleet.hpp>

int main() {
    std::string username{};
    std::cout << "\033[2J\033[1;1H";
    std::cout << "Enter a Matrix username (@<username>:<home server>)\n> ";
    std::getline(std::cin, username);

    if (username[0] != '@') {
    	std::cout << "You are stupid.\n";
	    return 1;
    }

    leet::Homeserver = leet::returnServerDiscovery(leet::returnHomeServerFromString(username));

    std::vector<std::string> supportedTypes = leet::returnSupportedLoginTypes();

    for (auto it : supportedTypes) {
    	std::cout << it << "\n";
    }
}
