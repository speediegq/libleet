/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU Affero General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

leet::Sync::Sync leet::returnSync(leet::User::credentialsResponse* resp, leet::Sync::syncConfiguration* conf) {
    leet::Sync::Sync sync;
    std::string presenceString{"offline"};

    switch(conf->Presence) {
        case LEET_PRESENCE_OFFLINE:
            presenceString = "offline";
            break;
        case LEET_PRESENCE_ONLINE:
            presenceString = "online";
            break;
        case LEET_PRESENCE_UNAVAILABLE:
            presenceString = "unavailable";
            break;
        default:
            break;
    }

    const std::string Output = leet::invokeRequest_Get(
            leet::getAPI("/_matrix/client/v3/sync?presence=" + presenceString + "&timeout=" + std::to_string(conf->Timeout) +
                        (conf->Since.compare("") ? "&since=" + conf->Since : "") + "&full_state=" + (conf->fullState ? "true" : "false") +
                        (conf->Filter.filterID.compare("") ? "&filter=" + conf->Filter.filterID : "")),
                        resp->accessToken
    );

    sync.theRequest = Output;

    nlohmann::json theOutput;

    try {
        theOutput = { nlohmann::json::parse(Output) };
    } catch (const nlohmann::json::parse_error& e) {
        return sync;
    }

    for (auto& it : theOutput) {
        leet::errorCode = 0;
        
        if (it["next_batch"].is_string()) {
            sync.nextBatch = it["next_batch"].get<std::string>();
        }

        // ToDevice
        if (it["to_device"]["events"].is_array())
        for (auto& itEvent : it["to_device"]["events"]) {
            leet::errorCode = 0;
            leet::Sync::megolmSession megolmSession;

            if (itEvent["content"]["sender_key"].is_string()) {
                megolmSession.senderKey = itEvent["content"]["sender_key"];
            }
            
            if (itEvent["content"]["algorithm"].is_string()) {
                megolmSession.Algorithm = itEvent["content"]["algorithm"];
            }

            if (megolmSession.senderKey.compare("")) {
                if (!itEvent["content"]["ciphertext"][megolmSession.senderKey]["body"].is_null()) {
                    megolmSession.cipherText = itEvent["content"]["ciphertext"][megolmSession.senderKey]["body"];
                }
                if (!itEvent["content"]["ciphertext"][megolmSession.senderKey]["type"].is_null()) {
                    megolmSession.cipherType = itEvent["content"]["ciphertext"][megolmSession.senderKey]["type"];
                }
            }

            if (itEvent["sender"].is_string()) {
                megolmSession.Sender = itEvent["sender"];
            }
            
            if (itEvent["type"].is_string()) {
                megolmSession.Type = itEvent["type"];
            }

            sync.megolmSessions.push_back(megolmSession);
        }

        // Room related events
        // TODO: Broken, fix it
        if (it["rooms"].is_object())
        for (auto& itEvent : it["rooms"]) {
            if (itEvent["invite"].is_object())
            for (auto& inviteIt : itEvent["invite"]) {
                leet::Sync::roomEvent::inviteEvent theInviteEvent;

                if (!inviteIt["invite_state"]["events"].is_array()) {
                    continue;
                }

                for (auto& eventIt : inviteIt["invite_state"]["events"]) {
                    if (!eventIt["type"].is_string()) {
                        continue;
                    }

                    const std::string theType{eventIt["type"].get<std::string>()};

                    if (!theType.compare("m.room.encryption")) {
                        theInviteEvent.Encrypted = true;
                    }

                    if (!theType.compare("m.room.create")) {
                        if (eventIt["content"]["creator"].is_string())
                            theInviteEvent.Creator = eventIt["content"]["creator"].get<std::string>();
                        if (eventIt["content"]["room_version"].is_number_integer())
                            theInviteEvent.roomVersion = eventIt["content"]["room_version"].get<int>();
                    }
                    if (!theType.compare("m.room.member")) {
                        // check if the event is a member event for us, if it isn't we will treat it as the creator of the room
                        if (eventIt["state_key"].is_string()) if (eventIt["state_key"].get<std::string>().compare(resp->userID)) {
                            if (eventIt["content"]["displayname"].is_string()) {
                                theInviteEvent.displayName = eventIt["content"]["displayname"].get<std::string>();
                            }
                            if (eventIt["content"]["avatar_url"].is_string()) {
                                theInviteEvent.avatarURL = eventIt["content"]["avatar_url"].get<std::string>();
                            }
                            continue;
                        }

                        if (eventIt["sender"].is_string()) {
                            theInviteEvent.userID = eventIt["sender"].get<std::string>();
                        }

                        if (eventIt["content"]["allow"]["room_id"].is_string()) {
                            theInviteEvent.roomID = eventIt["content"]["allow"]["room_id"].get<std::string>();
                        }
                    }

                    if (!theType.compare("m.room.name")) {
                        if (eventIt["content"]["name"].is_string()) {
                            theInviteEvent.roomName = eventIt["content"]["name"].get<std::string>();
                        }
                    }

                    if (!theType.compare("m.room.topic")) {
                        if (eventIt["content"]["topic"].is_string()) {
                            theInviteEvent.roomTopic = eventIt["content"]["topic"].get<std::string>();
                        }
                    }

                    if (!theType.compare("m.room.join_rules")) {
                        if (eventIt["type"].is_string()) {
                            theInviteEvent.joinRule = eventIt["type"].get<std::string>();
                        }
                    }
                }

                sync.roomEvents.Invites.push_back(theInviteEvent);
            }
        }
    }

    return sync;
}