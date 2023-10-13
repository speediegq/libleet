/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU General Public License version 3.
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
