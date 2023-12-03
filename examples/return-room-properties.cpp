/* libleet example
 * ===============
 *
 * This is a libleet example which returns a list of rooms the user is a part of.
 */
#include <iostream>
#include <string>
#include <algorithm>
#include <filesystem>
/* In practice you would:
 * #include <libleet/libleet.hpp>
 * and link with libleet:
 * g++ create-a-room.cpp -o create-a-room -lleet
 */
#include "../include/libleet.hpp"
#include "../src/libleet.cpp"

int checkError() {
    if (leet::errorCode != 0) { // Oh no, failed to login
        std::cerr << "Failed. " << leet::friendlyError << " (" << leet::Error << ")\n"; // leet::friendlyError is a (usually) human-friendly error returned by the Matrix server. leet::Error is a more specific error code
        return 1;
    }
    return 0;
}

int main() {
    leet::User::Credentials cred; // Create a credentials object which we'll pass to the login functionm.

    cred.Identifier = LEET_IDENTIFIER_USERID; // Our identifier. We're using a user ID, not a third party ID or phone number.
    cred.Type = LEET_TYPE_PASSWORD; // Our type. We're authenticating using a password, and that's the only supported method as of now.

    /* Get the username */
    std::cout << "\033[2J\033[1;1H";
    std::cout << "Enter a Matrix username (@<username>:<home server>)\n> ";
    std::getline(std::cin, cred.Username);

    /* Get the password */
    std::cout << "\033[2J\033[1;1H";
    std::cout << "Enter a Matrix password\n> ";
    std::getline(std::cin, cred.Password);

    /* In most Matrix clients, the device ID is simply a six character string of characters.
     * This device ID is most likely issued by the home server, but for this example we explicitly
     * set one for simplicity. If the device ID is not specified like this, the client must store
     * this data in some way, for example by writing to a file and restoring it later.
     */
    cred.deviceID = "libleet test client"; // Our device ID

    /* leet::Homeserver specifies which Matrix server to use when making API requests.
     * In most cases, this should be the same home server as you'd log in with.
     */
    cred.Homeserver = leet::Homeserver; /* matrix.org is the default home server */

    cred.Homeserver = leet::returnServerDiscovery(leet::returnHomeServerFromString(cred.Username));

    leet::User::credentialsResponse resp;

    /* Check if we should consider this an attempt to register. Then we will
     * either log in or register.
     */
    if (leet::checkIfUsernameIsAvailable(cred.Username)) {
        resp = leet::registerAccount(&cred);
    } else {
        resp = leet::loginAccount(&cred);
    }

    /* Now that we don't need the credentials anymore, let's get rid of them for security reasons */
    cred.clearCredentials();

    if (checkError() == true) { // Yeah, appears something went wrong.
        return false;
    }

    leet::Room::roomConfiguration conf;

    std::vector<leet::Room::Room> vector = leet::returnRooms(&resp, 9999);

    for (auto& it : vector) {
        std::cout << it.roomID << "\n";
        std::cout << "\t" << it.Alias << "\n";
        std::cout << "\t" << it.Topic << "\n";
    }
}
