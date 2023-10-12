/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

#include <vector>

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
        class Credentials { /* Only applies for the login account */
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

        class CredentialsResponse { /* Only applies for the login account */
            private:
            public:
                std::string AccessToken;
                std::string RefreshToken;
                std::string DeviceID; // @<user>:<homeserver>
                std::string Homeserver; // Should correspond to leet::MatrixOptions::Homeserver
                std::string UserID; // Should correspond to leet::User::Credentials.User
                int Expiration; // Probably unused, at least for now
        }; /* https://playground.matrix.org/#post-/_matrix/client/v3/login */

        class Profile { // Ideally, you should have a two classes for the login user
            private:
            public:
                std::string UserID;
                std::string DisplayName;
                std::string AvatarURL;
        }; /* https://spec.matrix.org/v1.8/client-server-api/#profiles */
    }

    namespace Room {
        class Room {
            private:
            public:
                std::string RoomID;
        };
    }

    class MatrixOptions {
        private:
        public:
            std::string Homeserver;
            int Type;

            /* The login user */
            User::Credentials Credentials;
            User::CredentialsResponse CredentialsResponse;
            User::Profile Profile;

            Room::Room activeRoom;
    };

    MatrixOptions MatrixOption;

    std::string Error;
    std::string friendlyError;
    int errorCode = 0;
    int TransID = 0;

    std::string defaultHomeserver = "matrix.org";

    void setSettings(MatrixOptions *);
    void saveCredentials(User::Credentials *);
    void clearUserCredentials();

    /* This function generates a new transaction ID this session by simply
     * incrementing by 1.
     *
     * If your program finishes execution, you must preserve the value of
     * leet::TransID until the next session and restore it (i.e. leet::TransID = <old value>;)
     * before calling any functions that use a transaction ID. (such as sendSimpleMessage)
     *
     * If you use an ID that has been used before, the action will be considered a duplicate by
     * the server, and most likely ignored.
     */
    int generateTransID();

    User::CredentialsResponse connectHomeserver();
    User::Profile getUserData(const std::string UserID);
    std::string getAPI(const std::string api);
    std::string invoke(const std::string URL, const std::string Data);

    std::vector<std::string> returnRooms();
    std::string findRoomID(std::string Alias);
    void setRoom(const std::string Room);

    std::string findUserID(const std::string Alias);

    void sendSimpleMessage(User::CredentialsResponse *, const std::string Message);
}

#endif
