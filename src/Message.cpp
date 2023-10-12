/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

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

    std::string Output { invokeRequest_Put(leet::getAPI(APIUrl), list.dump(), resp->AccessToken) };

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
