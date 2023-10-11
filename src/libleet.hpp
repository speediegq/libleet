/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

#ifndef LIBLEET_HPP
#define LIBLEET_HPP

/* Identifier */
enum {
    LUserID,
    LThirdPartyID, /* Unsupported for now */
    LPhone, /* Unsupported for now */
};

/* Type */
enum {
    TPassword,
    TToken,
};

/* The main namespace, most functions and variables will be contained in this. */
namespace leet {
    namespace User {
        class Credentials {
            private:
            public:
                std::string Username; // Username
                std::string Password; // Password
                std::string DisplayName; // Client side display name
                std::string DeviceID; // Unique ID
                int Identifier; // Identifier used to log in, LUserID is most likely used.
                bool RefreshToken;
                std::string Token;
                int Type;
        }; /* https://playground.matrix.org/#post-/_matrix/client/v3/login */

        class CredentialsResponse {
            private:
            public:
                std::string AccessToken;
                std::string RefreshToken;
                std::string DeviceID; // @<user>:<homeserver>
                std::string Homeserver; // Should correspond to leet::MatrixOptions::Homeserver
                std::string UserID; // Should correspond to leet::User::Credentials.User
                int Expiration;
                std::string Error;
        }; /* https://playground.matrix.org/#post-/_matrix/client/v3/login */
    }

    class MatrixOptions {
        private:
        public:
            std::string Homeserver;
            int Type;
            User::Credentials Credentials; // Passed to server
            User::CredentialsResponse CredentialsResponse; // Response from server
    };

    MatrixOptions MatrixOption;

    int errorCode;

    void setSettings(MatrixOptions *);
    void saveCredentials(User::Credentials *);
    void clearUserCredentials();
    User::CredentialsResponse connectHomeserver();
    std::string getAPI(std::string api);
    std::string invoke(std::string URL, std::string Data);
}

#endif
