/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU Affero General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

leet::VOIP::Credentials leet::returnTurnCredentials(leet::User::credentialsResponse* resp) {
    leet::VOIP::Credentials cred;

    using json = nlohmann::json;
    const std::string APIUrl { "/_matrix/client/v3/voip/turnServer" };

    const std::string Output { leet::invokeRequest_Get(leet::getAPI(APIUrl), resp->accessToken) };

    json reqOutput;

    try {
        reqOutput = { json::parse(Output) };
    }  catch (const json::parse_error& e) {
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
