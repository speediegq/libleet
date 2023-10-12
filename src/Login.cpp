/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

leet::User::CredentialsResponse leet::connectHomeserver() {
    leet::User::CredentialsResponse resp;
    using json = nlohmann::json;

    std::string actualType{};

    if (leet::MatrixOption.Credentials.Type == TPassword) {
        actualType = "m.login.password";
    } else if (leet::MatrixOption.Credentials.Type == TToken) {
        actualType = "m.login.token";
    }

    json list = {
        { "device_id", leet::MatrixOption.Credentials.DeviceID },
        { "identifier", {
            { "type", "m.id.user" },
            { "user", leet::MatrixOption.Credentials.Username },
        } },
        { "initial_device_display_name", leet::MatrixOption.Credentials.DisplayName },
        { "password", leet::MatrixOption.Credentials.Password },
        { "refresh_token", leet::MatrixOption.Credentials.RefreshToken },
        { "type", actualType },
    };

    /* Make a network request attempting a login */
    json loginOutput = {
        json::parse(leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/login"), list.dump()))
    };

    for (auto &output : loginOutput) {
        leet::errorCode = 0;

        resp.Homeserver = leet::MatrixOption.Homeserver;

        if (output["access_token"].is_string()) resp.AccessToken = output["access_token"].get<std::string>();
        if (output["device_id"].is_string()) resp.DeviceID = output["device_id"].get<std::string>();
        if (output["refresh_token"].is_string()) resp.RefreshToken = output["refresh_token"].get<std::string>();
        if (output["user_id"].is_string()) resp.UserID = output["user_id"].get<std::string>();

        if (output["errcode"].is_string()) {
            leet::errorCode = 1;
            leet::Error = output["errcode"].get<std::string>();
            if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
        }
    }

    return resp;
}
