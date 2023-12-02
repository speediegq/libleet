/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU Affero General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

/* Converts a username to a proper user ID if necessary (i.e. speedie -> @speedie:matrix.org) */
const std::string leet::findUserID(const std::string& Alias, const std::string& Homeserver) {
    if (Alias[0] != '@')
        return "@" + Alias + ":" + Homeserver;
    return Alias;
}

/* leet::findUserID in reverse */
const std::string leet::returnUserName(const std::string& userID) {
    std::string str;
    std::regex pattern{"@([^:]+):"};
    std::smatch reg;
    if (std::regex_search(userID, reg, pattern)) return reg[1].str();
    return str;
}

/* Returns a leet::User::Profile class containing things such as display name and avatar URL */
leet::User::Profile leet::getUserData(leet::User::CredentialsResponse* resp, const std::string& userID) {
    using json = nlohmann::json;
    leet::errorCode = 0;
    leet::User::Profile profile;

    if (userID[0] != '@') {
        return profile;
    }

    profile.userID = leet::findUserID(userID, resp->Homeserver);

    if (profile.userID.empty()) {
        leet::errorCode = 1;
        leet::friendlyError = "Failed to get User ID";
        return profile;
    }

    const std::string API { leet::getAPI("/_matrix/client/v3/profile/" + profile.userID) };
    const std::string Output = invokeRequest_Get(API);

    json reqOutput;

    try {
        reqOutput = { json::parse(Output) };
    } catch (const json::parse_error& e) {
        return profile;
    }

    for (auto& output : reqOutput) {
        if (output["avatar_url"].is_string()) profile.avatarURL = output["avatar_url"].get<std::string>();
        if (output["displayname"].is_string()) profile.displayName = output["displayname"].get<std::string>();

        if (output["errcode"].is_string()) {
            leet::errorCode = 1;
            leet::Error = output["errcode"].get<std::string>();
            if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
        }
    }

    leet::User::Profile userProfile;
    userProfile.userID = userID;
    std::vector<leet::User::Profile> User = { userProfile };

    profile.Devices = leet::returnDevicesFromUser(resp, User);

    return profile;
}

/* Returns an array of all devices for a user */
const std::vector<leet::User::Device> leet::returnDevicesFromUser(leet::User::CredentialsResponse* resp, const std::vector<leet::User::Profile>& user) {
    std::vector<leet::User::Device> devices;
    using json = nlohmann::json;

    json Body;
    json deviceKeys;

    for (auto& theUser : user) {
        deviceKeys[theUser.userID] = json::array();
        Body["device_keys"] = deviceKeys;
        Body["timeout"] = 10000;
    }

    const std::string Output = leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/keys/query"), Body.dump(), resp->accessToken);
    json returnOutput;

    try {
        returnOutput = json::parse(Output);
    } catch (const json::parse_error& e) {
        return devices;
    }

    for (auto& userID : user) {
        auto& deviceList = returnOutput["device_keys"][userID.userID];

        for (auto it = deviceList.begin(); it != deviceList.end(); ++it) {
            leet::User::Device device;

            device.userID = userID.userID;
            device.deviceID = it.key();

            json::json_pointer curve25519Pointer("/keys/curve25519:" + device.deviceID);
            if (it.value().contains(curve25519Pointer)) {
                device.curve25519Key = it.value()["keys"]["curve25519:" + device.deviceID];
            }
            json::json_pointer ed25519Pointer("/keys/ed25519:" + device.deviceID);
            if (it.value().contains(ed25519Pointer)) {
                device.ed25519Key = it.value()["keys"]["ed25519:" + device.deviceID];
            }
            json::json_pointer ed25519SigPointer("/signatures/" + userID.userID + "/ed25519:" + device.deviceID);
            if (it.value().contains(ed25519SigPointer)) {
                device.ed25519Signature = it.value()["signatures"][userID.userID]["ed25519:" + device.deviceID];
            }
            if (it.value().contains("/unsigned/device_display_name"_json_pointer)) {
                device.deviceDisplayName = it.value()["unsigned"]["device_display_name"];
            }

            device.olm = false;
            device.megolm = false;

            if (std::find(it.value()["algorithms"].begin(), it.value()["algorithms"].end(), "m.olm.v1.curve25519-aes-sha2") != it.value()["algorithms"].end())
                device.olm = true;

            if (std::find(it.value()["algorithms"].begin(), it.value()["algorithms"].end(), "m.megolm.v1.aes-sha2") != it.value()["algorithms"].end())
                device.megolm = true;

            devices.push_back(device);
        }
    }

    return devices;
}

/* Returns true if username is valid and available for registering */
const bool leet::checkIfUsernameIsAvailable(const std::string& Username) {
    using json = nlohmann::json;
    leet::errorCode = 0;

    std::string theUsername = Username;

    if (Username[0] == '@') {
        theUsername = leet::returnUserName(Username);

        if (theUsername[0] == '@' || !theUsername.compare("")) {
            return false;
            leet::errorCode = 1;
        }
    }

    const std::string API { leet::getAPI("/_matrix/client/v3/register/available?username=" + theUsername) };
    const std::string Output = invokeRequest_Get(API);

    json reqOutput;

    try {
        reqOutput = { json::parse(Output) };
    } catch (const json::parse_error& e) {
        return false; // fallback to false is probably safest?
    }

    for (auto& output : reqOutput) {
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
const std::vector<leet::User::Profile> leet::returnUsersInRoom(leet::User::CredentialsResponse* resp, leet::Room::Room* room) {
    using json = nlohmann::json;
    std::vector<leet::User::Profile> vector;

    const std::string Output = leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v3/rooms/" + room->roomID + "/joined_members"), resp->accessToken);
    json returnOutput;

    try {
        returnOutput = json::parse(Output);
    } catch (const json::parse_error& e) {
        return vector;
    }

    auto& users = returnOutput["joined"];

    for (auto it = users.begin(); it != users.end(); ++it) {
        leet::User::Profile profile;

        if (it.value().contains("avatar_url") && !it.value()["avatar_url"].is_null()) profile.avatarURL = it.value()["avatar_url"];
        if (it.value().contains("display_name") && !it.value()["display_name"].is_null()) profile.displayName = it.value()["display_name"];

        profile.userID = it.key();

        leet::User::Profile userProfile;
        userProfile.userID = it.key();
        std::vector<leet::User::Profile> User = { userProfile };
        profile.Devices = leet::returnDevicesFromUser(resp, User);

        vector.push_back(profile);
    }

    return vector;
}
