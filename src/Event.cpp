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
    using json = nlohmann::json;
    leet::Event::Event event;
    std::string Dir = Direction ? "f" : "b";

    const std::string Output = leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v1/rooms/" + room->roomID + "/timestamp_to_event" + "?ts=" + std::to_string(Timestamp) + "&dir=" + Dir), resp->accessToken);
    json reqOutput;

    try {
        reqOutput = { json::parse(Output) };
    } catch (const json::parse_error& e) {
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

leet::Sync::Sync leet::returnSync(leet::User::credentialsResponse* resp) {
    using json = nlohmann::json;
    leet::Sync::Sync sync;

    const std::string Output = leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v3/sync"), resp->accessToken);
    json theOutput;

    try {
        theOutput = json::parse(Output);
    } catch (const json::parse_error& e) {
        return sync;
    }

    sync.theRequest = theOutput.dump();

    auto& reqOutput = theOutput["to_device"]["events"];

    for (auto& output : reqOutput) {
        leet::errorCode = 0;

        leet::Sync::megolmSession megolmSession;

        if (!output["content"]["sender_key"].is_null()) {
            megolmSession.senderKey = output["content"]["sender_key"];
        }
        if (!output["content"]["algorithm"].is_null()) {
            megolmSession.Algorithm = output["content"]["algorithm"];
        }
        if (megolmSession.senderKey.compare("")) {
            if (!output["content"]["ciphertext"][megolmSession.senderKey]["body"].is_null()) {
                megolmSession.cipherText = output["content"]["ciphertext"][megolmSession.senderKey]["body"];
            }
            if (!output["content"]["ciphertext"][megolmSession.senderKey]["type"].is_null()) {
                megolmSession.cipherType = output["content"]["ciphertext"][megolmSession.senderKey]["type"];
            }
        }
        if (!output["sender"].is_null()) {
            megolmSession.Sender = output["sender"];
        }
        if (!output["type"].is_null()) {
            megolmSession.Type = output["type"];
        }

        sync.megolmSessions.push_back(megolmSession);
    }

    return sync;
}

void leet::redactEvent(leet::User::credentialsResponse* resp, leet::Room::Room* room, leet::Event::Event* event, const std::string& Reason) {
    using json = nlohmann::json;

    json body;

    if (Reason.compare("")) {
        body["reason"] = Reason;
    }

    const std::string Output { leet::invokeRequest_Put(leet::getAPI("/_matrix/client/v3/rooms/" + room->roomID + "/redact/" + event->eventID + "/" + std::to_string(leet::transID)), body.dump(), resp->accessToken) };

    json reqOutput;

    try {
        reqOutput = { json::parse(Output) };
    } catch (const json::parse_error& e) {
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
    using json = nlohmann::json;
    const std::string APIUrl { "/_matrix/client/v3/rooms/" + room->roomID + "/report/" + event->eventID };

    json body;

    body["reason"] = Reason;
    if (Score > 0 || Score < -100) {
        body["score"] = 0;
    } else {
        body["score"] = Score;
    }

    const std::string Output { leet::invokeRequest_Post(leet::getAPI(APIUrl), body.dump(), resp->accessToken) };

    json reqOutput;

    try {
        reqOutput = { json::parse(Output) };
    }  catch (const json::parse_error& e) {
        return;
    }

    for (auto& output : reqOutput) {
        leet::errorCode = 0;

        if (output["errcode"].is_string()) leet::Error = output["errcode"].get<std::string>();
        if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
    }
}
