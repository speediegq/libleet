/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

std::string leet::invokeRequest_Get(const std::string URL) {
    return cpr::Get(cpr::Url{ URL }).text;
}
std::string leet::invokeRequest_Put(const std::string URL, const std::string Data) {
    return cpr::Put(cpr::Url{URL}, cpr::Body{Data}).text;
}
std::string leet::invokeRequest_Post(const std::string URL, const std::string Data) {
    return cpr::Post(cpr::Url{URL}, cpr::Body{Data}).text;
}
std::string leet::invokeRequest_Get(const std::string URL, const std::string Authentication) {
    return cpr::Get(cpr::Url{ URL }, cpr::Header{{ "Authorization", "Bearer " + Authentication }}).text;
}
std::string leet::invokeRequest_Put(const std::string URL, const std::string Data, const std::string Authentication) {
    return cpr::Put(cpr::Url{URL}, cpr::Body{Data}, cpr::Header{{ "Authorization", "Bearer " + Authentication }}).text;
}
std::string leet::invokeRequest_Post(const std::string URL, const std::string Data, const std::string Authentication) {
    return cpr::Post(cpr::Url{URL}, cpr::Body{Data}, cpr::Header{{ "Authorization", "Bearer " + Authentication }}).text;
}
