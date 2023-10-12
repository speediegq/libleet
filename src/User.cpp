/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

/* Converts a username to a proper user ID if necessary (i.e. speedie -> @speedie:matrix.org) */
std::string leet::findUserID(const std::string Alias) {
    if (Alias[0] != '@')
        return "@" + Alias + ":" + leet::defaultHomeserver;
    return Alias;
}

/* Returns a leet::User::Profile class containing things such as display name and avatar URL */
leet::User::Profile leet::getUserData(const std::string UserID) {
    using json = nlohmann::json;
    leet::errorCode = 0;

    leet::User::Profile profile;

    profile.UserID = leet::findUserID(UserID);

    if (profile.UserID.empty()) {
        leet::errorCode = 1;
        leet::friendlyError = "Failed to get User ID";
        return profile;
    }

    const std::string API { leet::getAPI("/_matrix/client/v3/profile/" + profile.UserID) };
    const std::string Output = invokeRequest_Get(API);

    json reqOutput = { json::parse(Output) };

    for (auto &output : reqOutput) {
        if (output["avatar_url"].is_string()) profile.AvatarURL = output["avatar_url"].get<std::string>();
        if (output["displayname"].is_string()) profile.DisplayName = output["displayname"].get<std::string>();

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