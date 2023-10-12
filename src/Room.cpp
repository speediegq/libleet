/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

/* Returns an array of all rooms */
std::vector<leet::Room::Room> leet::returnRooms(leet::User::CredentialsResponse *resp) {
    using json = nlohmann::json;

    std::vector<leet::Room::Room> vector;

    const std::string Output = leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v3/joined_rooms"), resp->AccessToken);
    json returnOutput = json::parse(Output);

    auto &rooms = returnOutput["joined_rooms"];

    for (auto currKey = rooms.begin(); currKey != rooms.end(); ++currKey) {
        leet::Room::Room room;
        room.RoomID = currKey.value();
        vector.push_back(room);
    }

    return vector;
}

/* Returns an array of all users in a room */
std::vector<leet::User::Profile> leet::returnUsersInRoom(leet::User::CredentialsResponse *resp, const std::string RoomID) {
    using json = nlohmann::json;

    const std::string Output = leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v3/rooms/" + RoomID + "/joined_members"), resp->AccessToken);
    json returnOutput = json::parse(Output);

    std::vector<leet::User::Profile> vector;

    auto &users = returnOutput["joined"];

    for (auto currKey = users.begin(); currKey != users.end(); ++currKey) {
        leet::User::Profile profile;

        if (currKey.value().contains("avatar_url")) profile.AvatarURL = currKey.value()["display_name"];
        if (currKey.value().contains("display_name")) profile.DisplayName = currKey.value()["display_name"];
        profile.UserID = currKey.key();

        vector.push_back(profile);
    }

    return vector;
}

/* Returns an array of all users in activeRoom */
std::vector<leet::User::Profile> returnUsersInRoom(leet::User::CredentialsResponse *resp) {
    return leet::returnUsersInRoom(resp, leet::MatrixOption.activeRoom.RoomID);
}

/* Converts an alias to a proper room ID */
std::string leet::findRoomID(std::string Alias) {
    using json = nlohmann::json;

    leet::errorCode = 0;

    // Replace the '#' character with '%23' so that Matrix is happy
    Alias.replace(0, 1, "%23");

    const std::string Output = leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v3/directory/room/") + Alias);
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
