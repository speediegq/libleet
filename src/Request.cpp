/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU Affero General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

const std::string leet::invokeRequest_Get(const std::string& URL) {
    auto ret = cpr::Get(cpr::Url{ URL });
    leet::networkStatusCode = ret.status_code;
    return ret.text;
}
const std::string leet::invokeRequest_Put(const std::string& URL, const std::string& Data) {
    auto ret = cpr::Put(cpr::Url{URL}, cpr::Body{Data});
    leet::networkStatusCode = ret.status_code;
    return ret.text;
}
const std::string leet::invokeRequest_Post(const std::string& URL, const std::string& Data) {
    auto ret = cpr::Post(cpr::Url{URL}, cpr::Body{Data});
    leet::networkStatusCode = ret.status_code;
    return ret.text;
}
const std::string leet::invokeRequest_Get(const std::string& URL, const std::string& Authentication) {
    auto ret = cpr::Get(cpr::Url{ URL }, cpr::Header{{ "Authorization", "Bearer " + Authentication }});
    leet::networkStatusCode = ret.status_code;
    return ret.text;
}
const std::string leet::invokeRequest_Put(const std::string& URL, const std::string& Data, const std::string& Authentication) {
    auto ret = cpr::Put(cpr::Url{URL}, cpr::Body{Data}, cpr::Header{{ "Authorization", "Bearer " + Authentication }});
    leet::networkStatusCode = ret.status_code;
    return ret.text;
}
const std::string leet::invokeRequest_Post(const std::string& URL, const std::string& Data, const std::string& Authentication) {
    auto ret = cpr::Post(cpr::Url{URL}, cpr::Body{Data}, cpr::Header{{ "Authorization", "Bearer " + Authentication }});
    leet::networkStatusCode = ret.status_code;
    return ret.text;
}
const std::string leet::invokeRequest_Post_File(const std::string& URL, const std::string& File, const std::string& Authentication) {
    std::filesystem::path file{ File }; if (!std::filesystem::exists(file)) return "";
    auto ret = cpr::Post(cpr::Url{URL}, cpr::Body{ cpr::File{File} }, cpr::Header{{ "Authorization", "Bearer " + Authentication }, {"Content-Type", "application/octet-stream"}});
    leet::networkStatusCode = ret.status_code;
    return ret.text;
}
const std::string leet::invokeRequest_Post_File(const std::string& URL, const std::string& File) {
    std::filesystem::path file{ File }; if (!std::filesystem::exists(file)) return "";
    auto ret = cpr::Post(cpr::Url{URL}, cpr::Body{ cpr::File{File} }, cpr::Header{{"Content-Type", "application/octet-stream"}});
    leet::networkStatusCode = ret.status_code;
    return ret.text;
}
