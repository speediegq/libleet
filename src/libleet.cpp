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

#include "../include/libleet.hpp"
#include "Login.cpp"
#include "Request.cpp"
#include "User.cpp"
#include "Room.cpp"
#include "Message.cpp"
#include "File.cpp"
#include "Event.cpp"
#include "Voip.cpp"
#include "Encryption.cpp"

const std::string leet::getAPI(const std::string& API) {
    return leet::Homeserver + API;
}

const int leet::generateTransID() {
    return ++leet::transID;
}

const std::string leet::returnServerDiscovery(std::string Server) {
    using json = nlohmann::json;
    leet::errorCode = 0;

    if (Server[0] != 'h' || Server[1] != 't' || Server[2] != 't' || Server[3] != 'p') {
        Server = "https://" + Server;
    }

    const std::string Output = leet::invokeRequest_Get(Server + "/.well-known/matrix/client");

    if (json::accept(Output)) {
        json reqOutput;

        try {
            reqOutput = { json::parse(Output) };
        } catch (const json::parse_error& e) {
            return Server;
        }

        for (auto& output : reqOutput)
            if (output["m.homeserver"]["base_url"].is_string())
                return output["m.homeserver"]["base_url"].get<std::string>();
    }

    return Server;
}

const std::string leet::returnHomeServerFromString(const std::string& userID) {
    std::string uid{userID};
    if (uid[0] != '@') {
        leet::errorCode = 1;
        return "";
    }

    std::size_t colonPosition = uid.find(':');
    if (colonPosition != std::string::npos) return uid.substr(colonPosition + 1);

    return "";
}

std::vector<std::string> leet::returnSupportedSpecs() {
    using json = nlohmann::json;
    std::vector<std::string> vector;
    const std::string APIUrl { "/_matrix/client/versions" };

    std::string Output { leet::invokeRequest_Get(leet::getAPI(APIUrl)) };

    json reqOutput;

    try {
        reqOutput = { json::parse(Output) };
    } catch (const json::parse_error& e) {
        return vector;
    }

    for (auto& output : reqOutput) {
        leet::errorCode = 0;

        if (output["versions"].is_array()) {
            vector = output["versions"];
            break;
        }
    }

    return vector;
}

const int leet::returnMaxUploadLimit(leet::User::credentialsResponse* resp) {
    using json = nlohmann::json;
    const std::string APIUrl { "/_matrix/media/v3/config" };
    const std::string Output { leet::invokeRequest_Get(leet::getAPI(APIUrl), resp->accessToken) };

    json reqOutput;

    try {
        reqOutput = { json::parse(Output) };
    }  catch (const json::parse_error& e) {
        return 0;
    }

    for (auto& output : reqOutput) {
        leet::errorCode = 0;

        if (output["m.upload.size"].is_number_integer()) return output["m.upload.size"].get<int>();
        if (output["errcode"].is_string()) leet::Error = output["errcode"].get<std::string>();
        if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
    }

    return 0;
}
