/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

/* This is a simple function used to send a message. While it works, you should probably
 * not use it outside of a test environment.
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

    json reqOutput = { json::parse(Output) };

    for (auto &output : reqOutput) {
        leet::errorCode = 0;

        if (output["errcode"].is_string()) {
            leet::errorCode = 1;
            leet::Error = output["errcode"].get<std::string>();
            if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
        }
    }
}

void leet::sendMessage(leet::User::CredentialsResponse *resp, leet::Message::Message *msg) {
    using json = nlohmann::json;
    const int TransID { leet::generateTransID() };
    const std::string RoomID { leet::MatrixOption.activeRoom.RoomID };
    const std::string eventType { "m.room.message" };
    const std::string APIUrl { "/_matrix/client/v3/rooms/" + RoomID + "/send/" + eventType + "/" + std::to_string(TransID) };

    json list = {
        { "body", msg->messageText },
        { "msgtype", "m.text" },
    };

    std::string Output { invokeRequest_Put(leet::getAPI(APIUrl), list.dump(), resp->AccessToken) };

    json reqOutput = { json::parse(Output) };

    for (auto &output : reqOutput) {
        leet::errorCode = 0;

        if (output["errcode"].is_string()) {
            leet::errorCode = 1;
            leet::Error = output["errcode"].get<std::string>();
            if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
        }
    }
}

std::vector<leet::Message::Message> leet::returnMessages(leet::User::CredentialsResponse *resp, const int messageCount) {
    using json = nlohmann::json;
    std::vector<leet::Message::Message> vector;
    const std::string RoomID { leet::MatrixOption.activeRoom.RoomID };
    const std::string APIUrl { "/_matrix/client/v3/rooms/" + RoomID + "/messages?dir=b&limit=" + std::to_string(messageCount) };

    std::string Output { invokeRequest_Get(leet::getAPI(APIUrl), resp->AccessToken) };

    json reqOutput = json::parse(Output);

    auto &messages = reqOutput["chunk"];

    for (auto currKey = messages.begin(); currKey != messages.end(); ++currKey) {
        leet::Message::Message message;

        if (currKey.value().contains("/content/msgtype"_json_pointer)) message.messageType = currKey.value()["content"]["msgtype"];
        if (currKey.value().contains("/type"_json_pointer)) message.Type = currKey.value()["type"];
        if (currKey.value().contains("/sender"_json_pointer)) message.Sender = currKey.value()["sender"];

        if (currKey.value().contains("/content/body"_json_pointer)) message.messageText = currKey.value()["content"]["body"];
        if (currKey.value().contains("/content/formatted_body"_json_pointer)) message.formattedText = currKey.value()["content"]["formatted_body"];
        if (currKey.value().contains("/content/format"_json_pointer)) message.Format = currKey.value()["content"]["format"];
        if (currKey.value().contains("/content/info/mimetype"_json_pointer)) message.mimeType = currKey.value()["content"]["info"]["mimetype"];
        if (currKey.value().contains("/event_id"_json_pointer)) message.eventID = currKey.value()["event_id"];
        if (currKey.value().contains("/origin_server_ts"_json_pointer)) message.Age = currKey.value()["origin_server_ts"];

        // attachments
        if (currKey.value().contains("/content/info/size"_json_pointer)) message.attachmentSize = currKey.value()["content"]["info"]["size"];
        if (currKey.value().contains("/content/info/w"_json_pointer)) message.attachmentWidth = currKey.value()["content"]["info"]["w"];
        if (currKey.value().contains("/content/info/h"_json_pointer)) message.attachmentHeight = currKey.value()["content"]["info"]["h"];
        if (currKey.value().contains("/content/url"_json_pointer)) message.attachmentURL = currKey.value()["content"]["url"];

        if (!message.messageType.compare("m.video")) {
            if (currKey.value().contains("/content/info/duration"_json_pointer)) message.videoLength = currKey.value()["content"]["info"]["duration"];

            // thumbnail stuff
            if (currKey.value().contains("/content/info/thumbnail_info/w"_json_pointer)) message.thumbnailWidth = currKey.value()["content"]["info"]["thumbnail_info"]["w"];
            if (currKey.value().contains("/content/info/thumbnail_info/h"_json_pointer)) message.thumbnailHeight = currKey.value()["content"]["info"]["thumbnail_info"]["h"];
            if (currKey.value().contains("/content/info/thumbnail_info/size"_json_pointer)) message.thumbnailSize = currKey.value()["content"]["info"]["thumbnail_info"]["size"];
            if (currKey.value().contains("/content/info/thumbnail_info/mimetype"_json_pointer)) message.thumbnailMimeType = currKey.value()["content"]["info"]["thumbnail_info"]["mimetype"];
            if (currKey.value().contains("/content/info/thumbnail_url"_json_pointer)) message.thumbnailURL = currKey.value()["content"]["info"]["thumbnail_url"];
        }

        vector.push_back(message);
    }

    return vector;
}
