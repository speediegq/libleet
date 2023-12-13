/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU Affero General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

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
