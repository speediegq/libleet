#include <iostream>
#include <libleet/libleet.hpp>

int main(int argc, char** argv) {
    leet::MatrixOptions options;
    options.Homeserver = "https://matrix.org";

    leet::setSettings(&options);

    std::string str{};
    std::cout << "Enter a matrix username:\n> ";
    std::getline(std::cin, str);

    str = leet::finduserID(str);

    leet::User::Profile profile = leet::getUserData(str);

    if (leet::errorCode == 0) {
        std::cout << "Avatar URL: " << profile.avatarURL << std::endl;
        std::cout << "Nickname: " << profile.displayName << std::endl;
    } else {
        std::cerr << leet::Error;
    }
}
