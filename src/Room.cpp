/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

/* Converts an alias to a proper room ID */
std::string leet::findRoomID(std::string Alias) {
    using json = nlohmann::json;

    leet::errorCode = 0;

    if (Alias[0] == '!') { // It's a proper room ID already
        return Alias;
    }

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

/* Returns an array of all rooms */
std::vector<leet::Room::Room> leet::returnRooms(leet::User::CredentialsResponse *resp, const int Limit) {
    using json = nlohmann::json;

    std::vector<leet::Room::Room> vector;
    std::vector<leet::Room::Room> vector_with_val;

    const std::string Output = leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v3/joined_rooms"), resp->AccessToken);
    json returnOutput = json::parse(Output);

    auto &rooms = returnOutput["joined_rooms"];

    for (auto currKey = rooms.begin(); currKey != rooms.end(); ++currKey) {
        leet::Room::Room room;
        room.RoomID = currKey.value();
        vector.push_back(room);
    }

    for (auto& currKey : vector) {
        json returnOutput = json::parse(leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v1/rooms/" + currKey.RoomID + "/hierarchy?limit=" + std::to_string(Limit)), resp->AccessToken));

        auto &room = returnOutput["rooms"];

        for (auto i = room.begin(); i != room.end(); ++i) {
            leet::Room::Room room;

            if (i.value().contains("room_id")) room.RoomID = i.value()["room_id"];
            if (i.value().contains("join_rule")) room.joinRule = i.value()["join_rule"];
            if (i.value().contains("avatar_url")) room.AvatarURL = i.value()["avatar_url"];
            if (i.value().contains("canonical_alias")) room.Alias = i.value()["canonical_alias"];
            if (i.value().contains("name")) room.Name = i.value()["name"];
            if (i.value().contains("num_joined_members")) room.memberCount = i.value()["num_joined_members"];
            if (i.value().contains("topic")) room.Topic = i.value()["topic"];
            if (i.value().contains("guest_can_join")) room.guestCanJoin = i.value()["guest_can_join"];
            if (i.value().contains("world_readable")) room.worldReadable = i.value()["world_readable"];
            if (i.value().contains("room_type")) room.roomType = i.value()["room_type"];

            vector_with_val.push_back(room);
        }
    }

    return vector_with_val;
}

/* Returns an array of all room IDs */
std::vector<leet::Room::Room> leet::returnRoomIDs(leet::User::CredentialsResponse *resp) {
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

std::vector<leet::Room::Room> leet::returnRoomsInSpace(leet::User::CredentialsResponse *resp, const std::string SpaceID, const int Limit) {
    using json = nlohmann::json;
    std::vector<leet::Room::Room> rooms;
    if (SpaceID[0] != '!') {
        return rooms;
    }

    const std::string Output { leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v1/rooms/" + SpaceID + "/hierarchy?limit=" + std::to_string(Limit)), resp->AccessToken) };
    json returnOutput = json::parse(Output);

    auto &room = returnOutput["rooms"];

    for (auto currKey = room.begin(); currKey != room.end(); ++currKey) {
        leet::Room::Room room;

        if (currKey.value().contains("room_id")) room.RoomID = currKey.value()["room_id"];
        if (currKey.value().contains("join_rule")) room.joinRule = currKey.value()["join_rule"];
        if (currKey.value().contains("avatar_url")) room.AvatarURL = currKey.value()["avatar_url"];
        if (currKey.value().contains("canonical_alias")) room.Alias = currKey.value()["canonical_alias"];
        if (currKey.value().contains("name")) room.Name = currKey.value()["name"];
        if (currKey.value().contains("num_joined_members")) room.memberCount = currKey.value()["num_joined_members"];
        if (currKey.value().contains("topic")) room.Topic = currKey.value()["topic"];
        if (currKey.value().contains("guest_can_join")) room.guestCanJoin = currKey.value()["guest_can_join"];
        if (currKey.value().contains("world_readable")) room.worldReadable = currKey.value()["world_readable"];
        if (currKey.value().contains("room_type")) room.roomType = currKey.value()["room_type"];

        rooms.push_back(room);
    }

    return rooms;
}

/* Returns a vector of all spaces */
std::vector<leet::Space::Space> leet::returnSpaces(leet::User::CredentialsResponse *resp) {
    std::vector<leet::Space::Space> spaces;
    std::vector<leet::Room::Room> rooms = leet::returnRoomIDs(resp);

    for (auto &room : rooms) { // each room id
        leet::Space::Space space;
        std::vector<leet::Room::Room> roomsInSpace = leet::returnRoomsInSpace(resp, room.RoomID, 20);

        for (auto &roomInSpace : roomsInSpace) { // each room in the space
            if (roomInSpace.roomType.compare("m.space")) {
                space.Rooms.push_back(roomInSpace);
                continue;
            }

            space.SpaceID = roomInSpace.RoomID;
            space.joinRule = roomInSpace.joinRule;
            space.AvatarURL = roomInSpace.AvatarURL;
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
