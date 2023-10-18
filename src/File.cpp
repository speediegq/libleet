/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU Affero General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

bool leet::saveTransID(const std::string File) {
    std::filesystem::path file{ File };

    if (!std::filesystem::create_directories(file.parent_path()) && !std::filesystem::is_directory(file.parent_path())) {
        return false;
    }

    std::ofstream outputFile;

    outputFile.open(File);
    outputFile << leet::TransID;
    outputFile.close();

    return true;
}

bool leet::loadTransID(const std::string File) {
    std::filesystem::path file{ File };
    std::string line;
    if (std::filesystem::exists(file)) {
        std::ifstream inputFile(File);

        if (!inputFile.is_open()) {
            return false;
        }

        while (std::getline(inputFile, line)) {
            leet::TransID = std::atoi(line.c_str());
            inputFile.close();
            return true;
        }

        inputFile.close();
    }

    return false;
}

std::string leet::uploadFile(leet::User::CredentialsResponse *resp, const std::string File) {
    using json = nlohmann::json;
    const std::string Output = leet::invokeRequest_Post_File(leet::getAPI("/_matrix/media/v3/upload"), File, resp->accessToken);
    json returnOutput = { json::parse(Output) };

    for (auto &output : returnOutput) {
        leet::errorCode = 0;
        leet::Error = "";

        if (output["content_uri"].is_string())
            return output["content_uri"].get<std::string>();

        if (output["errcode"].is_string()) {
            leet::errorCode = 1;
            leet::Error = output["errcode"].get<std::string>();
            if (output["error"].is_string()) leet::friendlyError = output["error"].get<std::string>();
            return "";
        }
    }

    return "";
}

bool leet::downloadFile(leet::User::CredentialsResponse *resp, const std::string File, const std::string outputFile) {
    std::string Server{};
    std::string ID{};
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

    // Download the file
    std::ofstream of(outputFile, std::ios::binary);
    cpr::Response response = cpr::Download(of, cpr::Url{API});
    if (response.status_code == 200) {
        return true;
    }

    return false;
}
