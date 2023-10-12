/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

/* Returns an array of all channels */
std::vector<std::string> leet::returnRooms(leet::User::CredentialsResponse *resp) {
    using json = nlohmann::json;
    std::vector<std::string> vector;

    const std::string Output = leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v3/joined_rooms"), resp->AccessToken);
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
