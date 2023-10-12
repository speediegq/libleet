#include <iostream>
#include <libleet/libleet.hpp>

int main(int argc, char** argv) {
    leet::MatrixOptions options;
    options.Homeserver = "https://matrix.org";

    leet::setSettings(&options);

    std::string str{};
    std::cout << "Enter a matrix channel:\n> ";
    std::getline(std::cin, str);

    str = leet::findRoomID(str);

    if (leet::errorCode == 0) {
        std::cout << "The room ID is: " << str << std::endl;
    } else {
        std::cout << leet::Error;
    }
}
