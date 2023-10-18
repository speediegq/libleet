/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU Affero General Public License version 3.
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
#include <chrono>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>

#include "libleet.hpp"
#include "Login.cpp"
#include "Request.cpp"
#include "User.cpp"
#include "Room.cpp"
#include "Message.cpp"
#include "File.cpp"
#include "Event.cpp"
#ifdef ENCRYPTION
#include "Encryption.cpp"
#endif

void leet::setSettings(leet::MatrixOptions *options) {
    leet::MatrixOption.Homeserver = options->Homeserver;
    leet::MatrixOption.Type = options->Type;
}

/* std::string leet::User::Credentials.Username = <username>
 * std::string leet::User::Credentials.Password = <password>
 * std::string leet::User::Credentials.displayName = <display name>
 *
 * leet::User::Credentials passed to function.
 */
void leet::saveCredentials(leet::User::Credentials *cred) {
    leet::MatrixOption.Credentials.Username = leet::finduserID(cred->Username);
    leet::MatrixOption.Credentials.Password = cred->Password;
    leet::MatrixOption.Credentials.displayName = cred->displayName;
    leet::MatrixOption.Credentials.deviceID = cred->deviceID;
    leet::MatrixOption.Credentials.Token = cred->Token;
    leet::MatrixOption.Credentials.refreshToken = cred->refreshToken;

    leet::MatrixOption.Profile.userID = leet::finduserID(cred->Username);
    leet::MatrixOption.Profile.displayName = cred->displayName;
}

/* This function reverts the changes made by the above function */
void leet::clearUserCredentials() {
    leet::MatrixOption.Credentials.Username = "";
    leet::MatrixOption.Credentials.Password = "";
    leet::MatrixOption.Credentials.displayName = "";
    leet::MatrixOption.Credentials.deviceID = "";
    leet::MatrixOption.Credentials.Token = "";
    leet::MatrixOption.Credentials.refreshToken = "";
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

/* Finds a server using discovery */
std::string leet::returnServerDiscovery(std::string Server) {
    using json = nlohmann::json;
    leet::errorCode = 0;

    if (Server[0] != 'h' || Server[1] != 't' || Server[2] != 't' || Server[3] != 'p') {
        Server = "https://" + Server;
    }

    const std::string Output = leet::invokeRequest_Get(Server + "/.well-known/matrix/client");

    json reqOutput = { json::parse(Output) };

    for (auto &output : reqOutput) if (output["m.homeserver"]["base_url"].is_string()) return output["m.homeserver"]["base_url"].get<std::string>();

    leet::errorCode = 1;
    return "";
}

/* Returns the home server from a username string */
std::string leet::returnHomeServerFromString(std::string userID) {
    if (userID[0] != '@') {
        leet::errorCode = 1;
        return "";
    }

    std::size_t colonPosition = userID.find(':');
    if (colonPosition != std::string::npos) return userID.substr(colonPosition + 1);

    return "";
}
