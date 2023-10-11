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
std::string leet::getAPI(std::string api) {
    std::ostringstream url;
    url << leet::MatrixOption.Homeserver << api;

    return url.str();
}

std::string leet::invoke(std::string URL, std::string Data) {
    return cpr::Post(cpr::Url{URL}, cpr::Body{Data}).text;
}

/* Returns an object containing the response. */
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
        //{ "token", ... },
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
            resp.Error = output["errcode"].get<std::string>();
        }
    }

    return resp;
}

/* Returns an array of all channels */
std::vector<std::string> leet::returnRooms(leet::User::CredentialsResponse *resp) {
    using json = nlohmann::json;
    std::vector<std::string> vector;

    const std::string Output = cpr::Get(cpr::Url{ leet::getAPI("/_matrix/client/v3/joined_rooms") }, cpr::Header{{ "Authorization", "Bearer " + resp->AccessToken }}).text;
    auto returnOutput = json::parse(Output);

    returnOutput["joined_rooms"].get_to(vector);

    return vector;
}
