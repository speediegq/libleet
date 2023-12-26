/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU Lesser General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <future>
#include <vector>
#include <regex>
#include <filesystem>
#include <chrono>
#include <string>
#include <string_view>
#include <ctime>
#include <exception>
#include <map>
#include <nlohmann/json.hpp>

#include "../include/libleet.hpp"
#include "../include/net/Request.hpp"
#ifndef LEET_NO_ENCRYPTION
#include "../include/crypto/olm.hpp"

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

std::vector<std::string> leet::returnSupportedLoginTypes() {
    std::vector<std::string> vector;
    const std::string APIUrl { "/_matrix/client/v3/login" };

    std::string Output { leet::invokeRequest_Get(leet::getAPI(APIUrl)) };

    nlohmann::json reqOutput;

    try {
        reqOutput = nlohmann::json::parse(Output);
    } catch (const nlohmann::json::parse_error& e) {
        return vector;
    }

    auto& messages = reqOutput["flows"];

    for (auto it = messages.begin(); it != messages.end(); ++it) {
        std::string theString{};
        if (it.value().contains("/type"_json_pointer)) theString = it.value()["type"];
        vector.push_back(theString);
    }

    return vector;
}

void leet::invalidateAccessToken(const std::string& Token) {
    leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/logout"), Token);
}

leet::User::credentialsResponse leet::refreshAccessToken(leet::User::credentialsResponse* resp) {
    leet::User::credentialsResponse newResponse = *resp;

    if (!newResponse.refreshToken.compare("")) {
        return newResponse;
    }

    nlohmann::json body;

    body["refresh_token"] = newResponse.refreshToken;

    nlohmann::json refreshOutput;

    try {
        refreshOutput = { nlohmann::json::parse(leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/refresh"), body.dump())) };
    } catch (const nlohmann::json::parse_error& e) {
        return newResponse;
    }

    for (auto& output : refreshOutput) {
        leet::errorCode = 0;

        if (output["access_token"].is_string()) newResponse.accessToken = output["access_token"].get<std::string>();
        if (output["refresh_token"].is_string()) newResponse.refreshToken = output["refresh_token"].get<std::string>();
        if (output["expires_in_ms"].is_number_integer()) newResponse.Expiration = output["expires_in_ms"].get<int>();
        if (output["errcode"].is_string()) {
            leet::errorCode = 1;
            leet::Error = output["errcode"].get<std::string>();
            if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
        }
    }

    return newResponse;
}

bool leet::checkRegistrationTokenValidity(const std::string& Token) {
    nlohmann::json body;

    try {
        body = { nlohmann::json::parse(leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v1/register/m.login.registration_token/validity?token=" + Token))) };
    } catch (const nlohmann::json::parse_error& e) {
        return false;
    }

    for (auto& output : body) {
        leet::errorCode = 0;

        if (output["valid"].is_boolean()) {
            bool theBool = output["valid"].get<bool>();
            return theBool;
        }

        if (output["errcode"].is_string()) {
            leet::errorCode = 1;
            leet::Error = output["errcode"].get<std::string>();
            if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
        }
    }

    return false;
}

leet::User::credentialsResponse leet::registerAccount(leet::User::Credentials* cred) {
    leet::User::credentialsResponse resp;

    std::string theUsername = cred->Username;

    if (cred->Username[0] == '@') {
        theUsername = leet::returnUserName(cred->Username);

        if (theUsername[0] == '@' || !theUsername.compare("")) {
            return resp;
            leet::errorCode = 1;
        }
    }

    nlohmann::json body;

    if (cred->deviceID.compare("")) {
        body["device_id"] = cred->deviceID;
    }

    body["inhibit_login"] = false;
    body["initial_device_display_name"] = cred->displayName;
    body["username"] = theUsername;
    body["password"] = cred->Password;
    body["refresh_token"] = cred->refreshToken;

    nlohmann::json registerOutput;

    try {
        registerOutput = { nlohmann::json::parse(leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/register"), body.dump())) };
    } catch (const nlohmann::json::parse_error& e) {
        return resp;
    }

    for (auto& output : registerOutput) {
        leet::errorCode = 0;

        resp.Homeserver = leet::Homeserver = cred->Homeserver;

        if (output["access_token"].is_string()) resp.accessToken = output["access_token"].get<std::string>();
        if (output["device_id"].is_string()) resp.deviceID = output["device_id"].get<std::string>();
        if (output["refresh_token"].is_string()) resp.refreshToken = output["refresh_token"].get<std::string>();
        if (output["user_id"].is_string()) resp.userID = output["user_id"].get<std::string>();
        if (output["expires_in_ms"].is_number_integer()) resp.Expiration = output["expires_in_ms"].get<int>();

        if (output["errcode"].is_string()) {
            leet::errorCode = 1;
            leet::Error = output["errcode"].get<std::string>();
            if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
        }
    }

    return resp;
}

leet::User::credentialsResponse leet::loginAccount(leet::User::Credentials* cred) {
    leet::User::credentialsResponse resp;
    nlohmann::json list;

    std::string actualType{};

    if (cred->Type == LEET_TYPE_TOKEN) {
        actualType = "m.login.token";
    } else {
        actualType = "m.login.password";
    }

    if (cred->deviceID.compare("")) {
        list["device_id"] = cred->deviceID;
    }

    list["identifier"]["type"] = "m.id.user"; // Currently only supported method
    list["identifier"]["user"] = cred->Username;
    list["initial_device_display_name"] = cred->displayName;

    if (cred->Type == LEET_TYPE_TOKEN) {
        list["token"] = cred->Token;
    } else {
        list["password"] = cred->Password;
    }

    list["refresh_token"] = cred->refreshToken;
    list["type"] = actualType;

    nlohmann::json loginOutput = { nlohmann::json::parse(leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/login"), list.dump())) };

    for (auto& output : loginOutput) {
        leet::errorCode = 0;

        resp.Homeserver = leet::Homeserver = cred->Homeserver;

        if (output["access_token"].is_string()) resp.accessToken = output["access_token"].get<std::string>();
        if (output["device_id"].is_string()) resp.deviceID = output["device_id"].get<std::string>();
        if (output["refresh_token"].is_string()) resp.refreshToken = output["refresh_token"].get<std::string>();
        if (output["user_id"].is_string()) resp.userID = output["user_id"].get<std::string>();
        if (output["expires_in_ms"].is_number_integer()) resp.Expiration = output["expires_in_ms"].get<int>();

        if (output["errcode"].is_string()) {
            leet::errorCode = 1;
            leet::Error = output["errcode"].get<std::string>();
            if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
        }
    }

    return resp;
}

const std::string leet::invokeRequest_Get(const std::string& URL) {
    /*
    auto ret = cpr::Get(cpr::Url{ URL });
    leet::networkStatusCode = ret.status_code;
    return ret.text;
    */
    leetRequest::URL url;
    leetRequest::Request request;

    url.parseURLFromString(URL);

    request.Host = url.Host;
    request.Endpoint = url.Endpoint;
    request.Query = url.Query;
    request.Port = url.Port;
    request.Protocol = url.Protocol;
    request.Type = leetRequest::LEET_REQUEST_REQTYPE_GET;
    request.userAgent = "LIBLEET_USER_AGENT";

    leetRequest::Response response = request.makeRequest();

    leet::networkStatusCode = response.statusCode;
    return response.Body;
}

const std::string leet::invokeRequest_Put(const std::string& URL, const std::string& Data) {
    /*
    auto ret = cpr::Put(cpr::Url{URL}, cpr::Body{Data});
    leet::networkStatusCode = ret.status_code;
    return ret.text;
    */
    leetRequest::URL url;
    leetRequest::Request request;

    url.parseURLFromString(URL);

    request.Host = url.Host;
    request.Endpoint = url.Endpoint;
    request.Query = url.Query;
    request.Port = url.Port;
    request.Protocol = url.Protocol;
    request.Type = leetRequest::LEET_REQUEST_REQTYPE_PUT;
    request.userAgent = "LIBLEET_USER_AGENT";
    request.Body = Data;

    leetRequest::Response response = request.makeRequest();

    leet::networkStatusCode = response.statusCode;
    return response.Body;
}

const std::string leet::invokeRequest_Post(const std::string& URL, const std::string& Data) {
    /*
    auto ret = cpr::Post(cpr::Url{URL}, cpr::Body{Data});
    leet::networkStatusCode = ret.status_code;
    return ret.text;
    */
    leetRequest::URL url;
    leetRequest::Request request;

    url.parseURLFromString(URL);

    request.Host = url.Host;
    request.Endpoint = url.Endpoint;
    request.Query = url.Query;
    request.Port = url.Port;
    request.Protocol = url.Protocol;
    request.Type = leetRequest::LEET_REQUEST_REQTYPE_POST;
    request.userAgent = "LIBLEET_USER_AGENT";
    request.Body = Data;

    leetRequest::Response response = request.makeRequest();

    leet::networkStatusCode = response.statusCode;
    return response.Body;
}

const std::string leet::invokeRequest_Get(const std::string& URL, const std::string& Authentication) {
    /*
    auto ret = cpr::Get(cpr::Url{ URL }, cpr::Header{{ "Authorization", "Bearer " + Authentication }});
    leet::networkStatusCode = ret.status_code;
    return ret.text;
    */
    leetRequest::URL url;
    leetRequest::Request request;

    url.parseURLFromString(URL);

    request.Host = url.Host;
    request.Endpoint = url.Endpoint;
    request.Query = url.Query;
    request.Port = url.Port;
    request.Protocol = url.Protocol;
    request.Type = leetRequest::LEET_REQUEST_REQTYPE_GET;
    request.userAgent = "LIBLEET_USER_AGENT";
    request.setAuthenticationHeader("Bearer " + Authentication);

    leetRequest::Response response = request.makeRequest();

    leet::networkStatusCode = response.statusCode;
    return response.Body;
}

const std::string leet::invokeRequest_Put(const std::string& URL, const std::string& Data, const std::string& Authentication) {
    /*
    auto ret = cpr::Put(cpr::Url{URL}, cpr::Body{Data}, cpr::Header{{ "Authorization", "Bearer " + Authentication }});
    leet::networkStatusCode = ret.status_code;
    return ret.text;
    */
    leetRequest::URL url;
    leetRequest::Request request;

    url.parseURLFromString(URL);

    request.Host = url.Host;
    request.Endpoint = url.Endpoint;
    request.Query = url.Query;
    request.Port = url.Port;
    request.Protocol = url.Protocol;
    request.Type = leetRequest::LEET_REQUEST_REQTYPE_PUT;
    request.userAgent = "LIBLEET_USER_AGENT";
    request.Body = Data;
    request.setAuthenticationHeader("Bearer " + Authentication);

    leetRequest::Response response = request.makeRequest();

    leet::networkStatusCode = response.statusCode;
    return response.Body;
}

const std::string leet::invokeRequest_Post(const std::string& URL, const std::string& Data, const std::string& Authentication) {
    /*
    auto ret = cpr::Post(cpr::Url{URL}, cpr::Body{Data}, cpr::Header{{ "Authorization", "Bearer " + Authentication }});
    leet::networkStatusCode = ret.status_code;
    return ret.text;
    */
    leetRequest::URL url;
    leetRequest::Request request;

    url.parseURLFromString(URL);

    request.Host = url.Host;
    request.Endpoint = url.Endpoint;
    request.Query = url.Query;
    request.Port = url.Port;
    request.Protocol = url.Protocol;
    request.Type = leetRequest::LEET_REQUEST_REQTYPE_POST;
    request.userAgent = "LIBLEET_USER_AGENT";
    request.Body = Data;
    request.setAuthenticationHeader("Bearer " + Authentication);

    leetRequest::Response response = request.makeRequest();

    leet::networkStatusCode = response.statusCode;
    return response.Body;
}

const std::string leet::invokeRequest_Post_File(const std::string& URL, const std::string& File, const std::string& Authentication) {
    /*
    std::filesystem::path file{ File }; if (!std::filesystem::exists(file)) return "";
    auto ret = cpr::Post(cpr::Url{URL}, cpr::Body{ cpr::File{File} }, cpr::Header{{ "Authorization", "Bearer " + Authentication }, {"Content-Type", "application/octet-stream"}});
    leet::networkStatusCode = ret.status_code;
    return ret.text;
    */
    std::filesystem::path file{ File }; if (!std::filesystem::exists(file)) return "";

    leetRequest::URL url;
    leetRequest::Request request;

    url.parseURLFromString(URL);

    request.Host = url.Host;
    request.Endpoint = url.Endpoint;
    request.Query = url.Query;
    request.Port = url.Port;
    request.Protocol = url.Protocol;
    request.Type = leetRequest::LEET_REQUEST_REQTYPE_POST;
    request.userAgent = "LIBLEET_USER_AGENT";
    request.Filename = File;
    request.setAuthenticationHeader("Bearer " + Authentication);
    request.setContentTypeHeader("application/octet-stream");

    leetRequest::Response response = request.makeRequest();

    leet::networkStatusCode = response.statusCode;
    return response.Body;
}

const std::string leet::invokeRequest_Post_File(const std::string& URL, const std::string& File) {
    /*
    std::filesystem::path file{ File }; if (!std::filesystem::exists(file)) return "";
    auto ret = cpr::Post(cpr::Url{URL}, cpr::Body{ cpr::File{File} }, cpr::Header{{"Content-Type", "application/octet-stream"}});
    leet::networkStatusCode = ret.status_code;
    return ret.text;
    */
    std::filesystem::path file{ File }; if (!std::filesystem::exists(file)) return "";

    leetRequest::URL url;
    leetRequest::Request request;

    url.parseURLFromString(URL);

    request.Host = url.Host;
    request.Endpoint = url.Endpoint;
    request.Query = url.Query;
    request.Port = url.Port;
    request.Protocol = url.Protocol;
    request.Type = leetRequest::LEET_REQUEST_REQTYPE_POST;
    request.userAgent = "LIBLEET_USER_AGENT";
    request.Filename = File;
    request.setContentTypeHeader("application/octet-stream");

    leetRequest::Response response = request.makeRequest();

    leet::networkStatusCode = response.statusCode;
    return response.Body;
}

const std::string leet::findUserID(const std::string& Alias, const std::string& Homeserver) {
    if (Alias[0] != '@')
        return "@" + Alias + ":" + Homeserver;
    return Alias;
}

const std::string leet::returnUserName(const std::string& userID) {
    std::string str;
    std::regex pattern{"@([^:]+):"};
    std::smatch reg;
    if (std::regex_search(userID, reg, pattern)) return reg[1].str();
    return str;
}

leet::User::Profile leet::getUserData(leet::User::credentialsResponse* resp, const std::string& userID) {
    leet::errorCode = 0;
    leet::User::Profile profile;

    if (userID[0] != '@') {
        return profile;
    }

    profile.userID = leet::findUserID(userID, resp->Homeserver);

    if (profile.userID.empty()) {
        leet::errorCode = 1;
        leet::friendlyError = "Failed to get User ID";
        return profile;
    }

    const std::string API { leet::getAPI("/_matrix/client/v3/profile/" + profile.userID) };
    const std::string Output = invokeRequest_Get(API);

    nlohmann::json reqOutput;

    try {
        reqOutput = { nlohmann::json::parse(Output) };
    } catch (const nlohmann::json::parse_error& e) {
        return profile;
    }

    for (auto& output : reqOutput) {
        if (output["avatar_url"].is_string()) profile.avatarURL = output["avatar_url"].get<std::string>();
        if (output["displayname"].is_string()) profile.displayName = output["displayname"].get<std::string>();

        if (output["errcode"].is_string()) {
            leet::errorCode = 1;
            leet::Error = output["errcode"].get<std::string>();
            if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
        }
    }

    leet::User::Profile userProfile;
    userProfile.userID = userID;
    std::vector<leet::User::Profile> User = { userProfile };

    profile.Devices = leet::returnDevicesFromUser(resp, User);

    return profile;
}

const std::vector<leet::User::Device> leet::returnDevicesFromUser(leet::User::credentialsResponse* resp, const std::vector<leet::User::Profile>& user) {
    std::vector<leet::User::Device> devices;

    nlohmann::json Body;
    nlohmann::json deviceKeys;

    for (auto& theUser : user) {
        deviceKeys[theUser.userID] = nlohmann::json::array();
        Body["device_keys"] = deviceKeys;
        Body["timeout"] = 10000;
    }

    const std::string Output = leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/keys/query"), Body.dump(), resp->accessToken);
    nlohmann::json returnOutput;

    try {
        returnOutput = nlohmann::json::parse(Output);
    } catch (const nlohmann::json::parse_error& e) {
        return devices;
    }

    for (auto& userID : user) {
        auto& deviceList = returnOutput["device_keys"][userID.userID];

        for (auto it = deviceList.begin(); it != deviceList.end(); ++it) {
            leet::User::Device device;

            device.userID = userID.userID;
            device.deviceID = it.key();

            nlohmann::json::json_pointer curve25519Pointer("/keys/curve25519:" + device.deviceID);
            if (it.value().contains(curve25519Pointer)) {
                device.curve25519Key = it.value()["keys"]["curve25519:" + device.deviceID];
            }
            nlohmann::json::json_pointer ed25519Pointer("/keys/ed25519:" + device.deviceID);
            if (it.value().contains(ed25519Pointer)) {
                device.ed25519Key = it.value()["keys"]["ed25519:" + device.deviceID];
            }
            nlohmann::json::json_pointer ed25519SigPointer("/signatures/" + userID.userID + "/ed25519:" + device.deviceID);
            if (it.value().contains(ed25519SigPointer)) {
                device.ed25519Signature = it.value()["signatures"][userID.userID]["ed25519:" + device.deviceID];
            }
            if (it.value().contains("/unsigned/device_display_name"_json_pointer)) {
                device.deviceDisplayName = it.value()["unsigned"]["device_display_name"];
            }

            device.olm = false;
            device.megolm = false;

            if (std::find(it.value()["algorithms"].begin(), it.value()["algorithms"].end(), "m.olm.v1.curve25519-aes-sha2") != it.value()["algorithms"].end())
                device.olm = true;

            if (std::find(it.value()["algorithms"].begin(), it.value()["algorithms"].end(), "m.megolm.v1.aes-sha2") != it.value()["algorithms"].end())
                device.megolm = true;

            devices.push_back(device);
        }
    }

    return devices;
}

const bool leet::checkIfUsernameIsAvailable(const std::string& Username) {
    leet::errorCode = 0;

    std::string theUsername = Username;

    if (Username[0] == '@') {
        theUsername = leet::returnUserName(Username);

        if (theUsername[0] == '@' || !theUsername.compare("")) {
            return false;
            leet::errorCode = 1;
        }
    }

    const std::string API { leet::getAPI("/_matrix/client/v3/register/available?username=" + theUsername) };
    const std::string Output = invokeRequest_Get(API);

    nlohmann::json reqOutput;

    try {
        reqOutput = { nlohmann::json::parse(Output) };
    } catch (const nlohmann::json::parse_error& e) {
        return false; // fallback to false is probably safest?
    }

    for (auto& output : reqOutput) {
        if (output["errcode"].is_string()) {
            leet::errorCode = 1;
            leet::Error = output["errcode"].get<std::string>();
            if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
            return false;
        }

        if (output["available"].is_boolean()) {
            return output["available"].get<bool>();
        }
    }

    return false;
}

const std::vector<leet::User::Profile> leet::returnUsersInRoom(leet::User::credentialsResponse* resp, leet::Room::Room* room) {
    std::vector<leet::User::Profile> vector;

    const std::string Output = leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v3/rooms/" + room->roomID + "/joined_members"), resp->accessToken);
    nlohmann::json returnOutput;

    try {
        returnOutput = nlohmann::json::parse(Output);
    } catch (const nlohmann::json::parse_error& e) {
        return vector;
    }

    auto& users = returnOutput["joined"];

    for (auto it = users.begin(); it != users.end(); ++it) {
        leet::User::Profile profile;

        if (it.value().contains("avatar_url") && !it.value()["avatar_url"].is_null()) profile.avatarURL = it.value()["avatar_url"];
        if (it.value().contains("display_name") && !it.value()["display_name"].is_null()) profile.displayName = it.value()["display_name"];

        profile.userID = it.key();

        leet::User::Profile userProfile;
        userProfile.userID = it.key();
        std::vector<leet::User::Profile> User = { userProfile };
        profile.Devices = leet::returnDevicesFromUser(resp, User);

        vector.push_back(profile);
    }

    return vector;
}

const std::vector<std::string> leet::findRoomAliases(leet::User::credentialsResponse* resp, const std::string& roomID) {
    std::vector<std::string> ret;
    const std::string Output = leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v3/rooms/" + roomID + "/aliases"), resp->accessToken);

    nlohmann::json reqOutput;

    try {
        reqOutput = { nlohmann::json::parse(Output) };
    } catch (const nlohmann::json::parse_error& e) {
        return ret;
    }

    for (auto& output : reqOutput) {
        if (!output["aliases"].is_null()) {
            leet::errorCode = 0;
            return output["aliases"].get<std::vector<std::string>>();
        } else if (!output["errcode"].is_null()) {
            leet::errorCode = 1;
            leet::Error = output["errcode"].get<std::string>();
            if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();

            return ret;
        }
    }

    return ret;
}

const std::string leet::findRoomID(std::string Alias) {
    leet::errorCode = 0;

    if (Alias[0] == '!') { // It's a proper room ID already
        return Alias;
    }

    // Replace the '#' character with '%23' so that Matrix is happy
    Alias.replace(0, 1, "%23");

    const std::string Output = leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v3/directory/room/") + Alias);
    nlohmann::json reqOutput;

    try {
        reqOutput = { nlohmann::json::parse(Output) };
    } catch (const nlohmann::json::parse_error& e) {
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

const std::vector<leet::Room::Room> leet::returnRooms(leet::User::credentialsResponse* resp, const int Limit) {
    std::vector<leet::Room::Room> vector;
    std::vector<leet::Room::Room> vectorWithVal;

    const std::string Output = leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v3/joined_rooms"), resp->accessToken);
    nlohmann::json returnOutput;

    try {
        returnOutput = nlohmann::json::parse(Output);
    } catch (const nlohmann::json::parse_error& e) {
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

leet::Room::Room leet::returnRoom(leet::User::credentialsResponse* resp, leet::Room::Room* room) {
    leet::Room::Room theRoom;
    nlohmann::json returnOutput;

    try {
        returnOutput = nlohmann::json::parse(leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v1/rooms/" + room->roomID + "/hierarchy"), resp->accessToken));
    } catch (const nlohmann::json::parse_error& e) {
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
    const std::string APIUrl { "/_matrix/client/v3/rooms/" + room->roomID + "/upgrade" };

    nlohmann::json body;

    body["new_version"] = std::to_string(Version);

    const std::string Output { leet::invokeRequest_Post(leet::getAPI(APIUrl), body.dump(), resp->accessToken) };

    nlohmann::json reqOutput;

    try {
        reqOutput = { nlohmann::json::parse(Output) };
    }  catch (const nlohmann::json::parse_error& e) {
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
    leet::Room::Room theRoom;
    nlohmann::json theJson;

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
    nlohmann::json reqOutput;

    try {
        reqOutput = { nlohmann::json::parse(Output) };
    } catch (const nlohmann::json::parse_error& e) {
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

const std::vector<leet::Room::Room> leet::returnRoomIDs(leet::User::credentialsResponse* resp) {
    std::vector<leet::Room::Room> vector;

    const std::string Output = leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v3/joined_rooms"), resp->accessToken);
    nlohmann::json returnOutput;

    try {
        returnOutput = nlohmann::json::parse(Output);
    } catch (const nlohmann::json::parse_error& e) {
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

const std::vector<leet::Room::Room> leet::returnRoomsInSpace(leet::User::credentialsResponse* resp, const std::string& spaceID, const int Limit) {
    std::vector<leet::Room::Room> rooms;
    if (spaceID[0] != '!') {
        return rooms;
    }

    const std::string Output { leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v1/rooms/" + spaceID + "/hierarchy?limit=" + std::to_string(Limit)), resp->accessToken) };
    nlohmann::json returnOutput;

    try {
        returnOutput = nlohmann::json::parse(Output);
    } catch (const nlohmann::json::parse_error& e) {
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
    nlohmann::json list;

    list["timeout"] = Timeout;
    list["typing"] = Typing;

    const std::string Output { leet::invokeRequest_Put(leet::getAPI("/_matrix/client/v3/rooms/" + room->roomID + "/typing/" + resp->userID), list.dump(), resp->accessToken) };

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

void leet::inviteUserToRoom(leet::User::credentialsResponse* resp, leet::Room::Room* room, const std::string& Reason) {
    nlohmann::json request;

    request["reason"] = Reason;
    request["user_id"] = resp->userID;

    const std::string Output { leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/rooms/" + room->roomID + "/invite"), request.dump(), resp->accessToken) };
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

void leet::joinRoom(leet::User::credentialsResponse* resp, leet::Room::Room* room, const std::string& Reason) {
    nlohmann::json body;

    if (!Reason.compare("")) {
        body["reason"] = Reason;
    }

    const std::string Output { leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/rooms/" + room->roomID + "/join"), body.dump(), resp->accessToken) };

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

    // You may want to refresh your room list and call /sync after this
}

void leet::leaveRoom(leet::User::credentialsResponse* resp, leet::Room::Room* room, const std::string& Reason) {
    nlohmann::json body;

    if (!Reason.compare("")) {
        body["reason"] = Reason;
    }

    const std::string Output { leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/rooms/" + room->roomID + "/leave"), body.dump(), resp->accessToken) };

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

void leet::kickUserFromRoom(leet::User::credentialsResponse* resp, leet::Room::Room* room, leet::User::Profile* profile, const std::string& Reason) {
    nlohmann::json body;

    if (!Reason.compare("")) {
        body["reason"] = Reason;
    }

    body["user_id"] = profile->userID;

    const std::string Output { leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/rooms/" + room->roomID + "/kick"), body.dump(), resp->accessToken) };

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

void leet::banUserFromRoom(leet::User::credentialsResponse* resp, leet::Room::Room* room, leet::User::Profile* profile, const std::string& Reason) {
    nlohmann::json body;

    if (!Reason.compare("")) {
        body["reason"] = Reason;
    }

    body["user_id"] = profile->userID;

    const std::string Output { leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/rooms/" + room->roomID + "/ban"), body.dump(), resp->accessToken) };

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

void leet::unbanUserFromRoom(leet::User::credentialsResponse* resp, leet::Room::Room* room, leet::User::Profile* profile, const std::string& Reason) {
    nlohmann::json body;

    if (!Reason.compare("")) {
        body["reason"] = Reason;
    }

    body["user_id"] = profile->userID;

    const std::string Output { leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/rooms/" + room->roomID + "/unban"), body.dump(), resp->accessToken) };

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

const bool leet::getVisibilityOfRoom(leet::User::credentialsResponse* resp, leet::Room::Room* room) {
    const std::string Output { leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v3/directory/list/room/" + room->roomID), resp->accessToken) };

    nlohmann::json reqOutput;

    try {
        reqOutput = { nlohmann::json::parse(Output) };
    } catch (const nlohmann::json::parse_error& e) {
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

void leet::setVisibilityOfRoom(leet::User::credentialsResponse* resp, leet::Room::Room* room, const bool Visibility) {
    nlohmann::json body;

    body["visibility"] = Visibility ? "public" : "private";

    const std::string Output { leet::invokeRequest_Put(leet::getAPI("/_matrix/client/v3/directory/list/room/" + room->roomID), body.dump(), resp->accessToken) };

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

void leet::setReadMarkerPosition(leet::User::credentialsResponse* resp, leet::Room::Room* room,
        leet::Event::Event* fullyReadEvent, leet::Event::Event* readEvent, leet::Event::Event* privateReadEvent) {
    nlohmann::json body;

    body["m.fully_read"] = fullyReadEvent->eventID;
    body["m.read"] = readEvent->eventID;
    body["m.read.private"] = privateReadEvent->eventID;

    const std::string Output { leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/rooms/" + room->roomID + "/read_markers"), body.dump(), resp->accessToken) };

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

template <typename T> T leet::saveToFile(const std::string& File, T Data) {
    std::filesystem::path file{ File };
    std::ofstream outputFile;

    if (!std::filesystem::create_directories(file.parent_path()) && !std::filesystem::is_directory(file.parent_path())) {
        throw("Failed to create directory");
    }

    outputFile.open(File);
    outputFile << Data;
    outputFile.close();

    return T();
}

template <typename T> T leet::loadFromFile(const std::string& File) {
    std::filesystem::path file{ File };
    std::string line;
    if (std::filesystem::exists(file)) {
        std::ifstream inputFile(File);

        if (!inputFile.is_open()) {
            return false;
        }

        while (std::getline(inputFile, line)) {
            T ret = std::stoi(line);

            inputFile.close();
            return ret;
        }

        inputFile.close();
    }

    return false;
}

const bool leet::saveTransID(const std::string& File) {
    leet::saveToFile<int>(File, leet::transID);
    return true;
}

const bool leet::loadTransID(const std::string& File) {
    return (leet::transID = leet::loadFromFile<int>(File));
}

leet::Attachment::Attachment leet::uploadFile(leet::User::credentialsResponse* resp, const std::string& File) {
    leet::Attachment::Attachment theAttachment;
    const std::string Output = leet::invokeRequest_Post_File(leet::getAPI("/_matrix/media/v3/upload"), File, resp->accessToken);

    nlohmann::json returnOutput;
    try {
        returnOutput = { nlohmann::json::parse(Output) };
    } catch (const nlohmann::json::parse_error& e) {
        return theAttachment;
    }

    for (auto& output : returnOutput) {
        leet::errorCode = 0;
        leet::Error = "";

        if (output["content_uri"].is_string()) {
            theAttachment.URL = output["content_uri"].get<std::string>();
            return theAttachment;
        }

        if (output["errcode"].is_string()) {
            leet::errorCode = 1;
            leet::Error = output["errcode"].get<std::string>();
            if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
            return theAttachment;
        }
    }

    return theAttachment;
}

const std::string leet::decodeFile(leet::User::credentialsResponse* resp, leet::Attachment::Attachment* Attachment) {
    std::string Server{};
    std::string ID{};
    std::string File{Attachment->URL};
    std::size_t it = File.find("mxc://");

    if (it != std::string::npos) {
        it += 6;
        std::size_t nextSlash = File.find("/", it);

        if (nextSlash != std::string::npos) {
            Server = File.substr(it, nextSlash - it);
            ID = File.substr(nextSlash + 1);
        } else {
            leet::errorCode = 1;
            return "";
        }
    }

    return leet::getAPI("/_matrix/media/v3/download/" + Server + "/" + ID + "?allow_redirect=false");
}

const bool leet::downloadFile(leet::User::credentialsResponse* resp, leet::Attachment::Attachment* Attachment, const std::string& outputFile) {
    std::string Server{};
    std::string ID{};
    std::string File{Attachment->URL};
    std::size_t it = File.find("mxc://");

    if (it != std::string::npos) {
        it += 6;
        std::size_t nextSlash = File.find("/", it);

        if (nextSlash != std::string::npos) {
            Server = File.substr(it, nextSlash - it);
            ID = File.substr(nextSlash + 1);
        } else {
            leet::errorCode = 1;
            return false;
        }
    }

    // Now that we have what we need, let's make a request
    const std::string API { leet::getAPI("/_matrix/media/v3/download/" + Server + "/" + ID + "?allow_redirect=false") };
    std::filesystem::path file{ outputFile };

    if (!std::filesystem::create_directories(file.parent_path()) && !std::filesystem::is_directory(file.parent_path())) {
        leet::errorCode = 1;
        return false;
    }

    /*
    std::ofstream of(outputFile, std::ios::binary);
    cpr::Response response = cpr::Download(of, cpr::Url{API});
    if (response.status_code == 200) {
        return true;
    }
    */
    leetRequest::URL url;
    leetRequest::Request request;

    url.parseURLFromString(API);

    request.Host = url.Host;
    request.Endpoint = url.Endpoint;
    request.Query = url.Query;
    request.Port = url.Port;
    request.Protocol = url.Protocol;
    request.Type = leetRequest::LEET_REQUEST_REQTYPE_GET;
    request.userAgent = "LIBLEET_USER_AGENT";
    request.outputFile = outputFile;

    return request.downloadFile();
}

leet::URL::urlPreview leet::getURLPreview(leet::User::credentialsResponse* resp, const std::string& URL, int64_t time) {
    leet::URL::urlPreview preview;
    const std::string Output { leet::invokeRequest_Get(leet::getAPI("/_matrix/media/v3/preview_url?ts=" + std::to_string(time) + "&url=" + URL), resp->accessToken) };

    nlohmann::json reqOutput;

    try {
        reqOutput = { nlohmann::json::parse(Output) };
    } catch (const nlohmann::json::parse_error& e) {
        return preview;
    }

    preview.URL = URL;
    preview.Time = time;

    for (auto& output : reqOutput) {
        if (output["og:image"].is_string()) preview.imageURL = output["og:image"].get<std::string>();
        if (output["og:image:type"].is_string()) preview.Type = output["og:image:type"].get<std::string>();
        if (output["og:image:width"].is_number_integer()) preview.imageWidth = output["og:image:width"].get<int>();
        if (output["og:image:height"].is_number_integer()) preview.imageHeight = output["og:image:height"].get<int>();
        if (output["matrix:image:size"].is_number_integer()) preview.imageSize = output["matrix:image:size"].get<int>();
        if (output["og:title"].is_string()) preview.Title = output["og:title"].get<std::string>();
        if (output["og:description"].is_string()) preview.Description = output["og:description"].get<std::string>();
    }

    return preview;
}

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
    std::string messageType = msg->messageType;

    switch (msg->msgType) {
        case leet::LEET_MESSAGETYPE_IMAGE:
            messageType = "m.image";
            break;
        case leet::LEET_MESSAGETYPE_AUDIO:
            messageType = "m.audio";
            break;
        case leet::LEET_MESSAGETYPE_VIDEO:
            messageType = "m.video";
            break;
        case leet::LEET_MESSAGETYPE_FILE:
            messageType = "m.file";
            break;
        case leet::LEET_MESSAGETYPE_NOTICE:
            messageType = "m.notice";
            break;
        case leet::LEET_MESSAGETYPE_EMOTE:
            messageType = "m.emote";
            break;
        case leet::LEET_MESSAGETYPE_STRING:
            messageType = msg->messageType;
            if (!messageType.compare("")) {
                messageType = "m.text";
            }
            break;
        default:
            messageType = "m.text";
            break;
    }

    if (msg->bodyType == leet::LEET_BODYTYPE_SLIM) {
        throw std::runtime_error{ "You seem like a funny guy." };
    } else if (msg->bodyType == leet::LEET_BODYTYPE_SPEEDIE) {
        throw std::runtime_error{ "Why would you want to be a lazy fatass like me?" };
    }

    nlohmann::json list;

    if (!messageType.compare("m.image") || !messageType.compare("m.audio") || !messageType.compare("m.video") || !messageType.compare("m.file")) {
        if (msg->attachmentURL[0] != 'm' || msg->attachmentURL[1] != 'x' || msg->attachmentURL[2] != 'c') {
            leet::errorCode = 1;
            return;
        }

        list["type"] = "m.room.message";
        list["room_id"] = room->roomID;

        if ((msg->bodyType == leet::LEET_BODYTYPE_BASIC) || (msg->bodyType == leet::LEET_BODYTYPE_BOTH)) {
            list["body"] = msg->messageText;
        }

        if ((msg->bodyType == leet::LEET_BODYTYPE_FORMATTED) || (msg->bodyType == leet::LEET_BODYTYPE_BOTH)) {
            list["formatted_body"] = msg->formattedText;

            if (!msg->Format.compare("")) {
                list["format"] = "org.matrix.custom.html";
            } else {
                list["format"] = msg->Format;
            }
        }

        list["msgtype"] = messageType;
        list["url"] = msg->attachmentURL;

        list["m.mentions"]["user_ids"] = msg->mentionedUserIDs;

        if (!msg->replyEvent.eventID.compare("")) {
            list["m.relates_to"]["m.in_reply_to"]["event_id"] = msg->replyEvent.eventID;
        }
    } else {
        list["type"] = "m.room.message";
        list["room_id"] = room->roomID;
        list["body"] = msg->messageText;

        if ((msg->bodyType == leet::LEET_BODYTYPE_BASIC) || (msg->bodyType == leet::LEET_BODYTYPE_BOTH)) {
            list["body"] = msg->messageText;
        }

        if ((msg->bodyType == leet::LEET_BODYTYPE_FORMATTED) || (msg->bodyType == leet::LEET_BODYTYPE_BOTH)) {
            list["formatted_body"] = msg->formattedText;

            if (!msg->Format.compare("")) {
                list["format"] = "org.matrix.custom.html";
            } else {
                list["format"] = msg->Format;
            }
        }

        list["m.mentions"]["user_ids"] = msg->mentionedUserIDs;

        if (!msg->replyEvent.eventID.compare("")) {
            list["m.relates_to"]["m.in_reply_to"]["event_id"] = msg->replyEvent.eventID;
        }
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

    if ((msg->bodyType == leet::LEET_BODYTYPE_BASIC) || (msg->bodyType == leet::LEET_BODYTYPE_BOTH)) {
        Body["body"] = msg->messageText;
    }

    if ((msg->bodyType == leet::LEET_BODYTYPE_FORMATTED) || (msg->bodyType == leet::LEET_BODYTYPE_BOTH)) {
        Body["formatted_body"] = msg->formattedText;

        if (!msg->Format.compare("")) {
            Body["format"] = "org.matrix.custom.html";
        } else {
            Body["format"] = msg->Format;
        }
    }

    Body["content"]["msgtype"] = "m.text";
    Body["m.mentions"]["user_ids"] = msg->mentionedUserIDs;

    if (!msg->replyEvent.eventID.compare("")) {
        Body["m.relates_to"]["m.in_reply_to"]["event_id"] = msg->replyEvent.eventID;
    }

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

leet::VOIP::Credentials leet::returnTurnCredentials(leet::User::credentialsResponse* resp) {
    leet::VOIP::Credentials cred;

    const std::string Output { leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v3/voip/turnServer"), resp->accessToken) };

    nlohmann::json reqOutput;

    try {
        reqOutput = { nlohmann::json::parse(Output) };
    }  catch (const nlohmann::json::parse_error& e) {
        return cred;
    }

    for (auto& output : reqOutput) {
        leet::errorCode = 0;

        if (output["uris"].is_array()) cred.URI = output["uris"];
        if (output["username"].is_string()) cred.Username = output["username"].get<std::string>();
        if (output["password"].is_string()) cred.Password = output["password"].get<std::string>();
        if (output["ttl"].is_number_integer()) cred.timeToLiveIn = output["ttl"].get<int>();
        if (output["errcode"].is_string()) leet::Error = output["errcode"].get<std::string>();
        if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
    }

    return cred;
}

const std::string leet::getAPI(const std::string& API) {
    return leet::Homeserver + API;
}

const int leet::generateTransID() {
    return ++leet::transID;
}

const std::string leet::returnServerDiscovery(std::string Server) {
    leet::errorCode = 0;

    if (Server[0] != 'h' || Server[1] != 't' || Server[2] != 't' || Server[3] != 'p') {
        Server = "https://" + Server;
    }

    const std::string Output = leet::invokeRequest_Get(Server + "/.well-known/matrix/client");

    if (nlohmann::json::accept(Output)) {
        nlohmann::json reqOutput;

        try {
            reqOutput = { nlohmann::json::parse(Output) };
        } catch (const nlohmann::json::parse_error& e) {
            return Server;
        }

        for (auto& output : reqOutput)
            if (output["m.homeserver"]["base_url"].is_string())
                return output["m.homeserver"]["base_url"].get<std::string>();
    }

    return Server;
}

const std::string leet::returnHomeServerFromString(const std::string& userID) {
    std::string uid{userID};
    if (uid[0] != '@') {
        leet::errorCode = 1;
        return "";
    }

    std::size_t colonPosition = uid.find(':');
    if (colonPosition != std::string::npos) return uid.substr(colonPosition + 1);

    return "";
}

std::vector<std::string> leet::returnSupportedSpecs() {
    std::vector<std::string> vector;
    const std::string APIUrl { "/_matrix/client/versions" };

    std::string Output { leet::invokeRequest_Get(leet::getAPI(APIUrl)) };

    nlohmann::json reqOutput;

    try {
        reqOutput = { nlohmann::json::parse(Output) };
    } catch (const nlohmann::json::parse_error& e) {
        return vector;
    }

    for (auto& output : reqOutput) {
        leet::errorCode = 0;

        if (output["versions"].is_array()) {
            vector = output["versions"];
            break;
        }
    }

    return vector;
}

const int leet::returnMaxUploadLimit(leet::User::credentialsResponse* resp) {
    const std::string APIUrl { "/_matrix/media/v3/config" };
    const std::string Output { leet::invokeRequest_Get(leet::getAPI(APIUrl), resp->accessToken) };

    nlohmann::json reqOutput;

    try {
        reqOutput = { nlohmann::json::parse(Output) };
    }  catch (const nlohmann::json::parse_error& e) {
        return 0;
    }

    for (auto& output : reqOutput) {
        leet::errorCode = 0;

        if (output["m.upload.size"].is_number_integer()) return output["m.upload.size"].get<int>();
        if (output["errcode"].is_string()) leet::Error = output["errcode"].get<std::string>();
        if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
    }

    return 0;
}

const bool leet::checkError() {
    if (leet::errorCode != 0) {
        return false;
    }

    return true;
}
