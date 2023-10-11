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

        class ServerResponse {
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
    };

    User::Credentials Credentials;
    MatrixOptions MatrixOption;
    User::ServerResponse ServerResponse;
    Room::Room activeRoom;

    int errorCode = 0;
    int TransID = 0;

    std::string reportError();
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

    void connectHomeserver();
    std::string getAPI(const std::string api);
    std::string invoke(const std::string URL, const std::string Data);

    std::vector<std::string> returnRooms();
    std::string findRoomID(const std::string roomAlias);
    void setRoom(const std::string Room);

    void sendSimpleMessage(const std::string Message);
}

#endif
