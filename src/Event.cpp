/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU Affero General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

const int64_t leet::returnUnixTimestamp() {
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

leet::Event::Event leet::returnEventFromTimestamp(leet::User::credentialsResponse* resp, leet::Room::Room* room, const int64_t Timestamp, const bool Direction) {
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

void leet::sendMessage(leet::User::credentialsResponse* resp, leet::Room::Room* room, leet::Event::Message* msg) {
    const int transID { leet::transID };
    const std::string eventType { "m.room.message" };
    const std::string APIUrl { "/_matrix/client/v3/rooms/" + room->roomID + "/send/" + eventType + "/" + std::to_string(transID) };

    nlohmann::json list;

    if (!msg->messageType.compare("m.image") || !msg->messageType.compare("m.audio") || !msg->messageType.compare("m.video") || !msg->messageType.compare("m.file")) {
        if (msg->attachmentURL[0] != 'm' || msg->attachmentURL[1] != 'x' || msg->attachmentURL[2] != 'c') {
            leet::errorCode = 1;
            return;
        }

        list["type"] = "m.room.message";
        list["room_id"] = room->roomID;
        list["body"] = msg->messageText;
        list["msgtype"] = msg->messageType;
        list["url"] = msg->attachmentURL;
    } else {
        list["type"] = "m.room.message";
        list["room_id"] = room->roomID;
        list["body"] = msg->messageText;
        list["msgtype"] = msg->messageType;
    }

    const std::string Output { leet::invokeRequest_Put(leet::getAPI(APIUrl), list.dump(), resp->accessToken) };

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

#ifndef LEET_NO_ENCRYPTION
void leet::sendEncryptedMessage(leet::User::credentialsResponse* resp, leet::Encryption* enc, leet::Room::Room* room, leet::Event::Message* msg) {
    const int transID { leet::transID };
    const std::string eventType { "m.room.encrypted" };
    const std::string APIUrl { "/_matrix/client/v3/rooms/" + room->roomID + "/send/" + eventType + "/" + std::to_string(transID) };

    nlohmann::json Body;

    Body["type"] = "m.room.message";
    Body["room_id"] = room->roomID;
    Body["content"]["body"] = msg->messageText;
    Body["content"]["msgtype"] = "m.text";

    const std::string Output { leet::invokeRequest_Put(leet::getAPI(APIUrl), enc->account.encryptMessage(resp, Body.dump()), resp->accessToken) };

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
#endif

const std::vector<leet::Event::Message> leet::returnMessages(leet::User::credentialsResponse* resp, leet::Room::Room* room, const int messageCount) {
    std::vector<leet::Event::Message> vector;
    const std::string APIUrl { "/_matrix/client/v3/rooms/" + room->roomID + "/messages?dir=b&limit=" + std::to_string(messageCount) };

    std::string Output { leet::invokeRequest_Get(leet::getAPI(APIUrl), resp->accessToken) };

    nlohmann::json reqOutput;
    try {
        reqOutput = nlohmann::json::parse(Output);
    } catch (const nlohmann::json::parse_error& e) {
        return vector;
    }

    auto& messages = reqOutput["chunk"];

    for (auto it = messages.begin(); it != messages.end(); ++it) {
        leet::Event::Message message;

        message.Encrypted = false;

        if (it.value().contains("/type"_json_pointer)) message.Type = it.value()["type"];

        // Encrypted message
        if (!message.Type.compare("m.room.encrypted")) {
            message.Encrypted = true;
            message.megolm = false;

            if (it.value().contains("/content/ciphertext"_json_pointer)) message.cipherText = it.value()["content"]["ciphertext"];
            if (it.value().contains("/content/sender_key"_json_pointer)) message.senderKey = it.value()["content"]["sender_key"];
            if (it.value().contains("/content/device_id"_json_pointer)) message.deviceID = it.value()["content"]["device_id"];
            if (it.value().contains("/content/session_id"_json_pointer)) message.sessionID = it.value()["content"]["session_id"];
            if (it.value().contains("/content/algorithm"_json_pointer)) if (it.value()["content"]["algorithm"] == "m.megolm.v1.aes-sha2") message.megolm = true;
        }

        if (it.value().contains("/content/msgtype"_json_pointer)) message.messageType = it.value()["content"]["msgtype"];
        if (it.value().contains("/sender"_json_pointer)) message.Sender = it.value()["sender"];

        if (it.value().contains("/content/body"_json_pointer)) message.messageText = it.value()["content"]["body"];
        if (it.value().contains("/content/formatted_body"_json_pointer)) message.formattedText = it.value()["content"]["formatted_body"];
        if (it.value().contains("/content/format"_json_pointer)) message.Format = it.value()["content"]["format"];
        if (it.value().contains("/content/info/mimetype"_json_pointer)) message.mimeType = it.value()["content"]["info"]["mimetype"];
        if (it.value().contains("/event_id"_json_pointer)) message.eventID = it.value()["event_id"];
        if (it.value().contains("/origin_server_ts"_json_pointer)) message.Age = it.value()["origin_server_ts"];

        // Attachments
        if (it.value().contains("/content/info/size"_json_pointer)) message.attachmentSize = it.value()["content"]["info"]["size"];
        if (it.value().contains("/content/info/duration"_json_pointer)) message.attachmentLength = it.value()["content"]["info"]["duration"];
        if (it.value().contains("/content/info/w"_json_pointer)) message.attachmentWidth = it.value()["content"]["info"]["w"];
        if (it.value().contains("/content/info/h"_json_pointer)) message.attachmentHeight = it.value()["content"]["info"]["h"];
        if (it.value().contains("/content/url"_json_pointer)) message.attachmentURL = it.value()["content"]["url"];

        // Handle thumbnails
        if (!message.messageType.compare("m.video")) {
            if (it.value().contains("/content/info/thumbnail_info/w"_json_pointer)) message.thumbnailWidth = it.value()["content"]["info"]["thumbnail_info"]["w"];
            if (it.value().contains("/content/info/thumbnail_info/h"_json_pointer)) message.thumbnailHeight = it.value()["content"]["info"]["thumbnail_info"]["h"];
            if (it.value().contains("/content/info/thumbnail_info/size"_json_pointer)) message.thumbnailSize = it.value()["content"]["info"]["thumbnail_info"]["size"];
            if (it.value().contains("/content/info/thumbnail_info/mimetype"_json_pointer)) message.thumbnailMimeType = it.value()["content"]["info"]["thumbnail_info"]["mimetype"];
            if (it.value().contains("/content/info/thumbnail_url"_json_pointer)) message.thumbnailURL = it.value()["content"]["info"]["thumbnail_url"];
        }

        vector.push_back(message);
    }

    return vector;
}

leet::Filter::Filter leet::returnFilter(leet::User::credentialsResponse* resp, leet::Filter::filterConfiguration *filter) {
    leet::Filter::Filter retFilter;
    const std::string APIUrl { "/_matrix/client/v3/user/" + resp->userID + "/filter" };

    nlohmann::json list;

    list["event_format"] = "client";
    list["event_fields"] = filter->Fields;
    list["presence"]["senders"] = filter->Senders;
    list["presence"]["not_senders"] = filter->notSenders;
    list["room"]["ephemeral"]["rooms"] = filter->Rooms;
    list["room"]["ephemeral"]["not_rooms"] = filter->notRooms;
    list["room"]["ephemeral"]["senders"] = filter->Senders;
    list["room"]["ephemeral"]["not_senders"] = filter->notSenders;
    list["room"]["state"]["rooms"] = filter->Rooms;
    list["room"]["state"]["not_rooms"] = filter->notRooms;

    if (filter->Limit != 0) {
        list["room"]["timeline"]["limit"] = filter->Limit;
    }

    list["room"]["timeline"]["not_rooms"] = filter->notRooms;
    list["room"]["timeline"]["not_senders"] = filter->notSenders;

    std::string Output { leet::invokeRequest_Post(leet::getAPI(APIUrl), list.dump(), resp->accessToken) };

    nlohmann::json reqOutput;
    try {
        reqOutput = nlohmann::json::parse(Output);
    } catch (const nlohmann::json::parse_error& e) {
        return retFilter;
    }

    for (auto& output : reqOutput) {
        leet::errorCode = 0;

        if (output["filter_id"].is_string()) {
            retFilter.filterID = output["filter_id"].get<std::string>();
            return retFilter;
        }
    }

    return retFilter;
}
