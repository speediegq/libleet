/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

#include <string>
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
                std::string roomType;
                std::string Name;
                std::string AvatarURL;
                bool guestCanJoin;
                bool worldReadable;
                std::string Alias;
                std::string Topic;
                std::string joinRule;
                int memberCount;
        };
    }

    namespace Space {
        class Space {
            private:
            public:
                std::string SpaceID;
                std::string roomType;
                std::string Name;
                std::vector<Room::Room> Rooms;
                std::string AvatarURL;
                std::string joinRule;
                bool guestCanJoin;
                bool worldReadable;
                std::string Alias;
                std::string Topic;
                int memberCount;
        };
    }

    namespace Message {
        class Message {
            private:
            public:
                std::string messageText;
                std::string formattedText;
                std::string Format;
                std::string Type;
                std::string messageType;
                std::string eventID;
                std::string Sender;
                std::string mimeType;
                int32_t Age;
                int attachmentWidth;
                int attachmentHeight;
                int attachmentSize;
                int thumbnailWidth;
                int thumbnailHeight;
                int thumbnailSize;
                int attachmentLength; /* mainly applies for videos and music */
                std::string thumbnailMimeType;
                std::string thumbnailURL;
                std::string attachmentURL;
        };
    }

    namespace Filter {
        class Filter {
            private:
            public:
                std::vector<std::string> notSenders = {};
                std::vector<std::string> Senders = {};
                std::vector<std::string> notRooms = {};
                std::vector<std::string> Rooms = {};
                std::vector<std::string> Fields = { "type", "content", "sender" };
                int Limit = 0;
        };
    }

    namespace Event {
        class Event {
            private:
            public:
                std::string eventID;
                int32_t Age;
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
     * before calling any functions that use a transaction ID. (such as sendMessage)
     *
     * If you use an ID that has been used before, the action will be considered a duplicate by
     * the server, and most likely ignored.
     */
    int generateTransID();
    bool saveTransID(const std::string File);
    bool loadTransID(const std::string File);

    std::string returnServerDiscovery(std::string Server);
    std::string returnHomeServerFromString(std::string UserID);

    User::CredentialsResponse connectHomeserver();
    User::Profile getUserData(const std::string UserID);
    std::string getAPI(const std::string api);

    std::string invokeRequest_Get(const std::string URL, const std::string Authentication);
    std::string invokeRequest_Put(const std::string URL, const std::string Data, const std::string Authentication);
    std::string invokeRequest_Post(const std::string URL, const std::string Data, const std::string Authentication);
    std::string invokeRequest_Get(const std::string URL);
    std::string invokeRequest_Put(const std::string URL, const std::string Data);
    std::string invokeRequest_Post(const std::string URL, const std::string Data);
    std::string invokeRequest_Post_File(const std::string URL, const std::string File);
    std::string invokeRequest_Post_File(const std::string URL, const std::string File, const std::string Authentication);

    std::vector<Room::Room> returnRooms(User::CredentialsResponse *resp, const int Limit);
    std::vector<Room::Room> returnRoomIDs(User::CredentialsResponse *resp);
    std::string findRoomID(std::string Alias);
    void setRoom(Room::Room *room);

    std::vector<Space::Space> returnSpaces(User::CredentialsResponse *resp, const int Limit);
    std::vector<Room::Room> returnRoomsInSpace(User::CredentialsResponse *resp, const std::string SpaceID, const int Limit);

    std::string findUserID(const std::string Alias);
    std::vector<User::Profile> returnUsersInRoom(User::CredentialsResponse *resp, const std::string RoomID);
    std::vector<User::Profile> returnUsersInRoom(User::CredentialsResponse *resp);

    bool checkIfUsernameIsAvailable(const std::string Username);
    void toggleTyping(User::CredentialsResponse *resp, const int Timeout, const bool Typing, const std::string UserID, const std::string RoomID);

    void sendMessage(User::CredentialsResponse *, Message::Message *);
    std::vector<Message::Message> returnMessages(User::CredentialsResponse *resp, const int messageCount);

    std::string returnFilter(User::CredentialsResponse *resp, Filter::Filter *filter);

    std::string uploadFile(User::CredentialsResponse *resp, const std::string File);
    bool downloadFile(User::CredentialsResponse *resp, const std::string File, const std::string outputFile);

    int32_t returnUnixTimestamp();
    Event::Event returnEventFromTimestamp(User::CredentialsResponse *resp, const std::string RoomID, const int32_t Timestamp, const bool Direction);
    Event::Event returnLatestEvent(User::CredentialsResponse *resp, const std::string RoomID);
}

#endif
