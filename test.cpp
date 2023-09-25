#include <iostream>
#include "src/libleet.hpp"
#include "src/libleet.cpp"

int main() {
    leet::MatrixOptions options;
    options.Homeserver = "https://matrix.org";
    leet::setSettings(&options);

    std::cout << "Home server: " << leet::MatrixOption.Homeserver << "\n";
}
