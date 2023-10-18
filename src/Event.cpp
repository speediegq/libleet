/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU Affero General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

int32_t leet::returnUnixTimestamp() {
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

leet::Event::Event leet::returnEventFromTimestamp(leet::User::CredentialsResponse *resp, const std::string RoomID, const int32_t Timestamp, const bool Direction) {
    using json = nlohmann::json;
    leet::Event::Event event;
    std::string Dir = Direction ? "f" : "b";

    const std::string Output = leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v1/rooms/" + RoomID + "/timestamp_to_event" + "?ts=" + std::to_string(Timestamp) + "&dir=" + Dir), resp->accessToken);
    json reqOutput = { json::parse(Output) };

    for (auto &output : reqOutput) {
        leet::errorCode = 0;

        if (output["event_id"].is_string()) event.eventID = output["event_id"].get<std::string>();
        if (output["origin_server_ts"].is_number_integer()) event.Age = output["origin_server_ts"].get<int>();

        if (output["errcode"].is_string()) {
            leet::errorCode = 1;
            leet::Error = output["errcode"].get<std::string>();
            if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
        }
    }

    return event;
}

leet::Event::Event leet::returnLatestEvent(leet::User::CredentialsResponse *resp, const std::string RoomID) {
    return leet::returnEventFromTimestamp(resp, RoomID, leet::returnUnixTimestamp(), true);
}
