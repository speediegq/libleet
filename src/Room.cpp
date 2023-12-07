/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU Affero General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

/* Converts an alias to a proper room ID */
const std::string leet::findRoomID(std::string Alias) {
    using json = nlohmann::json;

    leet::errorCode = 0;

    if (Alias[0] == '!') { // It's a proper room ID already
        return Alias;
    }

    // Replace the '#' character with '%23' so that Matrix is happy
    Alias.replace(0, 1, "%23");

    const std::string Output = leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v3/directory/room/") + Alias);
    json reqOutput;

    try {
        reqOutput = { json::parse(Output) };
    } catch (const json::parse_error& e) {
        return "";
    }

    for (auto& output : reqOutput) {
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

/* Returns an array of all rooms */
const std::vector<leet::Room::Room> leet::returnRooms(leet::User::credentialsResponse* resp, const int Limit) {
    using json = nlohmann::json;

    std::vector<leet::Room::Room> vector;
    std::vector<leet::Room::Room> vectorWithVal;

    const std::string Output = leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v3/joined_rooms"), resp->accessToken);
    json returnOutput;

    try {
        returnOutput = json::parse(Output);
    } catch (const json::parse_error& e) {
        return vector;
    }

    auto& rooms = returnOutput["joined_rooms"];

    for (auto it = rooms.begin(); it != rooms.end(); ++it) {
        leet::Room::Room room;
        room.roomID = it.value();
        vector.push_back(room);
    }

    for (auto& it : vector) {
        leet::Room::Room room = leet::returnRoom(resp, &it);

        vectorWithVal.push_back(room);
    }

    return vectorWithVal;
}

/* Returns a single room based on the room ID */
leet::Room::Room leet::returnRoom(leet::User::credentialsResponse* resp, leet::Room::Room* room) {
    using json = nlohmann::json;
    leet::Room::Room theRoom;
    json returnOutput;

    try {
        returnOutput = json::parse(leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v1/rooms/" + room->roomID + "/hierarchy"), resp->accessToken));
    } catch (const json::parse_error& e) {
        return theRoom;
    }

    auto& roomOutput = returnOutput["rooms"];

    for (auto i = roomOutput.begin(); i != roomOutput.end(); ++i) {
        if (i.value().contains("room_id")) theRoom.roomID = i.value()["room_id"];
        if (i.value().contains("join_rule")) theRoom.joinRule = i.value()["join_rule"];
        if (i.value().contains("avatar_url")) theRoom.avatarURL = i.value()["avatar_url"];
        if (i.value().contains("canonical_alias")) theRoom.Alias = i.value()["canonical_alias"];
        if (i.value().contains("name")) theRoom.Name = i.value()["name"];
        if (i.value().contains("num_joined_members")) theRoom.memberCount = i.value()["num_joined_members"];
        if (i.value().contains("topic")) theRoom.Topic = i.value()["topic"];
        if (i.value().contains("guest_can_join")) theRoom.guestCanJoin = i.value()["guest_can_join"];
        if (i.value().contains("world_readable")) theRoom.worldReadable = i.value()["world_readable"];
        if (i.value().contains("room_type")) theRoom.roomType = i.value()["room_type"];
    }

    return theRoom;
}

leet::Room::Room leet::upgradeRoom(leet::User::credentialsResponse* resp, leet::Room::Room* room, const int Version) {
    using json = nlohmann::json;
    const std::string APIUrl { "/_matrix/client/v3/rooms/" + room->roomID + "/upgrade" };

    json body;

    body["new_version"] = std::to_string(Version);

    const std::string Output { leet::invokeRequest_Post(leet::getAPI(APIUrl), body.dump(), resp->accessToken) };

    json reqOutput;

    try {
        reqOutput = { json::parse(Output) };
    }  catch (const json::parse_error& e) {
        return *room;
    }

    std::string theRoomID{""};
    for (auto& output : reqOutput) {
        leet::errorCode = 0;

        if (output["replacement_room"].is_string()) theRoomID = output["replacement_room"].get<std::string>();
        if (output["errcode"].is_string()) leet::Error = output["errcode"].get<std::string>();
        if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
    }

    if (!theRoomID.compare("")) {
        leet::Room::Room theRoom;
        theRoom.roomID = theRoomID;
        return leet::returnRoom(resp, &theRoom);
    }

    return *room;
}

leet::Room::Room leet::createRoom(leet::User::credentialsResponse* resp, leet::Room::roomConfiguration* conf) {
    using json = nlohmann::json;

    leet::Room::Room theRoom;
    json theJson;

    theJson["creation_content"]["m.federate"] = conf->Federate;
    theJson["name"] = conf->Name;
    theJson["room_alias_name"] = conf->Alias;
    theJson["topic"] = conf->Topic;
    theJson["is_direct"] = conf->directMessage;

    if (conf->Preset == LEET_PRESET_PUBLIC) {
        theJson["preset"] = "public_chat";
    } else if (conf->Preset == LEET_PRESET_PRIVATE) {
        theJson["preset"] = "private_chat";
    } else if (conf->Preset == LEET_PRESET_TRUSTED_PRIVATE) {
        theJson["preset"] = "trusted_private_chat";
    }

    const std::string Output = leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/createRoom"), theJson.dump(), resp->accessToken);
    json reqOutput;

    try {
        reqOutput = { json::parse(Output) };
    } catch (const json::parse_error& e) {
        return theRoom;
    }

    for (auto& output : reqOutput) {
        leet::errorCode = 0;

        if (output["room_id"].is_string()) theRoom.roomID = output["room_id"].get<std::string>();
        if (output["errcode"].is_string()) leet::Error = output["errcode"].get<std::string>();
        if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
    }

    return leet::returnRoom(resp, &theRoom);
}

/* Returns an array of all room IDs */
const std::vector<leet::Room::Room> leet::returnRoomIDs(leet::User::credentialsResponse* resp) {
    using json = nlohmann::json;

    std::vector<leet::Room::Room> vector;

    const std::string Output = leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v3/joined_rooms"), resp->accessToken);
    json returnOutput;

    try {
        returnOutput = json::parse(Output);
    } catch (const json::parse_error& e) {
        return vector;
    }

    auto& rooms = returnOutput["joined_rooms"];

    for (auto it = rooms.begin(); it != rooms.end(); ++it) {
        leet::Room::Room room;
        room.roomID = it.value();
        vector.push_back(room);
    }

    return vector;
}

/* Returns an array of all rooms in a space */
const std::vector<leet::Room::Room> leet::returnRoomsInSpace(leet::User::credentialsResponse* resp, const std::string& spaceID, const int Limit) {
    using json = nlohmann::json;
    std::vector<leet::Room::Room> rooms;
    if (spaceID[0] != '!') {
        return rooms;
    }

    const std::string Output { leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v1/rooms/" + spaceID + "/hierarchy?limit=" + std::to_string(Limit)), resp->accessToken) };
    json returnOutput;

    try {
        returnOutput = json::parse(Output);
    } catch (const json::parse_error& e) {
        return rooms;
    }

    auto& room = returnOutput["rooms"];

    for (auto it = room.begin(); it != room.end(); ++it) {
        leet::Room::Room room;

        if (it.value().contains("room_id")) room.roomID = it.value()["room_id"];
        if (it.value().contains("join_rule")) room.joinRule = it.value()["join_rule"];
        if (it.value().contains("avatar_url")) room.avatarURL = it.value()["avatar_url"];
        if (it.value().contains("canonical_alias")) room.Alias = it.value()["canonical_alias"];
        if (it.value().contains("name")) room.Name = it.value()["name"];
        if (it.value().contains("num_joined_members")) room.memberCount = it.value()["num_joined_members"];
        if (it.value().contains("topic")) room.Topic = it.value()["topic"];
        if (it.value().contains("guest_can_join")) room.guestCanJoin = it.value()["guest_can_join"];
        if (it.value().contains("world_readable")) room.worldReadable = it.value()["world_readable"];
        if (it.value().contains("room_type")) room.roomType = it.value()["room_type"];

        rooms.push_back(room);
    }

    return rooms;
}

/* Returns a vector of all spaces */
const std::vector<leet::Space::Space> leet::returnSpaces(leet::User::credentialsResponse* resp, const int Limit) {
    std::vector<leet::Space::Space> spaces;
    std::vector<leet::Room::Room> rooms = leet::returnRoomIDs(resp);

    for (auto& room : rooms) { // each room id
        leet::Space::Space space;
        std::vector<leet::Room::Room> roomsInSpace = leet::returnRoomsInSpace(resp, room.roomID, Limit);

        for (auto& roomInSpace : roomsInSpace) { // each room in the space
            if (roomInSpace.roomType.compare("m.space")) {
                space.Rooms.push_back(roomInSpace);
                continue;
            }

            space.spaceID = roomInSpace.roomID;
            space.joinRule = roomInSpace.joinRule;
            space.avatarURL = roomInSpace.avatarURL;
            space.Alias = roomInSpace.Alias;
            space.Name = roomInSpace.Name;
            space.memberCount = roomInSpace.memberCount;
            space.Topic = roomInSpace.Topic;
            space.guestCanJoin = roomInSpace.guestCanJoin;
            space.worldReadable = roomInSpace.worldReadable;

            space.Rooms.push_back(roomInSpace);

            break;
        }

        spaces.push_back(space);
    }

    return spaces;
}

void leet::toggleTyping(leet::User::credentialsResponse* resp, const int Timeout, const bool Typing, leet::Room::Room* room) {
    using json = nlohmann::json;

    json list;

    list["timeout"] = Timeout;
    list["typing"] = Typing;

    const std::string Output { leet::invokeRequest_Put(leet::getAPI("/_matrix/client/v3/rooms/" + room->roomID + "/typing/" + resp->userID), list.dump(), resp->accessToken) };

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

void leet::inviteUserToRoom(leet::User::credentialsResponse* resp, leet::Room::Room* room, const std::string& Reason) {
    using json = nlohmann::json;

    json request;

    request["reason"] = Reason;
    request["user_id"] = resp->userID;

    const std::string Output { leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/rooms/" + room->roomID + "/invite"), request.dump(), resp->accessToken) };
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

void leet::joinRoom(leet::User::credentialsResponse* resp, leet::Room::Room* room, const std::string& Reason) {
    using json = nlohmann::json;

    json body;

    if (!Reason.compare("")) {
        body["reason"] = Reason;
    }

    const std::string Output { leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/rooms/" + room->roomID + "/join"), body.dump(), resp->accessToken) };

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

    // You may want to refresh your room list and call /sync after this
}

void leet::leaveRoom(leet::User::credentialsResponse* resp, leet::Room::Room* room, const std::string& Reason) {
    using json = nlohmann::json;

    json body;

    if (!Reason.compare("")) {
        body["reason"] = Reason;
    }

    const std::string Output { leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/rooms/" + room->roomID + "/leave"), body.dump(), resp->accessToken) };

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

void leet::kickUserFromRoom(leet::User::credentialsResponse* resp, leet::Room::Room* room, leet::User::Profile* profile, const std::string& Reason) {
    using json = nlohmann::json;

    json body;

    if (!Reason.compare("")) {
        body["reason"] = Reason;
    }

    body["user_id"] = profile->userID;

    const std::string Output { leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/rooms/" + room->roomID + "/kick"), body.dump(), resp->accessToken) };

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

void leet::banUserFromRoom(leet::User::credentialsResponse* resp, leet::Room::Room* room, leet::User::Profile* profile, const std::string& Reason) {
    using json = nlohmann::json;

    json body;

    if (!Reason.compare("")) {
        body["reason"] = Reason;
    }

    body["user_id"] = profile->userID;

    const std::string Output { leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/rooms/" + room->roomID + "/ban"), body.dump(), resp->accessToken) };

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

void leet::unbanUserFromRoom(leet::User::credentialsResponse* resp, leet::Room::Room* room, leet::User::Profile* profile, const std::string& Reason) {
    using json = nlohmann::json;

    json body;

    if (!Reason.compare("")) {
        body["reason"] = Reason;
    }

    body["user_id"] = profile->userID;

    const std::string Output { leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/rooms/" + room->roomID + "/unban"), body.dump(), resp->accessToken) };

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

const bool getVisibilityOfRoom(leet::User::credentialsResponse* resp, leet::Room::Room* room) {
    using json = nlohmann::json;

    const std::string Output { leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v3/directory/list/room/" + room->roomID), resp->accessToken) };

    json reqOutput;

    try {
        reqOutput = { json::parse(Output) };
    } catch (const json::parse_error& e) {
        return false;
    }

    for (auto& output : reqOutput) {
        leet::errorCode = 0;

        if (output["errcode"].is_string()) {
            leet::errorCode = 1;
            leet::Error = output["errcode"].get<std::string>();
            if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
            if (output["visibility"].is_string()) {
                if (output["visibility"].get<std::string>().compare("private")) {
                    return true;
                } else {
                    return false;
                }
            }
        }
    }

    return false;
}

void setVisibilityOfRoom(leet::User::credentialsResponse* resp, leet::Room::Room* room, const bool Visibility) {
    using json = nlohmann::json;

    json body;

    body["visibility"] = Visibility ? "public" : "private";

    const std::string Output { leet::invokeRequest_Put(leet::getAPI("/_matrix/client/v3/directory/list/room/" + room->roomID), body.dump(), resp->accessToken) };

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

void leet::setReadMarkerPosition(leet::User::credentialsResponse* resp, leet::Room::Room* room,
        leet::Event::Event* fullyReadEvent, leet::Event::Event* readEvent, leet::Event::Event* privateReadEvent) {
    using json = nlohmann::json;

    json body;

    body["m.fully_read"] = fullyReadEvent->eventID;
    body["m.read"] = readEvent->eventID;
    body["m.read.private"] = privateReadEvent->eventID;

    const std::string Output { leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/rooms/" + room->roomID + "/read_markers"), body.dump(), resp->accessToken) };

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
