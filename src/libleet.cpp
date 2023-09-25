/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

#include <iostream>
#include <sstream>
#include <future>
#include <nlohmann/json.hpp>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

#include "libleet.hpp"

void leet::setSettings(leet::MatrixOptions *options) {
    leet::MatrixOption.Homeserver = options->Homeserver;
    leet::MatrixOption.Type = options->Type;
}

/* std::string leet::User::Credentials.Username = <username>
 * std::string leet::User::Credentials.Password = <password>
 * std::string leet::User::Credentails.DisplayName = <display name>
 *
 * leet::User::Credentials passed to function.
 */
void leet::saveCredentials(leet::User::Credentials *cred) {
    leet::MatrixOption.Credentials.Username = cred->Username;
    leet::MatrixOption.Credentials.Password = cred->Password;
    leet::MatrixOption.Credentials.DisplayName = cred->DisplayName;
    leet::MatrixOption.Credentials.DeviceID = cred->DeviceID;
}

std::future<std::string> invoke(std::string const& url, std::string const& body) {
  return std::async(std::launch::async,
    [](std::string const& url, std::string const& body) mutable {
      std::list<std::string> header;
      header.push_back("Content-Type: application/json");

      //curlpp::Cleanup clean;
      curlpp::Easy r;
      r.setOpt(new curlpp::options::Url(url));
      r.setOpt(new curlpp::options::HttpHeader(header));
      r.setOpt(new curlpp::options::PostFields(body));
      r.setOpt(new curlpp::options::PostFieldSize(body.length()));

      std::ostringstream response;
      r.setOpt(new curlpp::options::WriteStream(&response));

      r.perform();

      return std::string(response.str());
    }, url, body);
}

/* api: /_matrix/client/v3/login */
std::string leet::getAPI(std::string api) {
    std::ostringstream url;
    url << leet::MatrixOption.Homeserver << api;

    return url.str();
}

void leet::connectHomeserver() {
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
        //{ "token", ... },
    };

    //leet::jsonCache = list.dump();
    invoke(leet::getAPI("/_matrix/client/v3/login"), list.dump());
}
