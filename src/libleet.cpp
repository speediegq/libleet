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
#include <regex>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>

#include "libleet.hpp"

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
    leet::MatrixOption.Credentials.Username = cred->Username;
    leet::MatrixOption.Credentials.Password = cred->Password;
    leet::MatrixOption.Credentials.DisplayName = cred->DisplayName;
    leet::MatrixOption.Credentials.DeviceID = cred->DeviceID;
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

std::string leet::invoke(const std::string URL, const std::string Data) {
    return cpr::Post(cpr::Url{URL}, cpr::Body{Data}).text;
}

leet::User::CredentialsResponse leet::connectHomeserver() {
    leet::User::CredentialsResponse resp;
    using json = nlohmann::json;

    std::string actualType{};

    if (leet::MatrixOption.Credentials.Type == TPassword) {
        actualType = "m.login.password";
    } else if (leet::MatrixOption.Credentials.Type == TToken) {
        actualType = "m.login.token";
    }

    json list = {
        { "device_id", leet::MatrixOption.Credentials.DeviceID },
        { "identifier", {
            { "type", "m.id.user" },
            { "user", leet::MatrixOption.Credentials.Username },
        } },
        { "initial_device_display_name", leet::MatrixOption.Credentials.DisplayName },
        { "password", leet::MatrixOption.Credentials.Password },
        { "refresh_token", leet::MatrixOption.Credentials.RefreshToken },
        { "type", actualType },
    };

    /* Make a network request attempting a login */
    json loginOutput = {
        json::parse(leet::invoke(leet::getAPI("/_matrix/client/v3/login"), list.dump()))
    };

    for (auto &output : loginOutput) {
        leet::errorCode = 0;

        resp.Homeserver = leet::MatrixOption.Homeserver;

        if (output["access_token"].is_string()) resp.AccessToken = output["access_token"].get<std::string>();
        if (output["device_id"].is_string()) resp.DeviceID = output["device_id"].get<std::string>();
        if (output["refresh_token"].is_string()) resp.RefreshToken = output["refresh_token"].get<std::string>();
        if (output["user_id"].is_string()) resp.UserID = output["user_id"].get<std::string>();

        if (output["errcode"].is_string()) {
            leet::errorCode = 1;
            leet::Error = output["errcode"].get<std::string>();
            if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
        }
    }

    return resp;
}

/* Returns an array of all channels */
std::vector<std::string> leet::returnRooms() {
    using json = nlohmann::json;
    std::vector<std::string> vector;

    const std::string Output = cpr::Get(cpr::Url{ leet::getAPI("/_matrix/client/v3/joined_rooms") }, cpr::Header{{ "Authorization", "Bearer " + leet::MatrixOption.CredentialsResponse.AccessToken }}).text;
    auto returnOutput = json::parse(Output);

    returnOutput["joined_rooms"].get_to(vector);

    return vector;
}

/* Converts an alias to a proper room ID */
std::string leet::findRoomID(std::string Alias) {
    using json = nlohmann::json;

    leet::errorCode = 0;

    // Replace the '#' character with '%23' so that Matrix is happy
    Alias.replace(0, 1, "%23");

    const std::string Output = cpr::Get(cpr::Url{ leet::getAPI("/_matrix/client/v3/directory/room/") + Alias }).text;
    json reqOutput = { json::parse(Output) };

    for (auto &output : reqOutput) {
        if (output["room_id"].is_string()) {
            leet::errorCode = 0;
            return output["room_id"].get<std::string>();
        } else if (!output["errcode"].is_null()) {
            leet::errorCode = 1;
            leet::Error = output["errcode"].get<std::string>();
            if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();

            return "";
        }
    }

    return "";
}

/* Simply returns a new transaction ID */
int leet::generateTransID() {
    return ++leet::TransID;
}

void leet::setRoom(const std::string Room) {
    leet::MatrixOption.activeRoom.RoomID = Room;
}

void leet::sendSimpleMessage(leet::User::CredentialsResponse *resp, const std::string Message) {
    using json = nlohmann::json;
    const int TransID { leet::generateTransID() };
    const std::string RoomID { leet::MatrixOption.activeRoom.RoomID };
    const std::string eventType { "m.room.message" };
    const std::string APIUrl { "/_matrix/client/v3/rooms/" + RoomID + "/send/" + eventType + "/" + std::to_string(TransID) };

    json list = {
        { "body", Message },
        { "msgtype", "m.text" },
    };

    std::string Output { cpr::Put(cpr::Url{ leet::getAPI(APIUrl) }, cpr::Body{ list.dump() }, cpr::Header{{ "Authorization", "Bearer " + resp->AccessToken }}).text };

    /* Make a network request attempting a login */
    json reqOutput = {
        json::parse(Output)
    };

    for (auto &output : reqOutput) {
        leet::errorCode = 0;

        if (output["errcode"].is_string()) {
            leet::errorCode = 1;
            leet::Error = output["errcode"].get<std::string>();
            if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
        }
    }
}
