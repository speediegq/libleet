/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

#include <iostream>
#include <sstream>
#include <future>
#include <vector>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>

#include "libleet.hpp"

/* Handy function, because typing it out gets rather tiring. */
std::string leet::reportError() {
    return leet::ServerResponse.Error;
}

/* Apply settings */
void leet::setSettings(leet::MatrixOptions *options) {
    leet::MatrixOption.Homeserver = options->Homeserver;
    leet::MatrixOption.Type = options->Type;
}

/* std::string leet::User::Credentials.Username = <username>
 * std::string leet::User::Credentials.Password = <password>
 * std::string leet::User::Credentialss.DisplayName = <display name>
 *
 * leet::User::Credentials passed to function.
 */
void leet::saveCredentials(leet::User::Credentials *cred) {
    leet::Credentials.Username = cred->Username;
    leet::Credentials.Password = cred->Password;
    leet::Credentials.DisplayName = cred->DisplayName;
    leet::Credentials.DeviceID = cred->DeviceID;
}

/* This function reverts the changes made by the above function */
void leet::clearUserCredentials() {
    leet::Credentials.Username = "";
    leet::Credentials.Password = "";
    leet::Credentials.DisplayName = "";
    leet::Credentials.DeviceID = "";
}

/* Login API: /_matrix/client/v3/login */
std::string leet::getAPI(const std::string api) {
    std::ostringstream url;
    url << leet::MatrixOption.Homeserver << api;

    return url.str();
}

std::string leet::invoke(const std::string URL, const std::string Data) {
    return cpr::Post(cpr::Url{URL}, cpr::Body{Data}).text;
}

void leet::connectHomeserver() {
    using json = nlohmann::json;

    std::string actualType{};

    if (leet::Credentials.Type == TPassword) {
        actualType = "m.login.password";
    } else if (leet::Credentials.Type == TToken) {
        actualType = "m.login.token";
    }

    json list = {
        { "device_id", leet::Credentials.DeviceID },
        { "identifier", {
            { "type", "m.id.user" },
            { "user", leet::Credentials.Username },
        } },
        { "initial_device_display_name", leet::Credentials.DisplayName },
        { "password", leet::Credentials.Password },
        { "refresh_token", leet::Credentials.RefreshToken },
        { "type", actualType },
    };

    /* Make a network request attempting a login */
    json loginOutput = {
        json::parse(leet::invoke(leet::getAPI("/_matrix/client/v3/login"), list.dump()))
    };

    for (auto &output : loginOutput) {
        leet::errorCode = 0;

        leet::ServerResponse.Homeserver = leet::MatrixOption.Homeserver;

        if (output["access_token"].is_string()) leet::ServerResponse.AccessToken = output["access_token"].get<std::string>();
        if (output["device_id"].is_string()) leet::ServerResponse.DeviceID = output["device_id"].get<std::string>();
        if (output["refresh_token"].is_string()) leet::ServerResponse.RefreshToken = output["refresh_token"].get<std::string>();
        if (output["user_id"].is_string()) leet::ServerResponse.UserID = output["user_id"].get<std::string>();

        if (output["errcode"].is_string()) {
            leet::errorCode = 1;
            leet::ServerResponse.Error = output["errcode"].get<std::string>();
        }
    }
}

/* Returns an array of all channels */
std::vector<std::string> leet::returnRooms() {
    using json = nlohmann::json;
    std::vector<std::string> vector;

    const std::string Output = cpr::Get(cpr::Url{ leet::getAPI("/_matrix/client/v3/joined_rooms") }, cpr::Header{{ "Authorization", "Bearer " + leet::ServerResponse.AccessToken }}).text;
    auto returnOutput = json::parse(Output);

    returnOutput["joined_rooms"].get_to(vector);

    return vector;
}

/* Returns a room ID from room Alias (i.e. #speedie:matrix.org -> !OGNyGIKFSskVdwouMg:matrix.org) */
std::string leet::findRoomID(const std::string roomAlias) {
    // TODO
    // https://playground.matrix.org/#get-/_matrix/client/v3/directory/room/-roomAlias-
}

/* Simply returns a new transaction ID */
int leet::generateTransID() {
    return ++leet::TransID;
}

void leet::setRoom(const std::string Room) {
    leet::activeRoom.RoomID = Room;
}

void leet::sendSimpleMessage(const std::string Message) {
    using json = nlohmann::json;
    const int TransID { leet::generateTransID() };
    const std::string RoomID { leet::activeRoom.RoomID };
    const std::string eventType { "m.room.message" };
    const std::string APIUrl { "/_matrix/client/v3/rooms/" + RoomID + "/send/" + eventType + "/" + std::to_string(TransID) };

    json list = {
        { "body", Message },
        { "msgtype", "m.text" },
    };

    std::string Output { cpr::Put(cpr::Url{ leet::getAPI(APIUrl) }, cpr::Body{ list.dump() }, cpr::Header{{ "Authorization", "Bearer " + leet::ServerResponse.AccessToken }}).text };

    /* Make a network request attempting a login */
    json reqOutput = {
        json::parse(Output)
    };

    for (auto &output : reqOutput) {
        leet::errorCode = 0;

        if (output["errcode"].is_string()) {
            leet::errorCode = 1;
            leet::ServerResponse.Error = output["errcode"].get<std::string>();
        }
    }
}
