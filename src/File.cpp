/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU Affero General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

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
