#include <iostream>
#include "src/libleet.hpp"
#include "src/libleet.cpp"

int main() {
    leet::MatrixOptions options;
    leet::User::Credentials cred;
    leet::User::CredentialsResponse resp;

    options.Homeserver = "https://matrix.org";

    /* Here we specify what type of authentication we're using
     * Since we're logging in using a password, the type is TPassword.
     */
    cred.Type = TPassword;

    /* Then, we specify a username here. */
    cred.Username = "speedie"; // Username

    /* Now let's read the password from the user's shell. */
    std::getline(std::cin, cred.Password);

    /* Save the settings, and the credentials using libleet functions */
    leet::setSettings(&options);
    leet::saveCredentials(&cred);

    /* Test print of the home server we're connecting to */
    std::cout << "Home server: " << leet::MatrixOption.Homeserver << "\n";

    /* Connect to the home server using the credentials we saved */
    resp = leet::connectHomeserver();

    /* Now that we have a response, we can simply clear this */

    if (leet::errorCode == 0) {
        leet::clearUserCredentials();

        /* As a test, let's print out our user ID and home server */
        std::cout << "User ID: " << resp.UserID << std::endl << "Homeserver: " << resp.Homeserver << std::endl;
    } else {
        std::cerr << "Failed to authenticate. Invalid username/password (" << resp.Error << ")\n";
        return 1;
    }

    /* From here on, we can use resp.AccessToken to authenticate actions */
}
