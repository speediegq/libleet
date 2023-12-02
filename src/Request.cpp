/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU Affero General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

const std::string leet::invokeRequest_Get(const std::string& URL) {
    return cpr::Get(cpr::Url{ URL }).text;
}
const std::string leet::invokeRequest_Put(const std::string& URL, const std::string& Data) {
    return cpr::Put(cpr::Url{URL}, cpr::Body{Data}).text;
}
const std::string leet::invokeRequest_Post(const std::string& URL, const std::string& Data) {
    return cpr::Post(cpr::Url{URL}, cpr::Body{Data}).text;
}
const std::string leet::invokeRequest_Get(const std::string& URL, const std::string& Authentication) {
    return cpr::Get(cpr::Url{ URL }, cpr::Header{{ "Authorization", "Bearer " + Authentication }}).text;
}
const std::string leet::invokeRequest_Put(const std::string& URL, const std::string& Data, const std::string& Authentication) {
    return cpr::Put(cpr::Url{URL}, cpr::Body{Data}, cpr::Header{{ "Authorization", "Bearer " + Authentication }}).text;
}
const std::string leet::invokeRequest_Post(const std::string& URL, const std::string& Data, const std::string& Authentication) {
    return cpr::Post(cpr::Url{URL}, cpr::Body{Data}, cpr::Header{{ "Authorization", "Bearer " + Authentication }}).text;
}
const std::string leet::invokeRequest_Post_File(const std::string& URL, const std::string& File, const std::string& Authentication) {
    std::filesystem::path file{ File }; if (!std::filesystem::exists(file)) return "";
    return cpr::Post(cpr::Url{URL}, cpr::Body{ cpr::File{File} }, cpr::Header{{ "Authorization", "Bearer " + Authentication }, {"Content-Type", "application/octet-stream"}}).text;
}
const std::string leet::invokeRequest_Post_File(const std::string& URL, const std::string& File) {
    std::filesystem::path file{ File }; if (!std::filesystem::exists(file)) return "";
    return cpr::Post(cpr::Url{URL}, cpr::Body{ cpr::File{File} }, cpr::Header{{"Content-Type", "application/octet-stream"}}).text;
}
