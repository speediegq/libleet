/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU Affero General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

#ifndef LEET_NO_ENCRYPTION
namespace leetOlm {
    OlmAccount* Account;
    OlmUtility* Utility;
    OlmOutboundGroupSession* megolmSession;
    std::map<std::string, OlmSession*> olmSessions;

    /**
     * @brief  Pickle an OlmAccount*
     * @param  pickleKey The key that should be used to pickle. The client should (must) store this safely. It is considered sensitive data.
     * @return Returns a pickle for an OlmAccount. The client should (must) store this safely. It is considered sensitive data.
     *
     * Returns a pickle for an OlmAccount
     */
    const std::string pickle(const std::string& pickleKey, OlmAccount* acc) {
        std::size_t pickleLength = olm_pickle_account_length(acc);
        char* Pickle = (char* )malloc(pickleLength + 1);
        const char* pKey = pickleKey.c_str();
        std::size_t pKeyLength = pickleKey.length();

        if (olm_pickle_account(acc, pKey, pKeyLength, Pickle, pickleLength) == olm_error()) {
            throw std::runtime_error("olm_pickle_account()");
        }

        const std::string ret = Pickle;
        free(Pickle);

        return ret;
    }

    /**
     * @brief  Unpickle an OlmAccount*
     * @param  pickleKey The key that should be used to unpickle. The client should (must) store this safely. It is considered sensitive data.
     * @param  pickleData The pickle data returned by a pickle() function.
     * @return Returns an OlmAccount*.
     *
     * Returns a pickle for an OlmAccount
     */
    OlmAccount* unpickle(const std::string& pickleKey, const std::string& pickleData, OlmAccount* acc) {
        const char* pKey = pickleKey.c_str();
        std::size_t pKeyLength = pickleKey.length();

        std::size_t pickleLength = olm_pickle_account_length(acc);
        char* Pickle = const_cast<char*>(pickleData.c_str());

        if (olm_unpickle_account(acc, pKey, pKeyLength, Pickle, pickleLength) == olm_error()) {
            throw std::runtime_error("olm_unpickle_account()");
        }

        return acc;
    }

    /**
     * @brief  Pickle an OlmInboundGroupSession*
     * @param  pickleKey The key that should be used to pickle. The client should (must) store this safely. It is considered sensitive data.
     * @return Returns a pickle for an OlmInboundGroupSession. The client should (must) store this safely. It is considered sensitive data.
     *
     * Returns a pickle for an OlmInboundGroupSession
     */
    const std::string pickle(const std::string& pickleKey, OlmInboundGroupSession* session) {
        std::size_t pickleLength = olm_pickle_inbound_group_session_length(session);
        char* Pickle = (char* )malloc(pickleLength + 1);
        const char* pKey = pickleKey.c_str();
        std::size_t pKeyLength = pickleKey.length();

        if (olm_pickle_inbound_group_session(session, pKey, pKeyLength, Pickle, pickleLength) == olm_error()) {
            throw std::runtime_error("olm_pickle_account()");
        }

        const std::string ret = Pickle;
        free(Pickle);

        return ret;
    }

    /**
     * @brief  Unpickle an OlmInboundGroupSession*
     * @param  pickleKey The key that should be used to unpickle. The client should (must) store this safely. It is considered sensitive data.
     * @param  pickleData The pickle data returned by a pickle() function.
     * @return Returns an OlmInboundGroupSession*.
     *
     * Returns a pickle for an OlmInboundGroupSession
     */
    OlmInboundGroupSession* unpickle(const std::string& pickleKey, const std::string& pickleData, OlmInboundGroupSession* session) {
        const char* pKey = pickleKey.c_str();
        std::size_t pKeyLength = pickleKey.length();

        std::size_t pickleLength = olm_pickle_inbound_group_session_length(session);
        char* Pickle = const_cast<char*>(pickleData.c_str());

        if (olm_unpickle_inbound_group_session(session, pKey, pKeyLength, Pickle, pickleLength) == olm_error()) {
            throw std::runtime_error("olm_unpickle_account()");
        }

        return session;
    }

    /**
     * @brief  Pickle an OlmOutboundGroupSession*
     * @param  pickleKey The key that should be used to pickle. The client should (must) store this safely. It is considered sensitive data.
     * @return Returns a pickle for an OlmOutboundGroupSession. The client should (must) store this safely. It is considered sensitive data.
     *
     * Returns a pickle for an OlmOutboundGroupSession
     */
    const std::string pickle(const std::string& pickleKey, OlmOutboundGroupSession* session) {
        std::size_t pickleLength = olm_pickle_outbound_group_session_length(session);
        char* Pickle = (char* )malloc(pickleLength + 1);
        const char* pKey = pickleKey.c_str();
        std::size_t pKeyLength = pickleKey.length();

        if (olm_pickle_outbound_group_session(session, pKey, pKeyLength, Pickle, pickleLength) == olm_error()) {
            throw std::runtime_error("olm_pickle_account()");
        }

        const std::string ret = Pickle;
        free(Pickle);

        return ret;
    }

    /**
     * @brief  Unpickle an OlmOutboundGroupSession*
     * @param  pickleKey The key that should be used to unpickle. The client should (must) store this safely. It is considered sensitive data.
     * @param  pickleData The pickle data returned by a pickle() function.
     * @return Returns an OlmOutboundGroupSession*.
     *
     * Returns a pickle for an OlmOutboundGroupSession
     */
    OlmOutboundGroupSession* unpickle(const std::string& pickleKey, const std::string& pickleData, OlmOutboundGroupSession* session) {
        const char* pKey = pickleKey.c_str();
        std::size_t pKeyLength = pickleKey.length();

        std::size_t pickleLength = olm_pickle_outbound_group_session_length(session);
        char* Pickle = const_cast<char*>(pickleData.c_str());

        if (olm_unpickle_outbound_group_session(session, pKey, pKeyLength, Pickle, pickleLength) == olm_error()) {
            throw std::runtime_error("olm_unpickle_account()");
        }

        return session;
    }

    /**
     * @brief  Pickle an OlmSession*
     * @param  pickleKey The key that should be used to pickle. The client should (must) store this safely. It is considered sensitive data.
     * @return Returns a pickle for an OlmSession. The client should (must) store this safely. It is considered sensitive data.
     *
     * Returns a pickle for an OlmSession
     */
    const std::string pickle(const std::string& pickleKey, OlmSession* session) {
        std::size_t pickleLength = olm_pickle_session_length(session);
        char* Pickle = (char* )malloc(pickleLength + 1);
        const char* pKey = pickleKey.c_str();
        std::size_t pKeyLength = pickleKey.length();

        if (olm_pickle_session(session, pKey, pKeyLength, Pickle, pickleLength) == olm_error()) {
            throw std::runtime_error("olm_pickle_account()");
        }

        const std::string ret = Pickle;
        free(Pickle);

        return ret;
    }

    /**
     * @brief  Unpickle an OlmSession*
     * @param  pickleKey The key that should be used to unpickle. The client should (must) store this safely. It is considered sensitive data.
     * @param  pickleData The pickle data returned by a pickle() function.
     * @return Returns an OlmSession*.
     *
     * Returns a pickle for an OlmSession
     */
    OlmSession* unpickle(const std::string& pickleKey, const std::string& pickleData, OlmSession* session) {
        const char* pKey = pickleKey.c_str();
        std::size_t pKeyLength = pickleKey.length();

        std::size_t pickleLength = olm_pickle_session_length(session);
        char* Pickle = const_cast<char*>(pickleData.c_str());

        if (olm_unpickle_session(session, pKey, pKeyLength, Pickle, pickleLength) == olm_error()) {
            throw std::runtime_error("olm_unpickle_account()");
        }

        return session;
    }
}

void leet::olmAccount::createAccount() {
    if (!accountMemoryAllocated) {
        accountMemory = malloc(olm_account_size());
        leetOlm::Account = olm_account(accountMemory);
        accountMemoryAllocated = true;
    } else {
        throw std::runtime_error{ "createAccount(): Cannot allocate memory because it is already allocated." };
    }

    leetCrypto::randomBytes randomBytes(olm_create_account_random_length(leetOlm::Account));

    if (olm_create_account(leetOlm::Account, randomBytes.data(), randomBytes.length()) == olm_error()) {
        throw std::runtime_error("olm_create_account()");
    }
}

void leet::olmAccount::loadAccount(const std::string& pickleKey, const std::string& pickleData) {
    if (!accountMemoryAllocated) {
        accountMemory = malloc(olm_account_size());
        leetOlm::Account = leetOlm::unpickle(pickleKey, pickleData, leetOlm::Account);
        accountMemoryAllocated = true;
    } else {
        throw std::runtime_error{ "loadAccount(): Cannot allocate memory because it is already allocated." };
    }
}

void leet::olmAccount::createMegolmSession() {
    if (!megolmSessionMemoryAllocated) {
        megolmSessionMemory = malloc(olm_outbound_group_session_size());
        leetOlm::megolmSession = olm_outbound_group_session(megolmSessionMemory);
        megolmSessionMemoryAllocated = true;
    } else {
        throw std::runtime_error{ "createMegolmSession(): Cannot allocate memory because it is already allocated. (1)" };
    }
    leetCrypto::randomBytes randomBytesSession(olm_init_outbound_group_session_random_length(leetOlm::megolmSession));

    if (olm_init_outbound_group_session(leetOlm::megolmSession, static_cast<uint8_t* >(randomBytesSession.data()), randomBytesSession.length()) == olm_error()) {
        free(megolmSessionMemory);
        megolmSessionMemoryAllocated = false;
        throw std::runtime_error("olm_init_outbound_group_session()");
    }

    if (!megolmSessionIDMemoryAllocated) {
        megolmSessionIDLength = olm_outbound_group_session_id_length(leetOlm::megolmSession);
        megolmSessionID = (char* )malloc(megolmSessionIDLength + 1);
        megolmSessionIDMemoryAllocated = true;
    } else {
        throw std::runtime_error{ "createMegolmSession(): Cannot allocate memory because it is already allocated. (2)" };
    }

    std::size_t tSize = olm_outbound_group_session_id(leetOlm::megolmSession, (uint8_t* )megolmSessionID, megolmSessionIDLength);

    if (tSize == olm_error()) {
        throw std::runtime_error("olm_outbound_group_session_id()");
    }

    megolmSessionID[tSize] = '\0';
}

void leet::olmAccount::loadMegolmSession(const std::string& pickleKey, const std::string& pickleData) {
    if (!megolmSessionMemoryAllocated) {
        megolmSessionMemory = malloc(olm_outbound_group_session_size());
        leetOlm::megolmSession = leetOlm::unpickle(pickleKey, pickleData, leetOlm::megolmSession);
        megolmSessionMemoryAllocated = true;
    } else {
        throw std::runtime_error{ "createMegolmSession(): Cannot allocate memory because it is already allocated. (1)" };
    }

    if (!megolmSessionIDMemoryAllocated) {
        megolmSessionIDLength = olm_outbound_group_session_id_length(leetOlm::megolmSession);
        megolmSessionID = (char* )malloc(megolmSessionIDLength + 1);
        megolmSessionIDMemoryAllocated = true;
    } else {
        throw std::runtime_error{ "createMegolmSession(): Cannot allocate memory because it is already allocated. (2)" };
    }

    std::size_t tSize = olm_outbound_group_session_id(leetOlm::megolmSession, (uint8_t* )megolmSessionID, megolmSessionIDLength);

    if (tSize == olm_error()) {
        throw std::runtime_error("olm_outbound_group_session_id()");
    }

    megolmSessionID[tSize] = '\0';
}

void leet::olmAccount::createIdentity() {
    if (!identityMemoryAllocated) {
        identityLength = olm_account_identity_keys_length(leetOlm::Account);
        Identity = (char* )malloc(identityLength + 1);
        identityMemoryAllocated = true;
    } else {
        throw std::runtime_error{ "createIdentity(): Cannot allocate memory because it is already allocated." };
    }

    std::size_t tSize = olm_account_identity_keys(leetOlm::Account, Identity, identityLength);

    if (tSize == olm_error()) {
        free(Identity); // Identity was allocated
        identityMemoryAllocated = false;
        throw std::runtime_error("olm_account_identity_keys()");
    }

    Identity[tSize] = '\0';
    nlohmann::json identityJson;

    try {
        identityJson = { nlohmann::json::parse(Identity) };
    } catch (const nlohmann::json::parse_error& e) {
        free(Identity);
        identityMemoryAllocated = false;
        return;
    }

    for (auto &output : identityJson) {
        leet::errorCode = 0;

        if (output["curve25519"].is_string())
            curve25519 = output["curve25519"].get<std::string>();
        if (output["ed25519"].is_string())
            ed25519 = output["ed25519"].get<std::string>();
    }

    // Now we don't this anymore
    free(Identity);
    identityMemoryAllocated = false;
}

void leet::olmAccount::upload(leet::User::credentialsResponse* resp) {
    if (!curve25519.compare("")) {
        throw std::runtime_error{ "upload(): Identity not allocated." };
    }

    nlohmann::json Body = {
        { "algorithms", {"m.olm.v1.curve25519-aes-sha2", "m.megolm.v1.aes-sha2" }},
        { "device_id", resp->deviceID },
        { "user_id", resp->userID },
        { "keys", {
            { std::string("ed25519:") + resp->deviceID, ed25519 },
            { std::string("curve25519:") + resp->deviceID, curve25519 }
        } }
    };

    std::string Keys = Body.dump();

    if (!signatureMemoryAllocated) {
        signatureLength = olm_account_signature_length(leetOlm::Account);
        Signature = (char* )malloc(signatureLength + 1);
        signatureMemoryAllocated = true;
    } else {
        throw std::runtime_error{ "upload(): Cannot allocate memory because it is already allocated." };
    }

    // Sign using the nlohmann::json we constructed, which will provide us with a signature
    if (olm_account_sign(leetOlm::Account, Keys.data(), Keys.length(), Signature, signatureLength) == olm_error()) {
        free(Signature); // Signature was allocated
        signatureMemoryAllocated = false;
        throw std::runtime_error("olm_account_sign()");
    }

    Signature[signatureLength] = '\0';

    Body["signatures"] = {
        { resp->userID, {
            { std::string("ed25519:") + resp->deviceID, Signature }
        } }
    };

    nlohmann::json keysJson = {
        { "device_keys", Body }
    };

    free(Signature);
    signatureMemoryAllocated = false;

    // Upload our device keys
    const std::string Output {
        leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/keys/upload"), keysJson.dump(), resp->accessToken)
    };

    nlohmann::json uploadedKeys;

    try {
        uploadedKeys = nlohmann::json::parse(Output);
    } catch (const nlohmann::json::parse_error& e) {
        return;
    }

    keysRemaining = uploadedKeys["one_time_key_counts"]["signed_curve25519"];

    // Max / 2 is a good amount of one time keys to keep available
    const int keysToKeep = olm_account_max_number_of_one_time_keys(leetOlm::Account) / 2;

    // We might need to generate new keys
    if (keysRemaining < keysToKeep) {
        int keysNeeded = keysToKeep - keysRemaining;
        std::size_t otkLength = olm_account_one_time_keys_length(leetOlm::Account);
        char* Otk = (char* )malloc(otkLength + 1);

        std::size_t tSize = olm_account_one_time_keys(leetOlm::Account, Otk, otkLength);

        if (tSize == olm_error()) {
            free(Otk); // Otk was allocated
            throw std::runtime_error("olm_account_one_time_keys() (1)");
        }

        Otk[tSize] = '\0';

        nlohmann::json Otks;

        try {
            Otks = nlohmann::json::parse(Otk);
        } catch (const nlohmann::json::parse_error& e) {
            return;
        }

        free(Otk);

        int keysAvailable = Otks["ed25519"].size();

        // We do need to generate new keys
        if (keysNeeded > keysAvailable) {
            int keysToGenerate = keysNeeded - keysAvailable;
            std::size_t otkRandomLength = olm_account_generate_one_time_keys_random_length(leetOlm::Account, keysToGenerate);
            leetCrypto::randomBytes randomBytes(otkRandomLength);

            if (olm_account_generate_one_time_keys(leetOlm::Account, keysToGenerate, randomBytes.data(), randomBytes.length()) == olm_error()) {
                throw std::runtime_error("olm_account_generate_one_time_keys()");
            }

            // Now let's get all the keys we have
            otkLength = olm_account_one_time_keys_length(leetOlm::Account);
            Otk = (char* )malloc(otkLength + 1);

            tSize = olm_account_one_time_keys(leetOlm::Account, Otk, otkLength);
            if (tSize == olm_error()) {
                throw std::runtime_error("olm_account_one_time_keys() (2)");
            }

            Otk[tSize] = '\0';
            Otks = nlohmann::json::parse(Otk);

            free(Otk);
        }

        nlohmann::json signedOtks;

        // Now let's sign all of them
        for (auto& output : Otks["curve25519"].items()) {
            nlohmann::json Keys = {
                { "key", output.value() }
            };

            if (!signatureMemoryAllocated) {
                signatureLength = olm_account_signature_length(leetOlm::Account);
                Signature = (char* )malloc(signatureLength + 1);
                signatureMemoryAllocated = true;
            } else {
                throw std::runtime_error{ "upload(): Cannot allocate memory because it is already allocated." };
            }

            std::string theKeys = Keys.dump();

            if (olm_account_sign(leetOlm::Account, theKeys.data(), theKeys.length(), Signature, signatureLength) == olm_error()) {
                free(Signature);
                signatureMemoryAllocated = false;
                throw std::runtime_error("olm_account_sign()");
            }

            Signature[signatureLength] = '\0';

            Keys["signatures"] = {
                { resp->userID, {
                    { std::string("ed25519:") + resp->deviceID, Signature }
                } }
            };

            // Increase the number of keys
            signedOtks[std::string("signed_curve25519:") + output.key()] = Keys;
            free(Signature);
            signatureMemoryAllocated = false;
        }

        // Upload it all
        nlohmann::json Body = { { "one_time_keys", signedOtks } };
        const std::string outputReq {
            leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/keys/upload"), Body.dump(), resp->accessToken)
        };

        olm_account_mark_keys_as_published(leetOlm::Account);
    }
}

void leet::olmAccount::createSession(leet::User::credentialsResponse* resp, leet::Room::Room* room, const std::vector<leet::User::Profile>& users) {
    if (!megolmSessionMemoryAllocated) {
        throw std::runtime_error{ "createSession(): Megolm session not allocated." };
    }

    if (!utilityMemoryAllocated) {
        utilityMemory = malloc(olm_utility_size());
        leetOlm::Utility = olm_utility(utilityMemory);
        utilityMemoryAllocated = true;
    } else {
        throw std::runtime_error{ "createSession(): Cannot allocate memory because it is already allocated. (0)" };
    }

    // Loop through all devices that this user has
    nlohmann::json Body;
    nlohmann::json deviceKeys;

    for (auto& user : users) {
        for (auto& output : user.Devices) { // each user and each of his devices
            // deviceKeys should look like the nlohmann::json returned from keys/query
            if (output.olm && output.megolm) {
                deviceKeys["device_keys"][output.userID][output.deviceID]["algorithms"] = nlohmann::json::array({"m.olm.v1.curve25519-aes-sha2", "m.megolm.v1.aes-sha2"});
            } else if (output.olm) {
                deviceKeys["device_keys"][output.userID][output.deviceID]["algorithms"] = nlohmann::json::array({"m.olm.v1.curve25519-aes-sha2"});
            } else if (output.megolm) {
                deviceKeys["device_keys"][output.userID][output.deviceID]["algorithms"] = nlohmann::json::array({"m.megolm.v1.aes-sha2"});
            }

            deviceKeys["device_keys"][output.userID][output.deviceID]["device_id"] = output.deviceID;
            deviceKeys["device_keys"][output.userID][output.deviceID]["keys"]["curve25519:" + output.deviceID] = output.curve25519Key;
            deviceKeys["device_keys"][output.userID][output.deviceID]["keys"]["ed25519:" + output.deviceID] = output.ed25519Key;
            deviceKeys["device_keys"][output.userID][output.deviceID]["user_id"] = output.userID;

            Body["one_time_keys"][output.userID][output.deviceID] = "signed_curve25519";
        }
    }

    const std::string Output {
        leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/keys/claim"), Body.dump(), resp->accessToken)
    };

    nlohmann::json claimedKeys;

    try {
        claimedKeys = nlohmann::json::parse(Output);
    } catch (const nlohmann::json::parse_error& e) {
        return;
    }

    std::size_t tSize;

    nlohmann::json eventToSend;
    for (auto& user : users) {
        for (auto& output : user.Devices) {
            nlohmann::json deviceKey = deviceKeys["device_keys"][output.userID][output.deviceID];
            const std::string keysToSign = deviceKey.dump();

            char* signatureCopy = (char* )malloc(output.ed25519Signature.length() + 1);
            output.ed25519Signature.copy(signatureCopy, output.ed25519Signature.length());

            if (olm_ed25519_verify(leetOlm::Utility, output.ed25519Key.data(), output.ed25519Key.length(),
                keysToSign.data(), keysToSign.length(), signatureCopy, output.ed25519Signature.length()) == olm_error()) {

                free(signatureCopy);
                continue; // Invalid device
            }

            free(signatureCopy);

            // fetch megolm session key
            megolmSessionKeyLength = olm_outbound_group_session_key_length(leetOlm::megolmSession);
            megolmSessionKey = (char* )malloc(megolmSessionKeyLength + 1);

            tSize = olm_outbound_group_session_key(leetOlm::megolmSession, (uint8_t* )megolmSessionKey, megolmSessionKeyLength);

            if (tSize == olm_error()) {
                throw std::runtime_error("olm_outbound_group_session_key()");
            }

            megolmSessionKey[tSize] = '\0';

            std::string Otk{""};

            nlohmann::json Signed = claimedKeys["one_time_keys"][output.userID][output.deviceID];
            for (auto& signedOtk : Signed.items()) {
                if (signedOtk.value().contains("key")) {
                    Otk = signedOtk.value()["key"];
                    break;
                }
            }

            if (!Otk.compare("")) {
                continue;
            }

            // Create an olm session
            OlmSession* Session;
            void* sessionMemory;

            if (!olmSessionMemoryAllocated) {
                sessionMemory = malloc(olm_session_size());
                Session = olm_session(sessionMemory);
                olmSessionMemoryAllocated = true;
            } else {
                throw std::runtime_error{ "createSession(): Cannot allocate memory because it is already allocated. (4)" };
            }

            // Store the session
            leetOlm::olmSessions[output.userID+output.curve25519Key+output.deviceID] = Session;

            leetCrypto::randomBytes randomBytes(olm_create_outbound_session_random_length(Session));

            if (olm_create_outbound_session(Session, leetOlm::Account, output.curve25519Key.data(), output.curve25519Key.length(), Otk.data(), Otk.length(), randomBytes.data(), randomBytes.length()) == olm_error()) {
                throw std::runtime_error("olm_create_outbound_session()");
            }

            nlohmann::json roomKeyMessage;

            roomKeyMessage["algorithm"] = "m.megolm.v1.aes-sha2";
            roomKeyMessage["room_id"] = room->roomID;
            roomKeyMessage["session_id"] = megolmSessionID;
            roomKeyMessage["session_key"] = megolmSessionKey;

            nlohmann::json roomKey;

            roomKey["type"] = "m.room_key";
            roomKey["content"] = roomKeyMessage;
            roomKey["sender"] = resp->userID;
            roomKey["sender_device"] = resp->deviceID;
            roomKey["recipient"] = output.userID;
            roomKey["recipient_keys"]["ed25519"] = output.ed25519Key;
            roomKey["keys"]["ed25519"] = ed25519;

            const std::string roomKeyMsg = roomKey.dump();

            leetCrypto::randomBytes randomBytesEncrypt(olm_encrypt_random_length(Session));
            std::size_t cipherTextLength = olm_encrypt_message_length(Session, roomKeyMsg.length());
            char* cipherText = (char* )malloc(cipherTextLength + 1);

            tSize = olm_encrypt(Session, roomKeyMsg.data(), roomKeyMsg.length(), randomBytesEncrypt.data(), randomBytesEncrypt.length(), cipherText, cipherTextLength);

            if (tSize == olm_error()) {
                free(cipherText);
                throw std::runtime_error("olm_encrypt()");
            }

            cipherText[tSize] = '\0';

            nlohmann::json encryptedMessage;

            encryptedMessage["algorithm"] = "m.olm.v1.curve25519-aes-sha2";
            encryptedMessage["sender_key"] = curve25519;
            encryptedMessage["ciphertext"][output.curve25519Key]["type"] = olm_encrypt_message_type(Session);
            encryptedMessage["ciphertext"][output.curve25519Key]["body"] = cipherText;

            eventToSend["messages"][output.userID][output.deviceID] = encryptedMessage;

            free(cipherText);
            free(megolmSessionKey);
            free(sessionMemory);

            olmSessionMemoryAllocated = false;
        }
    }

    const std::string putOutput {
        leet::invokeRequest_Put(leet::getAPI("/_matrix/client/v3/sendToDevice/m.room.encrypted/" + std::to_string(transID)), eventToSend.dump(), resp->accessToken)
    };

    free(utilityMemory);
    utilityMemoryAllocated = false;
}

const std::string leet::olmAccount::encryptMessage(leet::User::credentialsResponse* resp, const std::string& message) {
    std::size_t cipherTextLength = olm_group_encrypt_message_length(leetOlm::megolmSession, message.length());
    char* cipherText = (char* )malloc(cipherTextLength + 1);
    std::size_t tSize = olm_group_encrypt(leetOlm::megolmSession, (uint8_t* )message.data(), message.length(), (uint8_t* )cipherText, cipherTextLength);

    if (tSize == olm_error()) {
        throw std::runtime_error("olm_group_encrypt()");
    }

    cipherText[tSize] = '\0';

    nlohmann::json retMessage = {
        { "algorithm", "m.megolm.v1.aes-sha2" },
        { "sender_key", curve25519 },
        { "ciphertext", cipherText },
        { "session_id", megolmSessionID },
        { "device_id", resp->deviceID }
    };

    free(cipherText);

    return retMessage.dump();
}

void leet::olmAccount::clear() {
    if (accountMemoryAllocated) {
        olm_clear_account(leetOlm::Account);
    } else {
        throw std::runtime_error{ "clear(): Cannot clear account because memory is not allocated for the account." };
    }
}

void leet::olmAccount::destroy() {
    if (megolmSessionIDMemoryAllocated) {
        free(megolmSessionID);
        megolmSessionIDMemoryAllocated = false;
    }
    if (megolmSessionMemoryAllocated) {
        free(megolmSessionMemory);
        megolmSessionMemoryAllocated = false;
    }
    if (accountMemoryAllocated) {
        free(accountMemory);
        accountMemoryAllocated = false;
    }
}

void leet::Encryption::destroy() {
    if (!Cleaned) {
        account.clear();
        account.destroy();
        Cleaned = true;
    } else {
        throw std::runtime_error{ "olmAccount: Already destroyed." };
    }
}

leet::Encryption leet::initEncryption() {
    leet::Encryption enc;
    enc.account.createAccount();
    enc.hasCreatedAccount = true;
    return enc;
}

leet::Encryption leet::initEncryptionFromPickle(const std::string& pickleKey, const std::string& pickleData) {
    leet::Encryption enc;

    enc.account.loadAccount(pickleKey, pickleData);
    enc.hasCreatedAccount = true;

    return enc;
}

leet::Encryption leet::uploadKeys(leet::User::credentialsResponse* resp, leet::Encryption* enc) {
    leet::Encryption ret = *enc;

    if (!ret.hasCreatedAccount) {
        throw std::runtime_error{ "olmAccount: Account has not been created." };
    }
    if (ret.Cleaned) {
        throw std::runtime_error{ "olmAccount: Memory has already been cleared." };
    }

    ret.account.createIdentity();
    ret.account.upload(resp);
    ret.hasUploadedKeys = true;

    return ret;
}

leet::Encryption leet::createSessionInRoom(leet::User::credentialsResponse* resp, leet::Encryption* enc, leet::Room::Room* room) {
    leet::Encryption ret = *enc;

    if (!ret.hasCreatedAccount) {
        throw std::runtime_error{ "olmAccount: Account has not been created." };
    }
    if (ret.Cleaned) {
        throw std::runtime_error{ "olmAccount: Memory has already been cleared." };
    }
    if (!ret.hasUploadedKeys) {
        throw std::runtime_error{ "olmAccount: Keys have not been uploaded. (!ret.hasUploadedKeys)" };
    }

    // TODO: Store megolm sessions
    ret.account.createMegolmSession();

    std::vector<leet::User::Profile> users = leet::returnUsersInRoom(resp, room);

    ret.account.createSession(resp, room, users);

    return ret;
}

#endif // !LEET_NO_ENCRYPTION
