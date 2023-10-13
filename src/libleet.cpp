/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <future>
#include <vector>
#include <regex>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>

#include "libleet.hpp"
#include "Login.cpp"
#include "Request.cpp"
#include "User.cpp"
#include "Room.cpp"
#include "Message.cpp"
#include "File.cpp"

void leet::setSettings(leet::MatrixOptions *options) {
    leet::MatrixOption.Homeserver = options->Homeserver;
    leet::MatrixOption.Type = options->Type;
}

/* std::string leet::User::Credentials.Username = <username>
 * std::string leet::User::Credentials.Password = <password>
 * std::string leet::User::Credentials.DisplayName = <display name>
 *
 * leet::User::Credentials passed to function.
 */
void leet::saveCredentials(leet::User::Credentials *cred) {
    leet::MatrixOption.Credentials.Username = leet::findUserID(cred->Username);
    leet::MatrixOption.Credentials.Password = cred->Password;
    leet::MatrixOption.Credentials.DisplayName = cred->DisplayName;
    leet::MatrixOption.Credentials.DeviceID = cred->DeviceID;

    leet::MatrixOption.Profile.UserID = leet::findUserID(cred->Username);
    leet::MatrixOption.Profile.DisplayName = cred->DisplayName;
}

/* This function reverts the changes made by the above function */
void leet::clearUserCredentials() {
    leet::MatrixOption.Credentials.Username = "";
    leet::MatrixOption.Credentials.Password = "";
    leet::MatrixOption.Credentials.DisplayName = "";
    leet::MatrixOption.Credentials.DeviceID = "";
}

/* api: /_matrix/client/v3/login */
std::string leet::getAPI(const std::string api) {
    return leet::MatrixOption.Homeserver + api;
}

/* Simply returns a new transaction ID */
int leet::generateTransID() {
    return ++leet::TransID;
}

void leet::setRoom(leet::Room::Room *room) {
    leet::MatrixOption.activeRoom = *room;
}
