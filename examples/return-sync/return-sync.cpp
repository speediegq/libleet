#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include <libleet/libleet.hpp>

int main() {
    leet::User::Credentials cred;

    cred.Identifier = leet::LEET_IDENTIFIER_USERID;
    cred.Type = leet::LEET_TYPE_PASSWORD;

    std::cout << "\033[2J\033[1;1H";
    std::cout << "Enter a Matrix username (@<username>:<home server>)\n> ";
    std::getline(std::cin, cred.Username);

    std::cout << "\033[2J\033[1;1H";
    std::cout << "Enter a Matrix password\n> ";
    std::getline(std::cin, cred.Password);

    cred.deviceID = "libleet test client";
    cred.Homeserver = leet::returnServerDiscovery(leet::returnHomeServerFromString(cred.Username));

    leet::User::credentialsResponse resp;

    resp = leet::loginAccount(cred);

    cred.clearCredentials();

    leet::transID = leet::returnUnixTimestamp();

    leet::Sync::SyncConfiguration conf;
    leet::Sync::Sync sync = leet::returnSync(resp, conf);

    nlohmann::json obj = nlohmann::json::parse(sync.theRequest);

    std::ofstream file("output.json");

    file << obj.dump(2);

    file.close();
}
