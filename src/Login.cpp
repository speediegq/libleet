/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU Affero General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

leet::User::CredentialsResponse leet::loginAccount() {
    leet::User::CredentialsResponse resp;
    using json = nlohmann::json;

    std::string actualType{};

    if (leet::MatrixOption.Credentials.Type == TPassword) {
        actualType = "m.login.password";
    } else if (leet::MatrixOption.Credentials.Type == TToken) {
        actualType = "m.login.token";
    } else {
        actualType = "m.login.password";
    }

    json list;

    list["device_id"] = leet::MatrixOption.Credentials.deviceID;
    list["identifier"]["type"] = "m.id.user"; // Currently only supported method
    list["identifier"]["user"] = leet::MatrixOption.Credentials.Username;
    list["initial_device_display_name"] = leet::MatrixOption.Credentials.displayName;

    if (leet::MatrixOption.Credentials.Type == TToken) {
        list["token"] = leet::MatrixOption.Credentials.Token;
    } else {
        list["password"] = leet::MatrixOption.Credentials.Password;
    }

    list["refresh_token"] = leet::MatrixOption.Credentials.refreshToken;
    list["type"] = actualType;

    /* Make a network request attempting a login */
    json loginOutput = {
        json::parse(leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/login"), list.dump()))
    };

    for (auto &output : loginOutput) {
        leet::errorCode = 0;

        resp.Homeserver = leet::MatrixOption.Homeserver;

        if (output["access_token"].is_string()) resp.accessToken = output["access_token"].get<std::string>();
        if (output["device_id"].is_string()) resp.deviceID = output["device_id"].get<std::string>();
        if (output["refresh_token"].is_string()) resp.refreshToken = output["refresh_token"].get<std::string>();
        if (output["user_id"].is_string()) resp.userID = output["user_id"].get<std::string>();

        if (output["errcode"].is_string()) {
            leet::errorCode = 1;
            leet::Error = output["errcode"].get<std::string>();
            if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
        }
    }

    return resp;
}
