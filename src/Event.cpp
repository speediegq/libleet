/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU Affero General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

const int32_t leet::returnUnixTimestamp() {
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

leet::Event::Event leet::returnEventFromTimestamp(leet::User::credentialsResponse* resp, leet::Room::Room* room, const int32_t Timestamp, const bool Direction) {
    leet::Event::Event event;
    std::string Dir = Direction ? "f" : "b";

    const std::string Output = leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v1/rooms/" + room->roomID + "/timestamp_to_event" + "?ts=" + std::to_string(Timestamp) + "&dir=" + Dir), resp->accessToken);
    nlohmann::json reqOutput;

    try {
        reqOutput = { nlohmann::json::parse(Output) };
    } catch (const nlohmann::json::parse_error& e) {
        return event;
    }

    for (auto& output : reqOutput) {
        leet::errorCode = 0;

        if (output["event_id"].is_string()) event.eventID = output["event_id"].get<std::string>();
        if (output["origin_server_ts"].is_number_integer()) event.Age = output["origin_server_ts"].get<int>();

        if (output["errcode"].is_string()) {
            leet::errorCode = 1;
            leet::Error = output["errcode"].get<std::string>();
            if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
            break;
        }
    }

    return event;
}

leet::Event::Event leet::returnLatestEvent(leet::User::credentialsResponse* resp, leet::Room::Room* room) {
    return leet::returnEventFromTimestamp(resp, room, leet::returnUnixTimestamp(), true);
}

void leet::redactEvent(leet::User::credentialsResponse* resp, leet::Room::Room* room, leet::Event::Event* event, const std::string& Reason) {

    nlohmann::json body;

    if (Reason.compare("")) {
        body["reason"] = Reason;
    }

    const std::string Output { leet::invokeRequest_Put(leet::getAPI("/_matrix/client/v3/rooms/" + room->roomID + "/redact/" + event->eventID + "/" + std::to_string(leet::transID)), body.dump(), resp->accessToken) };

    nlohmann::json reqOutput;

    try {
        reqOutput = { nlohmann::json::parse(Output) };
    } catch (const nlohmann::json::parse_error& e) {
        return;
    }

    for (auto& output : reqOutput) {
        leet::errorCode = 0;

        if (output["errcode"].is_string()) {
            leet::errorCode = 1;
            leet::Error = output["errcode"].get<std::string>();
            if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
        }
    }

}

void leet::reportEvent(leet::User::credentialsResponse* resp, leet::Room::Room* room, leet::Event::Event* event, const std::string& Reason, const int Score) {
    const std::string APIUrl { "/_matrix/client/v3/rooms/" + room->roomID + "/report/" + event->eventID };

    nlohmann::json body;

    body["reason"] = Reason;
    if (Score > 0 || Score < -100) {
        body["score"] = 0;
    } else {
        body["score"] = Score;
    }

    const std::string Output { leet::invokeRequest_Post(leet::getAPI(APIUrl), body.dump(), resp->accessToken) };

    nlohmann::json reqOutput;

    try {
        reqOutput = { nlohmann::json::parse(Output) };
    }  catch (const nlohmann::json::parse_error& e) {
        return;
    }

    for (auto& output : reqOutput) {
        leet::errorCode = 0;

        if (output["errcode"].is_string()) leet::Error = output["errcode"].get<std::string>();
        if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
    }
}
