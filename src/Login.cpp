/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU Affero General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

std::vector<std::string> leet::returnSupportedLoginTypes() {
    using json = nlohmann::json;
    std::vector<std::string> vector;
    const std::string APIUrl { "/_matrix/client/v3/login" };

    std::string Output { leet::invokeRequest_Get(leet::getAPI(APIUrl)) };

    json reqOutput;
    try {
        reqOutput = json::parse(Output);
    } catch (const json::parse_error& e) {
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

leet::User::CredentialsResponse leet::refreshAccessToken(leet::User::CredentialsResponse* resp) {
    using json = nlohmann::json;
    leet::User::CredentialsResponse newResponse = *resp;

    if (!newResponse.refreshToken.compare("")) {
        return newResponse;
    }

    json body;

    body["refresh_token"] = newResponse.refreshToken;

    json refreshOutput;

    try {
        refreshOutput = { json::parse(leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/refresh"), body.dump())) };
    } catch (const json::parse_error& e) {
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
    using json = nlohmann::json;

    json body;

    try {
        body = { json::parse(leet::invokeRequest_Get(leet::getAPI("/_matrix/client/v1/register/m.login.registration_token/validity?token=" + Token))) };
    } catch (const json::parse_error& e) {
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

leet::User::CredentialsResponse leet::registerAccount(leet::User::Credentials* cred) {
    leet::User::CredentialsResponse resp;
    using json = nlohmann::json;

    std::string theUsername = cred->Username;

    if (cred->Username[0] == '@') {
        theUsername = leet::returnUserName(cred->Username);

        if (theUsername[0] == '@' || !theUsername.compare("")) {
            return resp;
            leet::errorCode = 1;
        }
    }

    json body;

    if (cred->deviceID.compare("")) {
        body["device_id"] = cred->deviceID;
    }

    body["inhibit_login"] = false;
    body["initial_device_display_name"] = cred->displayName;
    body["username"] = theUsername;
    body["password"] = cred->Password;
    body["refresh_token"] = cred->refreshToken;

    json registerOutput;

    try {
        registerOutput = { json::parse(leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/register"), body.dump())) };
    } catch (const json::parse_error& e) {
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

leet::User::CredentialsResponse leet::loginAccount(leet::User::Credentials* cred) {
    leet::User::CredentialsResponse resp;
    using json = nlohmann::json;
    json list;

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

    json loginOutput = { json::parse(leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/login"), list.dump())) };

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
