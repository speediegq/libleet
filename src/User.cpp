/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

/* Converts a username to a proper user ID if necessary (i.e. speedie -> @speedie:matrix.org) */
std::string leet::finduserID(const std::string Alias) {
    if (Alias[0] != '@')
        return "@" + Alias + ":" + leet::defaultHomeserver;
    return Alias;
}

/* Returns a leet::User::Profile class containing things such as display name and avatar URL */
leet::User::Profile leet::getUserData(const std::string userID) {
    using json = nlohmann::json;
    leet::errorCode = 0;
    leet::User::Profile profile;

    if (userID[0] != '@') {
        return profile;
    }

    profile.userID = leet::finduserID(userID);

    if (profile.userID.empty()) {
        leet::errorCode = 1;
        leet::friendlyError = "Failed to get User ID";
        return profile;
    }

    const std::string API { leet::getAPI("/_matrix/client/v3/profile/" + profile.userID) };
    const std::string Output = invokeRequest_Get(API);

    json reqOutput = { json::parse(Output) };

    for (auto &output : reqOutput) {
        if (output["avatar_url"].is_string()) profile.avatarURL = output["avatar_url"].get<std::string>();
        if (output["displayname"].is_string()) profile.displayName = output["displayname"].get<std::string>();

        if (output["errcode"].is_string()) {
            leet::errorCode = 1;
            leet::Error = output["errcode"].get<std::string>();
            if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
        }
    }

    return profile;
}

/* Returns true if username is valid and available for registering */
bool leet::checkIfUsernameIsAvailable(const std::string Username) {
    using json = nlohmann::json;
    leet::errorCode = 0;
    const std::string API { leet::getAPI("/_matrix/client/v3/register/available?username=" + Username) };
    const std::string Output = invokeRequest_Get(API);
    json reqOutput = { json::parse(Output) };

    for (auto &output : reqOutput) {
        if (output["errcode"].is_string()) {
            leet::errorCode = 1;
            leet::Error = output["errcode"].get<std::string>();
            if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
            return false;
        }

        if (output["available"].is_boolean()) {
            return output["available"].get<bool>();
        }
    }

    return false;
}

/* Returns an array of all users in a room */
std::vector<leet::User::Profile> leet::returnUsersInRoom(leet::User::CredentialsResponse *resp, const std::string RoomID) {
    using json = nlohmann::json;

    const std::string Output = leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v3/rooms/" + RoomID + "/joined_members"), resp->accessToken);
    json returnOutput = json::parse(Output);

    std::vector<leet::User::Profile> vector;

    auto &users = returnOutput["joined"];

    for (auto currKey = users.begin(); currKey != users.end(); ++currKey) {
        leet::User::Profile profile;

        if (currKey.value().contains("avatar_url")) profile.avatarURL = currKey.value()["display_name"];
        if (currKey.value().contains("display_name")) profile.displayName = currKey.value()["display_name"];
        profile.userID = currKey.key();

        vector.push_back(profile);
    }

    return vector;
}

/* Returns an array of all users in activeRoom */
std::vector<leet::User::Profile> returnUsersInRoom(leet::User::CredentialsResponse *resp) {
    return leet::returnUsersInRoom(resp, leet::MatrixOption.activeRoom.RoomID);
}
