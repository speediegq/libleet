/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU Lesser General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

#pragma once
#ifndef LEET_NO_ENCRYPTION
#include <random>
#include <cstring>

namespace leetCrypto {
    /**
     * @brief Simply fills a buffer with random bytes
     */
    inline void fillWithRandomBytes(void* _data, size_t _length) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<unsigned char> dist(0, 255);

        unsigned char* ptr = static_cast<unsigned char*>(_data);
        for (size_t i = 0; i < _length; ++i) {
            ptr[i] = dist(gen);
        }
    }

    /**
     * @brief Class used to generate random bytes
     */
    class RandomBytes {
        private:
        public:
            void *_data;
            size_t _length;

            RandomBytes(size_t length): _data(malloc(length)), _length(length) {
                fillWithRandomBytes(_data, _length);
            }
            ~RandomBytes() {
                memset(_data, 0, _length);
                free(_data);
            }
            size_t length() const {
                return _length;
            }
            void* data() const {
                return _data;
            }
    };
}
#endif

/* The main namespace, most functions and variables will be contained in this. */
namespace leet {
    enum {
        /* Identifiers */
        LEET_IDENTIFIER_USERID,
        LEET_IDENTIFIER_THIRDPARTYID, // Unsupported for now
        LEET_IDENTIFIER_PHONE, // Unsupported for now

        /* Login types */
        LEET_TYPE_PASSWORD,
        LEET_TYPE_TOKEN,

        /* Presets */
        LEET_PRESET_TRUSTED_PRIVATE, // Everyone becomes an administrator, otherwise same as LEET_PRESET_PRIVATE
        LEET_PRESET_PRIVATE, // Invite from an existing member of the room required to join
        LEET_PRESET_PUBLIC, // Invite not required, users can just join (except guests)

        /* Presence */
        LEET_PRESENCE_OFFLINE,
        LEET_PRESENCE_ONLINE,
        LEET_PRESENCE_UNAVAILABLE,

        /* Message types */
        LEET_MESSAGETYPE_TEXT,
        LEET_MESSAGETYPE_IMAGE,
        LEET_MESSAGETYPE_AUDIO,
        LEET_MESSAGETYPE_VIDEO,
        LEET_MESSAGETYPE_FILE,
        LEET_MESSAGETYPE_NOTICE, // Not implemented
        LEET_MESSAGETYPE_EMOTE, // Not implemented
        LEET_MESSAGETYPE_LOCATION, // Not implemented
        LEET_MESSAGETYPE_STRING, // This means the user enters his own type

        /* Body types */
        LEET_BODYTYPE_BASIC,
        LEET_BODYTYPE_FORMATTED,
        LEET_BODYTYPE_BOTH,

        /* All the available errors */
        LEET_ERROR_NONE,
        LEET_ERROR_MSG_IS_NOT_AN_ENCRYPTED_MSG,
        LEET_ERROR_SESSION_FOR_MSG_WAS_NOT_RETRIEVED,
        LEET_ERROR_SESSION_IS_NOT_PROPERLY_ENCRYPTED,
        LEET_ERROR_FAILED_TO_CREATE_INBOUND_SESSION,
        LEET_ERROR_MESSAGE_HAS_NO_SENDER,
        LEET_ERROR_MESSAGE_HAS_WRONG_SENDER,
        LEET_ERROR_MESSAGE_HAS_WRONG_USERID,
        LEET_ERROR_NOT_OUR_MESSAGE,
        LEET_ERROR_WRONG_MESSAGE_TYPE,
        LEET_ERROR_WRONG_MESSAGE_ALGORITHM,
    };

    namespace User {
        /**
         * @brief Credentials class which contains fields passed to login requests.
         *
         * Credentials class which contains fields passed to login requests.
         * The client should set these fields in an object and pass that to
         * a login function.
         */
        class Credentials { /* Only applies for the login account */
            private:
            public:
                std::string Username{}; // Username
                std::string Password{}; // Password
                std::string displayName{}; // Client side display name
                std::string deviceID{}; // Unique ID
                std::string Homeserver{};
                bool refreshToken{false}; // Should we use a refresh token?
                std::string Token{}; // An actual token
                int Identifier{0}; // Identifier used to log in
                int Type{0}; // Type

                /**
                 * @brief  Function which clears all of the variables stored in the object to their default value.
                 */
                void clearCredentials() {
                    Username = "";
                    Password = "";
                    displayName = "";
                    deviceID = "";
                    Homeserver = "";
                    Token = "";
                    refreshToken = false;
                    Identifier = 0;
                    Type = 0;
                }
        }; /* https://playground.matrix.org/#post-/_matrix/client/v3/login */

        /**
         * @brief Class which contains the response from the Matrix homeserver on initial login.
         */
        class CredentialsResponse { /* Only applies for the login account */
            private:
            public:
                std::string accessToken{}; // Access token, which will be used to authenticate all future events for this session
                std::string refreshToken{}; // Refresh token, if one is returned and used
                std::string deviceID{}; // @<user>:<homeserver>
                std::string Homeserver{}; // Should correspond to leet::User::Credentials.Homeserver
                std::string userID{}; // Should correspond to leet::User::Credentials.User
                int Expiration{}; // When the access token expires.
        }; /* https://playground.matrix.org/#post-/_matrix/client/v3/login */

        /**
         * @brief Class which represents a single device for a user.
         */
        class Device {
            private:
            public:
                std::string userID{}; // His user ID
                std::string deviceID{}; // His device ID
                std::string curve25519Key{}; // His public curve25519 key
                std::string ed25519Key{}; // His public ed25519 key
                std::string ed25519Signature{}; // His public ed25519 signature
                std::string deviceDisplayName{}; // His device display name (not to be confused with device ID)
                bool olm{false}; // Whether olm is supported or not
                bool megolm{false}; // Whether megolm is supported or not
        }; /* https://spec.matrix.org/v1.1/client-server-api/#post_matrixclientv3keysquery */

        /**
         * @brief Class which represents a user profile, along with his or her devices.
         */
        class Profile {
            private:
            public:
                std::string userID{}; // His user ID
                std::string displayName{}; // His display name
                std::string avatarURL{}; // His avatar mxc:// URL
                std::vector<User::Device> Devices{}; // All of his devices
        }; /* https://spec.matrix.org/v1.8/client-server-api/#profiles */
    }

    namespace VOIP {
        /**
         * @brief  Class which represents TURN server credentials returned
         */
        class Credentials {
            private:
            public:
                std::string Username{}; // The TURN server username
                std::string Password{}; // The TURN server password
                int timeToLiveIn{}; // Go figure.
                std::vector<std::string> URI{};
        };
    }

    namespace Room {
        /**
         * @brief Class which represents a single room, along with its properties.
         */
        class Room {
            private:
            public:
                std::string roomID{}; // Room ID
                std::string roomType{}; // Room type

                std::string Name{}; // Room name
                std::string avatarURL{}; // Avatar mxc:// URL
                bool guestCanJoin{false}; // Whether guest accounts can join or not
                bool worldReadable{false}; // Matrix speak for whether the room is public or not
                std::string Alias{}; // Room alias (i.e. #speedie:matrix.org)
                std::string Topic{}; // Room topic
                std::string joinRule{}; // Join rule
                int memberCount{}; // Number of members in the room
        };
        /**
         * @brief  Class which represents settings for a single room that should be created.
         */
        class RoomConfiguration {
            private:
            public:
                bool Federate{false};
                bool directMessage{false};
                int Preset{LEET_PRESET_PRIVATE};
                std::string Name{};
                std::string Alias{};
                std::string Topic{};
        };
    }

    namespace Space {
        /**
         * @brief Class which represents a space, which is a collection of rooms.
         *
         * This class represents a space, which in
         * libleet is different from a room, for simplicity.
         * In reality, a space is almost identical to a room.
         *
         */
        class Space : public Room::Room {
            private:
            public:
                std::string spaceID{}; // Space ID
                std::string roomType{}; // Room type
                std::vector<leet::Room::Room> Rooms{}; // Rooms in the space
        };
    }

    namespace Attachment {
        /**
         * @brief  Class which represents an uploaded attachment.
         */
        class Attachment {
            private:
            public:
                std::string URL{};
        };
    }

    namespace URL {
        /**
         * @brief  Class which represents a URL preview.
         */
        class URLPreview {
            private:
            public:
                std::string URL{};
                int64_t Time{};

                int imageSize{};
                int imageWidth{};
                int imageHeight{};
                std::string imageURL{};
                std::string Title{};
                std::string Description{};
                std::string Type{};
        };
    }

    namespace Filter {
        /**
         * @brief Filter configuration, can be used to generate a filter which can be used to find an event ID by functions that make use of it.
         */
        class FilterConfiguration {
            private:
            public:
                std::vector<std::string> notSenders{}; // Senders to exclude
                std::vector<std::string> Senders{}; // Senders to include
                std::vector<std::string> notRooms{}; // Rooms to exclude
                std::vector<std::string> Rooms{}; // Rooms to include
                std::vector<std::string> Fields = { "type", "content", "sender" }; // Vector of fields
                int Limit{0}; // Max number of events to return
        };

        /**
         * @brief Filter, which can be used to find an event ID by functions that make use of it.
         */
        class Filter {
            private:
            public:
                std::string filterID{}; // Filter ID returned from endpoint
        };
    }

    namespace Sync {
        /**
         * @brief Class that represents a user event.
         */
        class UserEvents {
            private:
            public:
                std::string userID{};
                std::string avatarURL{};
                bool currentlyActive{false};
                int lastActiveAgo{};
                std::string Presence{};
                std::string statusMessage{};
        };
        /**
         * @brief Class that represents a name event.
         */
        class NameEvents {
            private:
            public:
                std::string roomID{};
                std::string roomName{};
                std::string Inviter{};
                std::string stateKey{};
        };
        /**
         * @brief Class that represents a room event.
         */
        class RoomEvents {
            private:
            public:
                /**
                 * @brief Class that represents an invite event.
                 */
                class InviteEvent {
                    private:
                    public:
                        std::string roomID{}; // room id to join
                        std::string userID{}; // inviter
                        std::string eventID{}; // id of the actual invite
                        std::string avatarURL{}; // avatar of the inviter
                        std::string displayName{}; // display name of the inviter
                        std::string Creator{}; // creator of the room
                        std::string joinRule{}; // join rule
                        std::string roomName{}; // room name
                        std::string roomTopic{}; // room topic
                        int roomVersion{}; // room version

                        bool Encrypted{false}; // encrypted bool
                };
                /**
                 * @brief Class that represents a join event.
                 */
                class JoinEvent {
                    private:
                    public:
                };
                /**
                 * @brief Class that represents a knock event.
                 */
                class KnockEvent {
                    private:
                    public:
                };
                /**
                 * @brief Class that represents a leave event.
                 */
                class LeaveEvent {
                    private:
                    public:
                };

                std::vector<InviteEvent> inviteEvents{};
        };
        /**
         * @brief Class that represents a Megolm session sent in the room */
        class MegolmSession {
            private:
            public:
                std::string Algorithm{};
                std::string cipherText{};
                int cipherType{};
                std::string senderKey{};
                std::string Sender{};
                std::string Type{};
        };
        /**
         * @brief Class containing objects of all the other classes in leet::Sync
         */
        class Sync {
            private:
            public:
                UserEvents userEvents{};
                NameEvents nameEvents{};
                RoomEvents roomEvents{};
                std::vector<MegolmSession> megolmSessions{};
                std::string nextBatch{};
                std::string theRequest{};
        };

        /**
         * @brief Class containing settings for a sync call
         */
        class SyncConfiguration {
            private:
            public:
                std::string Since{};
                leet::Filter::Filter Filter{};
                bool fullState{false};
                int Presence{LEET_PRESENCE_OFFLINE};
                int Timeout{30000};
        };
    }

    namespace Event {
        /**
         * @brief Class that represents a generic event not covered by another class.
         *
         * Class that represents a generic event not covered by another class. It doesn't do much
         * and only provides the event ID and time stamp so it can be us ed with invokeRequest for example.
         */
        class Event {
            private:
            public:
                std::string eventID{}; // Event ID
                std::string eventContent{}; // Event contents in JSON format
                int64_t Age{}; // Time since the event occured
        };

        /**
         * @brief Class which represents a single message, along with its properties.
         *
         * This class represents a single message
         * To represent several messages, one might consider using std::vector
         */
        class Message : public Event {
            private:
            public:
                std::string messageText{}; // Message text
                std::string formattedText{}; // Formatted message
                std::string Format{}; // Message format
                std::string Type{}; // Event type (i.e. m.room.message)
                std::string messageType{}; // Message type (i.e. m.image, m.audio, m.video, m.file, m.text)
                std::string Sender{}; // The sender user ID
                std::string mimeType{}; // Message mime type
                int attachmentWidth{}; // Attachment width
                int attachmentHeight{}; // Attachment height
                int attachmentSize{}; // Attachment file size
                int attachmentLength{}; // Mainly applies for videos and music
                std::string attachmentURL{}; // Attachment mxc:// URL
                int thumbnailWidth{}; // Thumbnail width
                int thumbnailHeight{}; // Thumbnail height
                int thumbnailSize{}; // Thumbnail file size
                std::string thumbnailMimeType{}; // Thumbnail mime type
                std::string thumbnailURL{}; // Thumbnail mxc:// URL
                std::vector<std::string> mentionedUserIDs{}; // Mentioned users
                leet::Event::Event replyEvent{}; // Event to reply to

                bool Encrypted{false}; // Whether the message is encrypted or not
                bool megolm{false};
                std::string cipherText{}; // Encrypted message text
                std::string senderKey{}; // Sender curve25519 key
                std::string sessionID{}; // Sender session ID
                std::string deviceID{}; // Sender device ID

                int bodyType{LEET_BODYTYPE_BASIC};
                int msgType{LEET_MESSAGETYPE_STRING};
        };
    }

    inline std::string Homeserver{"https://matrix.org"}; // Home server used to make API calls. This should be overridden once a home server has been determined.
    inline std::string Error{}; // Error code returned by the server (i.e. M_UNKNOWN)
    inline std::string friendlyError{}; // Human readable error code also returned by the server in most cases (i.e. Unknown error)
    inline int leetError{LEET_ERROR_NONE}; // libleet specific error
    inline int errorCode{0}; // Error code returned by libleet functions. If not set to 0, something went wrong.
    inline int transID{0}; // Transaction ID. Should be loaded/saved for each session, and incremented for each event
    inline int networkStatusCode{200}; // Status code returned by the last network request

    /**
     * @brief  Generate a new transaction ID by simply incrementing the existing ID by 1.
     * @return New transaction ID.
     *
     * This function generates a new transaction ID this session by simply
     * incrementing by 1.
     *
     * If your program finishes execution, you must preserve the value of
     * leet::transID until the next session and restore it (i.e. leet::transID = <old value>;)
     * before calling any functions that use a transaction ID. (such as sendMessage)
     *
     * If you use an ID that has been used before, the action will be considered a duplicate by
     * the server, and most likely ignored.
     */
    int generateTransID();
    /**
     * @brief  Saves the stored transaction ID to a file.
     * @param  File String path to the file.
     * @return Boolean, true if it was successfully saved, otherwise false is returned.
     *
     * Saves the stored transaction ID to a file. It should be saved each time the client is
     * restarted, and should then be loaded again using loadTransID. This is because the Matrix
     * server will ignore duplicate requests.
     *
     * NOTE: The transaction ID does not need to be encrypted, because it is not sensitive data.
     */
    bool saveTransID(const std::string& File);
    /**
     * @brief  Loads the stored transaction ID from a file.
     * @param  File String path to the file.
     * @return Boolean, true if it was successfully loaded, otherwise false is returned.
     *
     * Loads the stored transaction ID from a file, overriding the transID integer. This
     * should be done if a transaction ID file exists, and it should be done each time the
     * client is restarted provided a new session is NOT used, and a transaction ID has previously
     * been stored in a file.
     *
     * NOTE: The transaction ID does not need to be encrypted, because it is not sensitive data.
     */
    bool loadTransID(const std::string& File);
    /**
     * @brief  Sets the transaction ID to id
     * @param  id The transaction ID to use.
     * @return Boolean, true if it was successfully assigned, otherwise false is returned.
     */
    bool constexpr setTransID(int id);

    /**
     * @brief  Saves data to a file.
     * @param  File String path to the file.
     * @param  Data The data that should be saved to the file.
     */
    template <typename T> T saveToFile(const std::string& File, T Data);
    /**
     * @brief  Loads data from a file.
     * @param  File String path to the file.
     * @return Returns the data from the file.
     */
    template <typename T> T loadFromFile(const std::string& File);

    /**
     * @brief  Performs server discovery (.well-known/matrix/client) to find the proper Matrix home server URL.
     * @param  Server URL to check for discovery at.
     * @return Returns the home server if it can be found. If there's no .well-known/matrix/client file on the server, the server URL passed will be returned.
     */
    std::string returnServerDiscovery(const std::string& Server);
    /**
     * @brief  Returns a home server (matrix.org) from a user ID (@speedie:matrix.org)
     * @param  userID The user ID to extract the home server from.
     * @return Home server string
     */
    std::string returnHomeServerFromString(const std::string& userID);

    /**
     * @brief  Get login types supported by the Matrix home server.
     * @return Returns login types supported by the Matrix home server in the form of an std::vector<std::string>.
     */
    std::vector<std::string> returnSupportedLoginTypes();
    /**
     * @brief  Get supported Matrix specification versions.
     * @return Returns supported Matrix specification versions in the form of an std::vector<std::string>.
     */
    std::vector<std::string> returnSupportedSpecs();
    /**
     * @brief  Return max upload size limit
     * @param  CredentialsResponse object, used for authentication.
     * @return Max upload size limit in bytes.
     */
    int returnMaxUploadLimit(const User::CredentialsResponse& resp);

    /**
     * @brief  Check if an error occured.
     * @return Returns false if an error occured, otherwise returns true;
     */
    bool checkError();

    /**
     * @brief  Attempts to create an account on the home server.
     * @param  cred Credentials object to use for registering
     * @return CredentialsResponse object, containing the access token among other variables.
     */
    User::CredentialsResponse registerAccount(const User::Credentials& cred);
    /**
     * @brief  Check if a registration token is still valid or not.
     * @param  Token The token that should be validated.
     * @return Returns true if it's valid, otherwise returns false.
     */
    bool checkRegistrationTokenValidity(const std::string& Token);

    /**
     * @brief  Attempts to log in to the home server.
     * @param  cred Credentials object to use for login
     * @return CredentialsResponse object, containing the access token among other variables.
     */
    User::CredentialsResponse loginAccount(const User::Credentials& cred);
    /**
     * @brief  Refresh an old access token (if a refresh token is available in the object)
     * @param  resp CredentialsResponse object, required for authentication.
     * @return Returns a new User::CredentialsResponse object based on the parameter.
     */
    User::CredentialsResponse refreshAccessToken(User::CredentialsResponse& resp);
    /**
     * @brief  Invalidate an access token
     * @param  Token Token to invalidate.
     */
    void invalidateAccessToken(const std::string& Token);
    /**
     * @brief  Returns user data in the form of a User::Profile object.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  userID User to get data for.
     * @return Returns a User::Profile object containing the user data.
     */
    User::Profile getUserData(const User::CredentialsResponse& resp, const std::string& userID);

    /**
     * @brief  Returns a full API URL from a Matrix endpoint.
     * @param  API The Matrix endpoint to call.
     * @return The full API URL string.
     *
     * Returns a full API URL from a Matrix endpoint. This uses the leet::Homeserver variable and the passed API.
     * In other words, it returns https://example.com/_matrix/... from the passed API (_matrix/...)
     */
    std::string getAPI(const std::string& API);

    /**
     * @brief  Invokes a Get request
     * @param  URL URL to request.
     * @param  Authentication Authentication that should be used. Often this is the access token.
     * @return Returns the output from the request.
     */
    std::string invokeRequest_Get(const std::string& URL, const std::string& Authentication);
    /**
     * @brief  Invokes a Delete request
     * @param  URL URL to request.
     * @param  Authentication Authentication that should be used. Often this is the access token.
     * @return Returns the output from the request.
     */
    std::string invokeRequest_Delete(const std::string& URL, const std::string& Authentication);
    /**
     * @brief  Invokes a Put request
     * @param  URL URL to request.
     * @param  Data Data to Put.
     * @param  Authentication Authentication that should be used. Often this is the access token.
     * @return Returns the output from the request.
     */
    std::string invokeRequest_Put(const std::string& URL, const std::string& Data, const std::string& Authentication);
    /**
     * @brief  Invokes a Post request
     * @param  URL URL to request.
     * @param  Data Data to Post.
     * @param  Authentication Authentication that should be used. Often this is the access token.
     * @return Returns the output from the request.
     */
    std::string invokeRequest_Post(const std::string& URL, const std::string& Data, const std::string& Authentication);
    /**
     * @brief  Invokes a Get request
     * @param  URL URL to request.
     * @return Returns the output from the request.
     */
    std::string invokeRequest_Get(const std::string& URL);
    /**
     * @brief  Invokes a Delete request
     * @param  URL URL to request.
     * @return Returns the output from the request.
     */
    std::string invokeRequest_Delete(const std::string& URL);
    /**
     * @brief  Invokes a Put request
     * @param  URL URL to request.
     * @param  Data Data to Put.
     * @return Returns the output from the request.
     */
    std::string invokeRequest_Put(const std::string& URL, const std::string& Data);
    /**
     * @brief  Invokes a Post request
     * @param  URL URL to request.
     * @param  Data Data to Post.
     * @return Returns the output from the request.
     */
    std::string invokeRequest_Post(const std::string& URL, const std::string& Data);
    /**
     * @brief  Invokes a Post request
     * @param  URL URL to request.
     * @param  File Path to a file which should be uploaded.
     * @return Returns the output from the request.
     */
    std::string invokeRequest_Post_File(const std::string& URL, const std::string& File);
    /**
     * @brief  Invokes a Post request
     * @param  URL URL to request.
     * @param  File Path to a file which should be uploaded.
     * @param  Authentication Authentication that should be used. Often this is the access token.
     * @return Returns the output from the request.
     */
    std::string invokeRequest_Post_File(const std::string& URL, const std::string& File, const std::string& Authentication);

    /**
     * @brief  Gets information about a room based on a room ID.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  room Room object, this object should contain a room ID.
     * @return Returns a Room object containing room data.
     */
    Room::Room returnRoom(const User::CredentialsResponse& resp, const Room::Room& room);
    /**
     * @brief  Upgrades a room to a specific room version.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  room Room object, this object should contain a room ID.
     * @param  Version Version integer.
     * @return Returns a room with a new room ID.
     */
    Room::Room upgradeRoom(const User::CredentialsResponse& resp, const Room::Room& room, const int Version);
    /**
     * @brief  Create a room from a Room::RoomConfiguration object.
     * @param  resp CredentialsResponse object, required for authentication.
     * @parma  conf Room configuration object, this object should contain room information.
     * @return Returns a Room object containing room data.
     */
    Room::Room createRoom(const User::CredentialsResponse& resp, const Room::RoomConfiguration& conf);

    /**
     * @brief  Join a public room or a room that you have been invited to.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  room Room object, this object should contain a room ID.
     * @param  Reason Reason for wanting to join.
     */
    void joinRoom(const User::CredentialsResponse& resp, const Room::Room& room, const std::string& Reason);
    /**
     * @brief  Leave a room you're part of.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  room Room object, this object should contain a room ID.
     * @param  Reason Reason for leaving.
     */
    void leaveRoom(const User::CredentialsResponse& resp, const Room::Room& room, const std::string& Reason);
    /**
     * @brief  Kick a user from a room.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  room Room object, this object should contain a room ID.
     * @param  profile Profile object, this object should contain a user ID for the user to kick.
     * @param  Reason Reason for kicking the user.
     */
    void kickUserFromRoom(const User::CredentialsResponse& resp, const Room::Room& room, const User::Profile& profile, const std::string& Reason);
    /**
     * @brief  Ban a user from a room.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  room Room object, this object should contain a room ID.
     * @param  profile Profile object, this object should contain a user ID for the user to ban.
     * @param  Reason Reason for banning the user.
     */
    void banUserFromRoom(const User::CredentialsResponse& resp, const Room::Room& room, const User::Profile& profile, const std::string& Reason);
    /**
     * @brief  Unban a user from a room.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  room Room object, this object should contain a room ID.
     * @param  profile Profile object, this object should contain a user ID for the user to unban.
     * @param  Reason Reason for unbanning the user.
     */
    void unbanUserFromRoom(const User::CredentialsResponse& resp, const Room::Room& room, const User::Profile& profile, const std::string& Reason);
    /**
     * @brief  Invite a user to a room.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  room Room object, this object should contain a room ID.
     * @param  Reason Reason for the invite being sent.
     */
    void inviteUserToRoom(const User::CredentialsResponse& resp, const Room::Room& room, const std::string& Reason);
    /**
     * @brief  Gets the visibility of a room.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  room Room object, this object should contain a room ID.
     * @return Returns true if the room is visible, otherwise returns false.
     */
    bool getVisibilityOfRoom(const User::CredentialsResponse& resp, const Room::Room& room);
    /**
     * @brief  Sets the visibility of a room to Visibility
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  room Room object, this object should contain a room ID.
     * @param  Visibility Boolean, true means visible, false means hidden.
     */
    void setVisibilityOfRoom(const User::CredentialsResponse& resp, const Room::Room& room, const bool Visibility);
    /**
     * @brief  Returns a vector of all rooms your user has joined.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  Limit Max number of rooms to return.
     * @return Returns a vector of all rooms your user has joined.
     */
    std::vector<Room::Room> returnRooms(const User::CredentialsResponse& resp, const int Limit);
    /**
     * @brief  Returns room IDs for all the rooms your user has joined.
     * @param  resp CredentialsResponse object, required for authentication.
     * @return Returns room IDs for all the rooms your user has joined.
     */
    std::vector<Room::Room> returnRoomIDs(const User::CredentialsResponse& resp);
    /**
     * @brief  Returns room aliases from a room ID.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  roomID The room to get aliases from.
     * @return Returns an std::vector<std::string> containing room aliases.
     */
    std::vector<std::string> findRoomAliases(const User::CredentialsResponse& resp, const std::string& roomID);
    /**
     * @brief  Resolves a room alias and returns the room ID.
     * @param  Alias Room alias to find the room ID from.
     * @return Returns a room ID.
     */
    std::string findRoomID(const std::string& Alias);
    /**
     * @brief  Removes a room alias from an undefined room ID.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  Alias The room alias to remove. The server will determine the room ID from this alias.
     * @return Returns a boolean, true if the room alias was successfully removed, otherwise false.
     */
    bool removeRoomAlias(const User::CredentialsResponse& resp, const std::string& Alias);

    /**
     * @brief  Returns a vector of all spaces your user has joined.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  Limit Max number of spaces to return.
     * @return Returns a vector of all spaces, along with all child rooms your user has joined.
     */
    std::vector<Space::Space> returnSpaces(const User::CredentialsResponse& resp, const int Limit);
    /**
     * @brief  Returns a vector of all rooms in a space.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  spaceID Space ID to get rooms from.
     * @return Returns a vector of all rooms in the space.
     */
    const std::vector<Room::Room> returnRoomsInSpace(const User::CredentialsResponse& resp, const std::string& spaceID, const int Limit);

    /**
     * @brief  Converts an incomplete user ID to a full user ID. (i.e. speedie is converted to @speedie:matrix.org)
     * @param  Alias The alias to get the user ID from.
     * @param  Homeserver Home server to use.
     * @return Returns a full user ID. You should make sure the home server is actually correct and validate the return value of this function.
     */
    std::string findUserID(const std::string& Alias, const std::string& Homeserver);
    /**
     * @brief  Returns <name> from @<name>:<home server>.
     * @param  userID Full user ID.
     * @return Returns a string.
     */
    std::string returnUserName(const std::string& userID);
    /**
     * @brief  Returns a vector of all users in a room.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  room Room object, the function uses the room ID, which must be set.
     * @return Returns a vector of all users in a room.
     */
    std::vector<User::Profile> returnUsersInRoom(const User::CredentialsResponse& resp, const Room::Room& room);

    /**
     * @brief  Returns a vector of all devices for a specific user.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  user User vector, each User must have the user ID set.
     * @return Returns a vector of all devices for a specific user.
     */
    std::vector<User::Device> returnDevicesFromUser(const User::CredentialsResponse& resp, const std::vector<User::Profile>& user);

    /**
     * @brief  Returns a boolean for whether a username is available on the home server or not.
     * @param  Username String to check for.
     * @return Returns true if the username is available, returns false otherwise.
     */
    bool checkIfUsernameIsAvailable(const std::string& Username);

    /**
     * @brief  Toggles typing in a room.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  Timeout How long to type for.
     * @param  Typing Boolean, whether or not we should be typing or not.
     * @param  room Room to send the event in.
     */
    void toggleTyping(const User::CredentialsResponse& resp, const int Timeout, const bool Typing, const Room::Room& room);

    /**
     * @brief  Sets the read marker position to a specific event
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  room Room to set the read marker in.
     * @param  fullyReadEvent Event to place the marker at.
     * @param  readEvent Event to set the read receipt at.
     * @param  privateReadEvent Event to set the private read receipt at.
     */
    void setReadMarkerPosition(const User::CredentialsResponse& resp, const Room::Room& room,
        const Event::Event& fullyReadEvent, const Event::Event& readEvent, const Event::Event& privateReadEvent);

    /**
     * @brief  Sends a plain text message to the specified room.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  room Room object, room that the message should be sent in.
     * @param  msg Event::Message object, the message that should be sent.
     */
    void sendMessage(const User::CredentialsResponse& resp, const Room::Room& room, const Event::Message& msg);
    /**
     * @brief  Returns a Event::Message vector from a room.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  room Room object, room that the messages should be retrieved from.
     * @param  messageCount Number of messages to retrieve from the room.
     * @return Returns a Event::Message vector which represents the retrieved messages.
     */
    std::vector<Event::Message> returnMessages(const User::CredentialsResponse& resp, const Room::Room& room, const int messageCount);

    /**
     * @brief  Returns a filter ID which can be used when requesting data.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  filter Filter configuration.
     * @return Returns a filter ID which can be used when requesting data.
     */
    Filter::Filter returnFilter(const User::CredentialsResponse& resp, const Filter::FilterConfiguration& filter);

    /**
     * @brief  Uploads a file to the Matrix server(s).
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  File Path to the file which should be uploaded.
     * @return Returns an Attachment::Attachment object containing an mxc:// Matrix URL.
     */
    Attachment::Attachment uploadFile(const User::CredentialsResponse& resp, const std::string& File);
    /**
     * @brief  Downloads a file from the Matrix server.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  File Attachment object containing an mxc:// URL to download from.
     * @param  outputFile Output file path.
     * @return Returns true if it was downloaded successfully, otherwise false is returned.
     */
    bool downloadFile(const User::CredentialsResponse& resp, const Attachment::Attachment& Attachment, const std::string& outputFile);

    /**
     * @brief  Get a URL preview by calling a Matrix media endpoint. Do not call in encrypted rooms.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  URL The URL to get a preview for.
     * @param  time The point in time to get a preview for. Specify 0 if you want the latest preview.
     * @return Returns a URLPreview object containing the response data.
     */
    URL::URLPreview getURLPreview(const User::CredentialsResponse& resp, const std::string& URL, const int64_t time);
    /**
     * @brief  Gets a download link for an attachment from the Matrix server.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  File Attachment object containing an mxc:// URL to download from.
     * @return Returns a URL in the form of an std::string
     */
    std::string decodeFile(const User::CredentialsResponse& resp, const Attachment::Attachment& Attachment);

    /**
     * @brief  Gets the Unix timestamp
     * @return Returns a Unix timestamp (i.e. time since epoch)
     */
    int64_t returnUnixTimestamp();
    /**
     * @brief  Get an event from a Unix timestamp.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  room Room object to get event(s) from.
     * @param  Timestamp Unix timestamp for the event.
     * @param  Direction Boolean which specifies which direction to search for events in. true means forward while false means backwards.
     * @return Returns the event. If none could be found, an empty object is simply returned instead.
     */
    Event::Event returnEventFromTimestamp(const User::CredentialsResponse& resp, const Room::Room& room, const int64_t Timestamp, const bool Direction);
    /**
     * @brief  Gets the latest event sent in the room.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  room Room object to get an event from.
     *
     * Note that this function is just a convenient wrapper for returnEventFromTimestamp()
     */
    Event::Event returnLatestEvent(const User::CredentialsResponse& resp, const Room::Room& room);
    /**
     * @brief  Get a state from a room ID, type and state key
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  room Room object to get an event from.
     * @param  eventType Event type to get.
     * @param  stateKey State key, most of the time you can just specify an empty string.
     * @return Returns an Event object.
     */
    Event::Event getStateFromType(const User::CredentialsResponse& resp, const Room::Room& room, const std::string& eventType, const std::string& stateKey);
    /**
     * @brief  Set a state from a room ID, type and state key. If a state already exists, it will be replaced.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  room Room object to get an event from.
     * @param  eventType Event type to get.
     * @param  stateKey State key, most of the time you can just specify an empty string.
     * @param  Body The body to include in the state.
     * @return Returns an Event object.
     */
    Event::Event setStateFromType(const User::CredentialsResponse& resp, const Room::Room& room, const std::string& eventType, const std::string& stateKey, const std::string& Body);

    /**
     * @brief  Redact an event sent in a room.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  room Room object to get an event from.
     * @param  event The event to redact.
     * @param  Reason Reason for the redaction.
     */
    void redactEvent(const User::CredentialsResponse& resp, const Room::Room& room, const Event::Event& event, const std::string& Reason);
    /**
     * @brief  Report an event to the home server owner.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  room Room the event can be found in.
     * @param  event The event to report.
     * @param  Reason Reason for the report being made.
     * @param  Score A score between -100 and 0 with -100 being the most offensive and 0 is inoffensive. If the integer is not in this range 0 will be used.
     */
    void reportEvent(const User::CredentialsResponse& resp, const Room::Room& room, const Event::Event& event, const std::string& Reason, const int Score);

    /**
     * @brief  Get sync information from the server.
     * @param  resp CredentialsResponse object, required for authentication.
     * @return Returns a Sync object with the fields.
     */
    Sync::Sync returnSync(const User::CredentialsResponse& resp, const Sync::SyncConfiguration& conf);

    /**
     * @brief  Get TURN server credentials
     * @param  resp CredentialsResponse object, required for authentication.
     * @return Returns a VOIP::Credentials object.
     */
    VOIP::Credentials returnTurnCredentials(const User::CredentialsResponse& resp);

    #ifndef LEET_NO_ENCRYPTION
    /**
     * @brief  Class which represents everything necessary to support encryption for a Matrix account
     */
    class olmAccount {
        private:
        public:
            void* accountMemory;
            void* utilityMemory;
            void* megolmSessionMemory;
            char* megolmSessionID;
            std::size_t megolmSessionIDLength;
            char* megolmSessionKey;
            std::size_t megolmSessionKeyLength;
            char* Identity;
            std::size_t identityLength;
            char* Signature;
            std::size_t signatureLength;
            int keysRemaining{0};

            std::string curve25519{};
            std::string ed25519{};

            /* These booleans essentially exist to prevent consequences from occuring in case
            * the caller tries to free or allocate twice.
            */
            bool accountMemoryAllocated{false};
            bool identityMemoryAllocated{false};
            bool signatureMemoryAllocated{false};
            bool olmSessionMemoryAllocated{false};
            bool megolmSessionMemoryAllocated{false};
            bool megolmSessionIDMemoryAllocated{false};
            bool utilityMemoryAllocated{false};

            /**
            * @brief  Creates an account.
            *
            * Creates an account, to be uploaded.
            * In basic test clients, or clients that will only run once or run constantly it is fine to call this
            * each time the program is restarted, but for proper clients you should call this function once, pickle
            * the data, store the key and data and unpickle the data using unpickle() or the convenient loadAccount()
            * function.
            */
            void createAccount();

            /**
            * @brief  Loads an account from pickle data using the pickle key.
            * @param  pickleKey Pickle key to use when unpickling the data. This must be stored by the client.
            * @param  pickleData Pickle data to unpickle with the key. This must be stored by the client.
            *
            * Loads an account from pickle data and a pickle key.
            * The pickle key should be stored somewhere, preferably encrypted for security reasons.
            * Your client should (must) store the pickle data as well. This function does not check
            * if the data is valid, so you should make sure it is. A runtime error will occur otherwise.
            *
            * Don't forget to free accountMemory
            */
            void loadAccount(const std::string& pickleKey, const std::string& pickleData);

            /**
            * @brief  Creates a Megolm session.
            *
            * Creates a megolm session.
            */
            void createMegolmSession();

            /**
            * @brief  Loads a Megolm session from pickle data
            * @param  pickleKey Pickle key to use when unpickling the data. This must be stored by the client.
            * @param  pickleData Pickle data to unpickle with the key. This must be stored by the client.
            *
            * Loads a megolm session from pickle data
            */
            void loadMegolmSession(const std::string& pickleKey, const std::string& pickleData);

            /**
            * @brief  Creates an identity.
            */
            void createIdentity();

            /**
            * @brief Uploads the device keys
            * @param  resp CredentialsResponse object, required for authentication.
            */
            void upload(const leet::User::CredentialsResponse& resp);

            /**
            * @brief Creates a session with one or more users in the current room.
            * @param  resp CredentialsResponse object, required for authentication.
            * @param  room The room to send and create the event in.
            * @param  users Vector of users to create a session with.
            */
            void createSession(const leet::User::CredentialsResponse& resp, const leet::Room::Room& room, const std::vector<leet::User::Profile>& users);

            /**
            * @brief  Encrypt a message.
            * @param  resp CredentialsResponse object, required for authentication.
            * @param  message Message to encrypt.
            * @return Returns a json string containing the encrypted cipher text.
            *
            * Returns string json containing encrypted cipher text, which can then be uploaded to the server as an event
            */
            std::string encryptMessage(const leet::User::CredentialsResponse& resp, const std::string& message);

            /**
            * @brief  Clean up by clearing the Olm account.
            */
            void clear();

            /**
            * @brief  Free memory that has been allocated.
            */
            void destroy();
    };

    /**
     * @brief  Class which hopefully allows you to safely handle encryption and Olm memory.
     */
    class Encryption {
        private:
        public:
            leet::olmAccount account;
            bool hasUploadedKeys{false};
            bool hasCreatedAccount{false};
            bool Cleaned = false;

            /**
             * @brief  Function that frees all allocated Olm data.
             */
            void destroy();
    };

    /**
     * @brief  Function that initializes an Encryption object properly.
     * @return Returns an Encryption object.
     */
    Encryption initEncryption();
    /**
     * @brief  Function that initializes an Encryption object from a pickle key and pickle data.
     * @param  pickleKey Pickle key to use when unpickling the data.
     * @param  pickleData Pickle data to unpickle.
     * @return Returns an Encryption object.
     */
    Encryption initEncryptionFromPickle(const std::string& pickleKey, const std::string& pickleData);
    /**
     * @brief  Function that uploads encryption keys for your account to the Matrix home server.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  enc Encryption object.
     * @return Returns an Encryption object.
     */
    Encryption uploadKeys(const User::CredentialsResponse& resp, Encryption& enc);
    /**
     * @brief  Creates a session in a room.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  enc Encryption object.
     * @param  room Room to create a session in.
     * @return Returns an Encryption object.
     */
    Encryption createSessionInRoom(const User::CredentialsResponse& resp, Encryption& enc, const Room::Room& room);
    /**
     * @brief  Sends an encrypted message to a room.
     * @param  resp CredentialsResponse object, required for authentication.
     * @param  enc Encryption object.
     * @param  room Room to create a session in.
     * @param  msg Message to send.
     */
    void sendEncryptedMessage(const User::CredentialsResponse& resp, Encryption& enc, const Room::Room& room, const Event::Message& msg);

    #endif // #ifndef LEET_NO_ENCRYPTION
}
