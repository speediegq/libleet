/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU Affero General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

#ifndef LIBLEET_HPP
#define LIBLEET_HPP
#include <iostream>
#include <string>
#include <cstring>
#include <ctime>
#include <vector>
#include <exception>
#include <random>
#include <map>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#ifndef LEET_NO_ENCRYPTION
#include <olm/error.h>
#include <olm/olm.h>
#include <olm/sas.h>
#endif

/* Identifier */
enum {
    LEET_IDENTIFIER_USERID,
    LEET_IDENTIFIER_THIRDPARTYID, /* Unsupported for now */
    LEET_IDENTIFIER_PHONE, /* Unsupported for now */
};

/* Type */
enum {
    LEET_TYPE_PASSWORD,
    LEET_TYPE_TOKEN,
};

/* Preset */
enum {
    LEET_PRESET_TRUSTED_PRIVATE, // Everyone becomes an administrator, otherwise same as LEET_PRESET_PRIVATE
    LEET_PRESET_PRIVATE, // Invite from an existing member of the room required to join
    LEET_PRESET_PUBLIC, // Invite not required, users can just join (except guests)
};

/* Errors */
enum {
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

#ifndef LEET_NO_ENCRYPTION
namespace leetCrypto {
    /**
     * @brief Simply fills a buffer with random bytes
     * @return void
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
    class randomBytes {
        private:
        public:
            void *_data;
            size_t _length;

            randomBytes(size_t length): _data(malloc(length)), _length(length) {
              fillWithRandomBytes(_data, _length);
            }
            ~randomBytes() {
              memset(_data, 0, _length);
              free(_data);
            }
            size_t length() const {
              return _length;
            }
            void *data() const {
              return _data;
            }
    };
}
#endif

/* The main namespace, most functions and variables will be contained in this. */
namespace leet {
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
                std::string Username; // Username
                std::string Password; // Password
                std::string displayName; // Client side display name
                std::string deviceID; // Unique ID
                std::string Homeserver;
                bool refreshToken; // Should we use a refresh token?
                std::string Token; // An actual token
                int Identifier; // Identifier used to log in
                int Type; // Type

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
        class credentialsResponse { /* Only applies for the login account */
            private:
            public:
                std::string accessToken; // Access token, which will be used to authenticate all future events for this session
                std::string refreshToken; // Refresh token, if one is returned and used
                std::string deviceID; // @<user>:<homeserver>
                std::string Homeserver; // Should correspond to leet::User::Credentials.Homeserver
                std::string userID; // Should correspond to leet::User::Credentials.User
                int Expiration; // When the access token expires.
        }; /* https://playground.matrix.org/#post-/_matrix/client/v3/login */

        /**
         * @brief Class which represents a single device for a user.
         */
        class Device {
            private:
            public:
                std::string userID; // His user ID
                std::string deviceID; // His device ID
                std::string curve25519Key; // His public curve25519 key
                std::string ed25519Key; // His public ed25519 key
                std::string ed25519Signature; // His public ed25519 signature
                std::string deviceDisplayName; // His device display name (not to be confused with device ID)
                bool olm; // Whether olm is supported or not
                bool megolm; // Whether megolm is supported or not
        }; /* https://spec.matrix.org/v1.1/client-server-api/#post_matrixclientv3keysquery */

        /**
         * @brief Class which represents a user profile, along with his or her devices.
         */
        class Profile {
            private:
            public:
                std::string userID; // His user ID
                std::string displayName; // His display name
                std::string avatarURL; // His avatar mxc:// URL
                std::vector<User::Device> Devices; // All of his devices
        }; /* https://spec.matrix.org/v1.8/client-server-api/#profiles */
    }

    namespace VOIP {
        /**
         * @brief  Class which represents TURN server credentials returned
         */
        class Credentials {
            private:
            public:
                std::string Username; // The TURN server username
                std::string Password; // The TURN server password
                int timeToLiveIn;
                std::vector<std::string> URI;
        };
    }

    namespace Room {
        /**
         * @brief Class which represents a single room, along with its properties.
         */
        class Room {
            private:
            public:
                std::string roomID; // Room ID
                std::string roomType; // Room type
                std::string Name; // Room name
                std::string avatarURL; // Avatar mxc:// URL
                bool guestCanJoin; // Whether guest accounts can join or not
                bool worldReadable; // Matrix speak for whether the room is public or not
                std::string Alias; // Room alias (i.e. #speedie:matrix.org)
                std::string Topic; // Room topic
                std::string joinRule; // Join rule
                int memberCount; // Number of members in the room
        };
        /**
         * @brief  Class which represents settings for a single room that should be created.
         */
        class roomConfiguration {
            private:
            public:
                bool Federate{false};
                bool directMessage{false};
                int Preset{LEET_PRESET_PRIVATE};
                std::string Name;
                std::string Alias;
                std::string Topic;
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
         * TODO: Maybe we should inheret from rooms?
         */
        class Space {
            private:
            public:
                std::string spaceID; // Space ID
                std::string roomType; // Room type
                std::string Name; // Space name
                std::string avatarURL; // Avatar mxc:// URL
                bool guestCanJoin; // Whether guest accounts can join or not
                bool worldReadable; // Matrix speak for whether the space is public or not
                std::string Alias; // Space room alias (i.e. #speediegq:matrix.org)
                std::string Topic; // Space room topic
                std::string joinRule; // Join rule
                int memberCount; // Number of members in the space
                std::vector<Room::Room> Rooms; // Rooms in the space
        };
    }

    namespace Message {
        /**
         * @brief Class which represents a single message, along with its properties.
         *
         * This class represents a single message
         * To represent several messages, one might consider using std::vector
         */
        class Message {
            private:
            public:
                std::string messageText; // Message text
                std::string formattedText; // Formatted message
                std::string Format; // Message format
                std::string Type; // Event type (i.e. m.room.message)
                std::string messageType; // Message type (i.e. m.image, m.audio, m.video, m.file, m.text)
                std::string eventID; // Event ID
                std::string Sender; // The sender user ID
                std::string mimeType; // Message mime type
                int32_t Age; // Time since the message was posted
                int attachmentWidth; // Attachment width
                int attachmentHeight; // Attachment height
                int attachmentSize; // Attachment file size
                int attachmentLength; // Mainly applies for videos and music
                std::string attachmentURL; // Attachment mxc:// URL
                int thumbnailWidth; // Thumbnail width
                int thumbnailHeight; // Thumbnail height
                int thumbnailSize; // Thumbnail file size
                std::string thumbnailMimeType; // Thumbnail mime type
                std::string thumbnailURL; // Thumbnail mxc:// URL

                bool Encrypted; // Whether the message is encrypted or not
                bool megolm;
                std::string cipherText; // Encrypted message text
                std::string senderKey; // Sender curve25519 key
                std::string sessionID; // Sender session ID
                std::string deviceID; // Sender device ID
        };
    }

    namespace Attachment {
        /**
         * @brief  Class which represents an uploaded attachment.
         */
        class Attachment {
            private:
            public:
                std::string URL;
        };
    }

    namespace Filter {
        /**
         * @brief Filter, can be used to find an event ID by functions that make use of it.
         */
        class Filter {
            private:
            public:
                std::vector<std::string> notSenders; // Senders to exclude
                std::vector<std::string> Senders; // Senders to include
                std::vector<std::string> notRooms; // Rooms to exclude
                std::vector<std::string> Rooms; // Rooms to include
                std::vector<std::string> Fields = { "type", "content", "sender" }; // Vector of fields
                int Limit = 0; // Max number of events to return
        };
    }

    namespace Sync {
        /**
         * @brief Class that represents a user event.
         */
        class userEvent {
            private:
            public:
                std::string userID;
                std::string avatarURL;
                bool currentlyActive;
                int lastActiveAgo;
                std::string Presence;
                std::string statusMessage;
        };
        /**
         * @brief Class that represents a name event.
         */
        class nameEvent {
            private:
            public:
                std::string roomID;
                std::string roomName;
                std::string Inviter;
                std::string stateKey;
        };
        /**
         * @brief Class that represents an invite event.
         */
        class inviteEvent {
            private:
            public:
                std::string Membership;
                std::string Inviter;
                std::string invitedUser;
        };
        /**
         * @brief Class that represents a Megolm session sent in the room */
        class megolmSession {
            private:
            public:
                std::string Algorithm;
                std::string cipherText;
                int cipherType;
                std::string senderKey;
                std::string Sender;
                std::string Type;
        };
        /**
         * @brief Class containing objects of all the other classes in leet::Sync
         */
        class Sync {
            private:
            public:
                std::vector<userEvent> userEvents;
                std::vector<nameEvent> nameEvents;
                std::vector<inviteEvent> inviteEvents;
                std::vector<megolmSession> megolmSessions;
                std::string nextBatch;
                nlohmann::json theRequest; // The full request in nlohmann::json format, because this class is not complete
        };
    }

    namespace Event {
        /**
         * @brief Class that represents a generic event not covered by another class.
         *
         * Class that represents a generic event not covered by another class. It doesn't do much
         * and only provides the event ID and time stamp so it can be used with invokeRequest for example.
         */
        class Event {
            private:
            public:
                std::string eventID; // Event ID (duh)
                int32_t Age; // Time since the event occured
        };
    }

    inline std::string Homeserver{ "https://matrix.org" }; // Home server used to make API calls. This should be overridden once a home server has been determined.
    inline std::string Error; // Error code returned by the server (i.e. M_UNKNOWN)
    inline std::string friendlyError; // Human readable error code also returned by the server in most cases (i.e. Unknown error)
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
    const int generateTransID();
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
    const bool saveTransID(const std::string& File);
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
    const bool loadTransID(const std::string& File);
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
    const std::string returnServerDiscovery(std::string Server);
    /**
     * @brief  Returns a home server (matrix.org) from a user ID (@speedie:matrix.org)
     * @param  userID The user ID to extract the home server from.
     * @return Home server string
     */
    const std::string returnHomeServerFromString(const std::string& userID);

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
     * @param  credentialsResponse object, used for authentication.
     * @return Max upload size limit in bytes.
     */
    const int returnMaxUploadLimit(User::credentialsResponse* resp);

    /**
     * @brief  Attempts to create an account on the home server.
     * @param  cred Credentials object to use for registering
     * @return credentialsResponse object, containing the access token among other variables.
     */
    User::credentialsResponse registerAccount(User::Credentials* cred);
    /**
     * @brief  Check if a registration token is still valid or not.
     * @param  Token The token that should be validated.
     * @return Returns true if it's valid, otherwise returns false.
     */
    bool checkRegistrationTokenValidity(const std::string& Token);

    /**
     * @brief  Attempts to log in to the home server.
     * @param  cred Credentials object to use for login
     * @return credentialsResponse object, containing the access token among other variables.
     */
    User::credentialsResponse loginAccount(User::Credentials* cred);
    /**
     * @brief  Refresh an old access token (if a refresh token is available in the object)
     * @param  resp credentialsResponse object, required for authentication.
     * @return Returns a new User::credentialsResponse object based on the parameter.
     */
    User::credentialsResponse refreshAccessToken(User::credentialsResponse* resp);
    /**
     * @brief  Returns user data in the form of a User::Profile object.
     * @param  resp credentialsResponse object, required for authentication.
     * @param  userID User to get data for.
     * @return Returns a User::Profile object containing the user data.
     */
    User::Profile getUserData(User::credentialsResponse* resp, const std::string& userID);

    /**
     * @brief  Returns a full API URL from a Matrix endpoint.
     * @param  API The Matrix endpoint to call.
     * @return The full API URL string.
     *
     * Returns a full API URL from a Matrix endpoint. This uses the leet::Homeserver variable and the passed API.
     * In other words, it returns https://example.com/_matrix/... from the passed API (_matrix/...)
     */
    const std::string getAPI(const std::string& API);

    /**
     * @brief  Invokes a Get request
     * @param  URL URL to request.
     * @param  Authentication Authentication that should be used. Often this is the access token.
     * @return Returns the output from the request.
     */
    const std::string invokeRequest_Get(const std::string& URL, const std::string& Authentication);
    /**
     * @brief  Invokes a Put request
     * @param  URL URL to request.
     * @param  Data Data to Put.
     * @param  Authentication Authentication that should be used. Often this is the access token.
     * @return Returns the output from the request.
     */
    const std::string invokeRequest_Put(const std::string& URL, const std::string& Data, const std::string& Authentication);
    /**
     * @brief  Invokes a Post request
     * @param  URL URL to request.
     * @param  Data Data to Post.
     * @param  Authentication Authentication that should be used. Often this is the access token.
     * @return Returns the output from the request.
     */
    const std::string invokeRequest_Post(const std::string& URL, const std::string& Data, const std::string& Authentication);
    /**
     * @brief  Invokes a Get request
     * @param  URL URL to request.
     * @return Returns the output from the request.
     */
    const std::string invokeRequest_Get(const std::string& URL);
    /**
     * @brief  Invokes a Put request
     * @param  URL URL to request.
     * @param  Data Data to Put.
     * @return Returns the output from the request.
     */
    const std::string invokeRequest_Put(const std::string& URL, const std::string& Data);
    /**
     * @brief  Invokes a Post request
     * @param  URL URL to request.
     * @param  Data Data to Post.
     * @return Returns the output from the request.
     */
    const std::string invokeRequest_Post(const std::string& URL, const std::string& Data);
    /**
     * @brief  Invokes a Post request
     * @param  URL URL to request.
     * @param  File Path to a file which should be uploaded.
     * @return Returns the output from the request.
     */
    const std::string invokeRequest_Post_File(const std::string& URL, const std::string& File);
    /**
     * @brief  Invokes a Post request
     * @param  URL URL to request.
     * @param  File Path to a file which should be uploaded.
     * @param  Authentication Authentication that should be used. Often this is the access token.
     * @return Returns the output from the request.
     */
    const std::string invokeRequest_Post_File(const std::string& URL, const std::string& File, const std::string& Authentication);

    /**
     * @brief  Gets information about a room based on a room ID.
     * @param  resp credentialsResponse object, required for authentication.
     * @param  room Room object, this object should contain a room ID.
     * @return Returns a Room object containing room data.
     */
    Room::Room returnRoom(User::credentialsResponse* resp, Room::Room* room);
    /**
     * @brief  Upgrades a room to a specific room version.
     * @param  resp credentialsResponse object, required for authentication.
     * @param  room Room object, this object should contain a room ID.
     * @param  Version Version integer.
     * @return Returns a room with a new room ID.
     */
    Room::Room upgradeRoom(User::credentialsResponse* resp, Room::Room* room, const int Version);
    /**
     * @brief  Create a room from a Room::roomConfiguration object.
     * @param  resp credentialsResponse object, required for authentication.
     * @parma  conf Room configuration object, this object should contain room information.
     * @return Returns a Room object containing room data.
     */
    Room::Room createRoom(User::credentialsResponse* resp, Room::roomConfiguration* conf);
    /**
     * @brief  Invite a user to a room,
     * @param  resp credentialsResponse object, required for authentication.
     * @param  room Room object, this object should contain a room ID.
     * @param  Reason Reason for the invite being sent.
     */
    void inviteUserToRoom(User::credentialsResponse* resp, Room::Room* room, const std::string& Reason);
    /**
     * @brief  Returns a vector of all rooms your user has joined.
     * @param  resp credentialsResponse object, required for authentication.
     * @param  Limit Max number of rooms to return.
     * @return Returns a vector of all rooms your user has joined.
     */
    const std::vector<Room::Room> returnRooms(User::credentialsResponse* resp, const int Limit);
    /**
     * @brief  Returns room IDs for all the rooms your user has joined.
     * @param  resp credentialsResponse object, required for authentication.
     * @return Returns room IDs for all the rooms your user has joined.
     */
    const std::vector<Room::Room> returnRoomIDs(User::credentialsResponse* resp);
    /**
     * @brief  Resolves a room alias and returns the room ID
     * @param  Alias Room alias to find the room ID from.
     * @return Returns a room ID
     */
    const std::string findRoomID(std::string Alias);

    /**
     * @brief  Returns a vector of all spaces your user has joined.
     * @param  resp credentialsResponse object, required for authentication.
     * @param  Limit Max number of spaces to return.
     * @return Returns a vector of all spaces, along with all child rooms your user has joined.
     */
    const std::vector<Space::Space> returnSpaces(User::credentialsResponse* resp, const int Limit);
    /**
     * @brief  Returns a vector of all rooms in a space.
     * @param  resp credentialsResponse object, required for authentication.
     * @param  spaceID Space ID to get rooms from.
     * @return Returns a vector of all rooms in the space.
     */
    const std::vector<Room::Room> returnRoomsInSpace(User::credentialsResponse* resp, const std::string& spaceID, const int Limit);

    /**
     * @brief  Converts an incomplete user ID to a full user ID. (i.e. speedie is converted to @speedie:matrix.org)
     * @param  Alias The alias to get the user ID from.
     * @param  Homeserver Home server to use.
     * @return Returns a full user ID. You should make sure the home server is actually correct and validate the return value of this function.
     */
    const std::string findUserID(const std::string& Alias, const std::string& Homeserver);
    /**
     * @brief  Returns <name> from @<name>:<home server>
     * @param  userID Full user ID
     * @return Returns a string
     */
    const std::string returnUserName(const std::string& userID);
    /**
     * @brief  Returns a vector of all users in a room.
     * @param  resp credentialsResponse object, required for authentication.
     * @param  room Room object, the function uses the room ID, which must be set.
     * @return Returns a vector of all users in a room.
     */
    const std::vector<User::Profile> returnUsersInRoom(User::credentialsResponse* resp, Room::Room* room);

    /**
     * @brief  Returns a vector of all devices for a specific user.
     * @param  resp credentialsResponse object, required for authentication.
     * @param  user User vector, each User must have the user ID set.
     * @return Returns a vector of all devices for a specific user.
     */
    const std::vector<User::Device> returnDevicesFromUser(User::credentialsResponse* resp, const std::vector<User::Profile>& user);

    /**
     * @brief  Returns a boolean for whether a username is available on the home server or not.
     * @param  Username String to check for.
     * @return Returns true if the username is available, returns false otherwise.
     */
    const bool checkIfUsernameIsAvailable(const std::string& Username);

    /**
     * @brief  Toggles typing in a room.
     * @param  resp credentialsResponse object, required for authentication.
     * @param  Timeout How long to type for.
     * @param  Typing Boolean, whether or not we should be typing or not.
     * @param  room Room to send the event in.
     */
    void toggleTyping(User::credentialsResponse* resp, const int Timeout, const bool Typing, Room::Room* room);

    /**
     * @brief  Sends a plain text message to the specified room.
     * @param  resp credentialsResponse object, required for authentication.
     * @param  room Room object, room that the message should be sent in.
     * @param  msg Message::Message object, the message that should be sent.
     */
    void sendMessage(User::credentialsResponse* resp, Room::Room* room, Message::Message* msg);
    /**
     * @brief  Returns a Message::Message vector from a room.
     * @param  resp credentialsResponse object, required for authentication.
     * @param  room Room object, room that the messages should be retrieved from.
     * @param  messageCount Number of messages to retrieve from the room.
     * @return Returns a Message::Message vector which represents the retrieved messages.
     */
    const std::vector<Message::Message> returnMessages(User::credentialsResponse* resp, Room::Room* room, const int messageCount);

    /**
     * @brief  Returns a filter ID which can be used when requesting data.
     * @param  resp credentialsResponse object, required for authentication.
     * @param  filter Filter object.
     * @return Returns a filter ID which can be used when requesting data.
     */
    const std::string returnFilter(User::credentialsResponse* resp, Filter::Filter *filter);

    /**
     * @brief  Uploads a file to the Matrix server(s).
     * @param  resp credentialsResponse object, required for authentication.
     * @param  File Path to the file which should be uploaded.
     * @return Returns an Attachment::Attachment object containing an mxc:// Matrix URL.
     */
    Attachment::Attachment uploadFile(User::credentialsResponse* resp, const std::string& File);
    /**
     * @brief  Downloads a file from the Matrix server.
     * @param  resp credentialsResponse object, required for authentication.
     * @param  File Attachment object containing an mxc:// URL to download from.
     * @param  outputFile Output file path.
     * @return Returns true if it was downloaded successfully, otherwise false is returned.
     */
    const bool downloadFile(User::credentialsResponse* resp, Attachment::Attachment* Attachment, const std::string& outputFile);

    /**
     * @brief  Gets the Unix timestamp
     * @return Returns a Unix timestamp (i.e. time since epoch)
     */
    const int32_t returnUnixTimestamp();
    /**
     * @brief  Get an event from a Unix timestamp.
     * @param  resp credentialsResponse object, required for authentication.
     * @param  room Room object to get event(s) from.
     * @param  Timestamp Unix timestamp for the event.
     * @param  Direction Boolean which specifies which direction to search for events in. true means forward while false means backwards.
     * @return Returns the event. If none could be found, an empty object is simply returned instead.
     */
    Event::Event returnEventFromTimestamp(User::credentialsResponse* resp, Room::Room* room, const int32_t Timestamp, const bool Direction);
    /**
     * @brief  Gets the latest event sent in the room.
     * @param  resp credentialsResponse object, required for authentication.
     * @param  room Room object to get an event from.
     *
     * Note that this function is just a convenient wrapper for returnEventFromTimestamp()
     */
    Event::Event returnLatestEvent(User::credentialsResponse* resp, Room::Room* room);

    /**
     * @brief  Report an event to the home server owner.
     * @param  resp credentialsResponse object, required for authentication.
     * @param  room Room the event can be found in.
     * @param  event The event to report.
     * @param  Reason Reason for the report being made.
     * @param  Score A score between -100 and 0 with -100 being the most offensive and 0 is inoffensive. If the integer is not in this range 0 will be used.
     */
    void reportEvent(User::credentialsResponse* resp, Room::Room* room, Event::Event* event, const std::string& Reason, const int Score);

    /**
     * @brief  Get sync information from the server.
     * @param  resp credentialsResponse object, required for authentication.
     * @return Returns a Sync object with the fields.
     */
    Sync::Sync returnSync(User::credentialsResponse* resp);

    /**
     * @brief  Get TURN server credentials
     * @param  resp credentialsResponse object, required for authentication.
     * @return Returns a VOIP::Credentials object.
     */
    VOIP::Credentials returnTurnCredentials(User::credentialsResponse* resp);

    #ifndef LEET_NO_ENCRYPTION
    class olmAccount {
        private:
        public:
            using json = nlohmann::json;
            void* accountMemory;
            OlmAccount* Account;
            void* utilityMemory;
            OlmUtility* Utility;
            void* megolmSessionMemory;
            OlmOutboundGroupSession* megolmSession;
            char* megolmSessionID;
            std::size_t megolmSessionIDLength;
            char* megolmSessionKey;
            std::size_t megolmSessionKeyLength;
            char* Identity;
            std::size_t identityLength;
            char* Signature;
            std::size_t signatureLength;
            int keysRemaining{0};

            std::string curve25519;
            std::string ed25519;

            /* Used to store established olm and megolm sessions */
            std::map<std::string, OlmSession*> olmSessions;

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
             * @brief  Throws an 'olm failure: ' error.
             * @param  Failure String to append to the error.
             */
            void handleError(const std::string& Failure) {
                throw std::runtime_error{ "olm failure: " + Failure };
            }

            /**
             * @brief  Creates an account.
             *
             * Creates an account, to be uploaded.
             * In basic test clients, or clients that will only run once or run constantly it is fine to call this
             * each time the program is restarted, but for proper clients you should call this function once, pickle
             * the data, store the key and data and unpickle the data using unpickle() or the convenient loadAccount()
             * function.
             */
            void createAccount() {
                if (!accountMemoryAllocated) {
                    accountMemory = malloc(olm_account_size());
                    Account = olm_account(accountMemory);
                    accountMemoryAllocated = true;
                } else {
                    throw std::runtime_error{ "createAccount(): Cannot allocate memory because it is already allocated." };
                }

                leetCrypto::randomBytes randomBytes(olm_create_account_random_length(Account));

                if (olm_create_account(Account, randomBytes.data(), randomBytes.length()) == olm_error()) {
                    handleError("olm_create_account()");
                }
            }

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
            void loadAccount(const std::string& pickleKey, const std::string& pickleData) {
                if (!accountMemoryAllocated) {
                    accountMemory = malloc(olm_account_size());
                    Account = unpickle(pickleKey, pickleData, Account);
                    accountMemoryAllocated = true;
                } else {
                    throw std::runtime_error{ "loadAccount(): Cannot allocate memory because it is already allocated." };
                }
            }

            /**
             * @brief  Creates a Megolm session.
             *
             * Creates a megolm session.
             */
            void createMegolmSession() {
                if (!megolmSessionMemoryAllocated) {
                    megolmSessionMemory = malloc(olm_outbound_group_session_size());
                    megolmSession = olm_outbound_group_session(megolmSessionMemory);
                    megolmSessionMemoryAllocated = true;
                } else {
                    throw std::runtime_error{ "createMegolmSession(): Cannot allocate memory because it is already allocated. (1)" };
                }
                leetCrypto::randomBytes randomBytesSession(olm_init_outbound_group_session_random_length(megolmSession));

                if (olm_init_outbound_group_session(megolmSession, static_cast<uint8_t* >(randomBytesSession.data()), randomBytesSession.length()) == olm_error()) {
                    free(megolmSessionMemory);
                    megolmSessionMemoryAllocated = false;
                    handleError("olm_init_outbound_group_session()");
                }

                if (!megolmSessionIDMemoryAllocated) {
                    megolmSessionIDLength = olm_outbound_group_session_id_length(megolmSession);
                    megolmSessionID = (char* )malloc(megolmSessionIDLength + 1);
                    megolmSessionIDMemoryAllocated = true;
                } else {
                    throw std::runtime_error{ "createMegolmSession(): Cannot allocate memory because it is already allocated. (2)" };
                }

                std::size_t tSize = olm_outbound_group_session_id(megolmSession, (uint8_t* )megolmSessionID, megolmSessionIDLength);

                if (tSize == olm_error()) {
                    handleError("olm_outbound_group_session_id()");
                }

                megolmSessionID[tSize] = '\0';
            }

            /**
             * @brief  Loads a Megolm session from pickle data
             * @param  pickleKey Pickle key to use when unpickling the data. This must be stored by the client.
             * @param  pickleData Pickle data to unpickle with the key. This must be stored by the client.
             *
             * Loads a megolm session from pickle data
             */
            void loadMegolmSession(const std::string& pickleKey, const std::string& pickleData) {
                if (!megolmSessionMemoryAllocated) {
                    megolmSessionMemory = malloc(olm_outbound_group_session_size());
                    megolmSession = unpickle(pickleKey, pickleData, megolmSession);
                    megolmSessionMemoryAllocated = true;
                } else {
                    throw std::runtime_error{ "createMegolmSession(): Cannot allocate memory because it is already allocated. (1)" };
                }

                if (!megolmSessionIDMemoryAllocated) {
                    megolmSessionIDLength = olm_outbound_group_session_id_length(megolmSession);
                    megolmSessionID = (char* )malloc(megolmSessionIDLength + 1);
                    megolmSessionIDMemoryAllocated = true;
                } else {
                    throw std::runtime_error{ "createMegolmSession(): Cannot allocate memory because it is already allocated. (2)" };
                }

                std::size_t tSize = olm_outbound_group_session_id(megolmSession, (uint8_t* )megolmSessionID, megolmSessionIDLength);

                if (tSize == olm_error()) {
                    handleError("olm_outbound_group_session_id()");
                }

                megolmSessionID[tSize] = '\0';
            }

            /**
             * @brief  Creates an identity.
             */
            void createIdentity() {
                if (!identityMemoryAllocated) {
                    identityLength = olm_account_identity_keys_length(Account);
                    Identity = (char* )malloc(identityLength + 1);
                    identityMemoryAllocated = true;
                } else {
                    throw std::runtime_error{ "createIdentity(): Cannot allocate memory because it is already allocated." };
                }

                std::size_t tSize = olm_account_identity_keys(Account, Identity, identityLength);

                if (tSize == olm_error()) {
                    free(Identity); // Identity was allocated
                    identityMemoryAllocated = false;
                    handleError("olm_account_identity_keys()");
                }

                Identity[tSize] = '\0';
                json identityJson;

                try {
                    identityJson = { json::parse(Identity) };
                } catch (const json::parse_error& e) {
                    free(Identity);
                    identityMemoryAllocated = false;
                    return;
                }

                for (auto &output : identityJson) {
                    leet::errorCode = 0;

                    if (output["curve25519"].is_string())
                        curve25519 = output["curve25519"].get<std::string>();
                    if (output["ed25519"].is_string())
                        ed25519 = output["ed25519"].get<std::string>();
                }

                // Now we don't this anymore
                free(Identity);
                identityMemoryAllocated = false;
            }

            /**
             * @brief Uploads the device keys
             * @param  resp credentialsResponse object, required for authentication.
             */
            void upload(leet::User::credentialsResponse* resp) {
                if (!curve25519.compare("")) {
                    throw std::runtime_error{ "upload(): Identity not allocated." };
                }

                json Body = {
                    { "algorithms", {"m.olm.v1.curve25519-aes-sha2", "m.megolm.v1.aes-sha2" }},
                    { "device_id", resp->deviceID },
                    { "user_id", resp->userID },
                    { "keys", {
                        { std::string("ed25519:") + resp->deviceID, ed25519 },
                        { std::string("curve25519:") + resp->deviceID, curve25519 }
                    } }
                };

                std::string Keys = Body.dump();

                if (!signatureMemoryAllocated) {
                    signatureLength = olm_account_signature_length(Account);
                    Signature = (char* )malloc(signatureLength + 1);
                    signatureMemoryAllocated = true;
                } else {
                    throw std::runtime_error{ "upload(): Cannot allocate memory because it is already allocated." };
                }

                // Sign using the json we constructed, which will provide us with a signature
                if (olm_account_sign(Account, Keys.data(), Keys.length(), Signature, signatureLength) == olm_error()) {
                    free(Signature); // Signature was allocated
                    signatureMemoryAllocated = false;
                    handleError("olm_account_sign()");
                }

                Signature[signatureLength] = '\0';

                Body["signatures"] = {
                    { resp->userID, {
                        { std::string("ed25519:") + resp->deviceID, Signature }
                    } }
                };

                json keysJson = {
                    { "device_keys", Body }
                };

                free(Signature);
                signatureMemoryAllocated = false;

                // Upload our device keys
                const std::string Output {
                    leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/keys/upload"), keysJson.dump(), resp->accessToken)
                };

                json uploadedKeys;

                try {
                    uploadedKeys = json::parse(Output);
                } catch (const json::parse_error& e) {
                    return;
                }

                keysRemaining = uploadedKeys["one_time_key_counts"]["signed_curve25519"];

                // Max / 2 is a good amount of one time keys to keep available
                const int keysToKeep = olm_account_max_number_of_one_time_keys(Account) / 2;

                // We might need to generate new keys
                if (keysRemaining < keysToKeep) {
                    int keysNeeded = keysToKeep - keysRemaining;
                    std::size_t otkLength = olm_account_one_time_keys_length(Account);
                    char* Otk = (char* )malloc(otkLength + 1);

                    std::size_t tSize = olm_account_one_time_keys(Account, Otk, otkLength);

                    if (tSize == olm_error()) {
                        free(Otk); // Otk was allocated
                        handleError("olm_account_one_time_keys() (1)");
                    }

                    Otk[tSize] = '\0';

                    json Otks;

                    try {
                        Otks = json::parse(Otk);
                    } catch (const json::parse_error& e) {
                        return;
                    }

                    free(Otk);

                    int keysAvailable = Otks["ed25519"].size();

                    // We do need to generate new keys
                    if (keysNeeded > keysAvailable) {
                        int keysToGenerate = keysNeeded - keysAvailable;
                        std::size_t otkRandomLength = olm_account_generate_one_time_keys_random_length(Account, keysToGenerate);
                        leetCrypto::randomBytes randomBytes(otkRandomLength);

                        if (olm_account_generate_one_time_keys(Account, keysToGenerate, randomBytes.data(), randomBytes.length()) == olm_error()) {
                            handleError("olm_account_generate_one_time_keys()");
                        }

                        // Now let's get all the keys we have
                        otkLength = olm_account_one_time_keys_length(Account);
                        Otk = (char* )malloc(otkLength + 1);

                        tSize = olm_account_one_time_keys(Account, Otk, otkLength);
                        if (tSize == olm_error()) {
                            handleError("olm_account_one_time_keys() (2)");
                        }

                        Otk[tSize] = '\0';
                        Otks = json::parse(Otk);

                        free(Otk);
                    }

                    json signedOtks;

                    // Now let's sign all of them
                    for (auto& output : Otks["curve25519"].items()) {
                        json Keys = {
                            { "key", output.value() }
                        };

                        if (!signatureMemoryAllocated) {
                            signatureLength = olm_account_signature_length(Account);
                            Signature = (char* )malloc(signatureLength + 1);
                            signatureMemoryAllocated = true;
                        } else {
                            throw std::runtime_error{ "upload(): Cannot allocate memory because it is already allocated." };
                        }

                        std::string theKeys = Keys.dump();

                        if (olm_account_sign(Account, theKeys.data(), theKeys.length(), Signature, signatureLength) == olm_error()) {
                            free(Signature);
                            signatureMemoryAllocated = false;
                            handleError("olm_account_sign()");
                        }

                        Signature[signatureLength] = '\0';

                        Keys["signatures"] = {
                            { resp->userID, {
                                { std::string("ed25519:") + resp->deviceID, Signature }
                            } }
                        };

                        // Increase the number of keys
                        signedOtks[std::string("signed_curve25519:") + output.key()] = Keys;
                        free(Signature);
                        signatureMemoryAllocated = false;
                    }

                    // Upload it all
                    json Body = { { "one_time_keys", signedOtks } };
                    const std::string outputReq {
                        leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/keys/upload"), Body.dump(), resp->accessToken)
                    };

                    olm_account_mark_keys_as_published(Account);
                }
            }

            /**
             * @brief Creates a session with one or more users in the current room.
             * @param  resp credentialsResponse object, required for authentication.
             * @param  room The room to send and create the event in.
             * @param  users Vector of users to create a session with.
             */
            void createSession(leet::User::credentialsResponse* resp, leet::Room::Room* room, const std::vector<leet::User::Profile>& users) {
                if (!megolmSessionMemoryAllocated) {
                    throw std::runtime_error{ "createSession(): Megolm session not allocated." };
                }

                if (!utilityMemoryAllocated) {
                    utilityMemory = malloc(olm_utility_size());
                    Utility = olm_utility(utilityMemory);
                    utilityMemoryAllocated = true;
                } else {
                    throw std::runtime_error{ "createSession(): Cannot allocate memory because it is already allocated. (0)" };
                }

                // Loop through all devices that this user has
                json Body;
                json deviceKeys;

                for (auto& user : users) {
                    for (auto& output : user.Devices) { // each user and each of his devices
                        // deviceKeys should look like the json returned from keys/query
                        if (output.olm && output.megolm) {
                            deviceKeys["device_keys"][output.userID][output.deviceID]["algorithms"] = json::array({"m.olm.v1.curve25519-aes-sha2", "m.megolm.v1.aes-sha2"});
                        } else if (output.olm) {
                            deviceKeys["device_keys"][output.userID][output.deviceID]["algorithms"] = json::array({"m.olm.v1.curve25519-aes-sha2"});
                        } else if (output.megolm) {
                            deviceKeys["device_keys"][output.userID][output.deviceID]["algorithms"] = json::array({"m.megolm.v1.aes-sha2"});
                        }

                        deviceKeys["device_keys"][output.userID][output.deviceID]["device_id"] = output.deviceID;
                        deviceKeys["device_keys"][output.userID][output.deviceID]["keys"]["curve25519:" + output.deviceID] = output.curve25519Key;
                        deviceKeys["device_keys"][output.userID][output.deviceID]["keys"]["ed25519:" + output.deviceID] = output.ed25519Key;
                        deviceKeys["device_keys"][output.userID][output.deviceID]["user_id"] = output.userID;

                        Body["one_time_keys"][output.userID][output.deviceID] = "signed_curve25519";
                    }
                }

                const std::string Output {
                    leet::invokeRequest_Post(leet::getAPI("/_matrix/client/v3/keys/claim"), Body.dump(), resp->accessToken)
                };

                json claimedKeys;

                try {
                    claimedKeys = json::parse(Output);
                } catch (const json::parse_error& e) {
                    return;
                }

                std::size_t tSize;

                json eventToSend;
                for (auto& user : users) {
                    for (auto& output : user.Devices) {
                        json deviceKey = deviceKeys["device_keys"][output.userID][output.deviceID];
                        const std::string keysToSign = deviceKey.dump();

                        char* signatureCopy = (char* )malloc(output.ed25519Signature.length() + 1);
                        output.ed25519Signature.copy(signatureCopy, output.ed25519Signature.length());

                        if (olm_ed25519_verify(Utility, output.ed25519Key.data(), output.ed25519Key.length(),
                            keysToSign.data(), keysToSign.length(), signatureCopy, output.ed25519Signature.length()) == olm_error()) {

                            free(signatureCopy);
                            continue; // Invalid device
                        }

                        free(signatureCopy);

                        // fetch megolm session key
                        megolmSessionKeyLength = olm_outbound_group_session_key_length(megolmSession);
                        megolmSessionKey = (char* )malloc(megolmSessionKeyLength + 1);

                        tSize = olm_outbound_group_session_key(megolmSession, (uint8_t* )megolmSessionKey, megolmSessionKeyLength);

                        if (tSize == olm_error()) {
                            handleError("olm_outbound_group_session_key()");
                        }

                        megolmSessionKey[tSize] = '\0';

                        std::string Otk{""};

                        json Signed = claimedKeys["one_time_keys"][output.userID][output.deviceID];
                        for (auto& signedOtk : Signed.items()) {
                            if (signedOtk.value().contains("key")) {
                                Otk = signedOtk.value()["key"];
                                break;
                            }
                        }

                        if (!Otk.compare("")) {
                            continue;
                        }

                        // Create an olm session
                        OlmSession* Session;
                        void* sessionMemory;

                        if (!olmSessionMemoryAllocated) {
                            sessionMemory = malloc(olm_session_size());
                            Session = olm_session(sessionMemory);
                            olmSessionMemoryAllocated = true;
                        } else {
                            throw std::runtime_error{ "createSession(): Cannot allocate memory because it is already allocated. (4)" };
                        }

                        // Store the session
                        olmSessions[output.userID+output.curve25519Key+output.deviceID] = Session;

                        leetCrypto::randomBytes randomBytes(olm_create_outbound_session_random_length(Session));

                        if (olm_create_outbound_session(Session, Account, output.curve25519Key.data(), output.curve25519Key.length(), Otk.data(), Otk.length(), randomBytes.data(), randomBytes.length()) == olm_error()) {
                            handleError("olm_create_outbound_session()");
                        }

                        json roomKeyMessage;

                        roomKeyMessage["algorithm"] = "m.megolm.v1.aes-sha2";
                        roomKeyMessage["room_id"] = room->roomID;
                        roomKeyMessage["session_id"] = megolmSessionID;
                        roomKeyMessage["session_key"] = megolmSessionKey;

                        json roomKey;

                        roomKey["type"] = "m.room_key";
                        roomKey["content"] = roomKeyMessage;
                        roomKey["sender"] = resp->userID;
                        roomKey["sender_device"] = resp->deviceID;
                        roomKey["recipient"] = output.userID;
                        roomKey["recipient_keys"]["ed25519"] = output.ed25519Key;
                        roomKey["keys"]["ed25519"] = ed25519;

                        const std::string roomKeyMsg = roomKey.dump();

                        leetCrypto::randomBytes randomBytesEncrypt(olm_encrypt_random_length(Session));
                        std::size_t cipherTextLength = olm_encrypt_message_length(Session, roomKeyMsg.length());
                        char* cipherText = (char* )malloc(cipherTextLength + 1);

                        tSize = olm_encrypt(Session, roomKeyMsg.data(), roomKeyMsg.length(), randomBytesEncrypt.data(), randomBytesEncrypt.length(), cipherText, cipherTextLength);

                        if (tSize == olm_error()) {
                            free(cipherText);
                            handleError("olm_encrypt()");
                        }

                        cipherText[tSize] = '\0';

                        json encryptedMessage;

                        encryptedMessage["algorithm"] = "m.olm.v1.curve25519-aes-sha2";
                        encryptedMessage["sender_key"] = curve25519;
                        encryptedMessage["ciphertext"][output.curve25519Key]["type"] = olm_encrypt_message_type(Session);
                        encryptedMessage["ciphertext"][output.curve25519Key]["body"] = cipherText;

                        eventToSend["messages"][output.userID][output.deviceID] = encryptedMessage;

                        free(cipherText);
                        free(megolmSessionKey);
                        free(sessionMemory);

                        olmSessionMemoryAllocated = false;
                    }
                }

                const std::string putOutput {
                    leet::invokeRequest_Put(leet::getAPI("/_matrix/client/v3/sendToDevice/m.room.encrypted/" + std::to_string(transID)), eventToSend.dump(), resp->accessToken)
                };

                free(utilityMemory);
                utilityMemoryAllocated = false;
            }

            /**
             * @brief  Encrypt a message.
             * @param  resp credentialsResponse object, required for authentication.
             * @param  message Message to encrypt.
             * @return Returns a json string containing the encrypted cipher text.
             *
             * Returns string json containing encrypted cipher text, which can then be uploaded to the server as an event
             */
            const std::string encryptMessage(leet::User::credentialsResponse* resp, const std::string& message) {
                std::size_t cipherTextLength = olm_group_encrypt_message_length(megolmSession, message.length());
                char* cipherText = (char* )malloc(cipherTextLength + 1);
                std::size_t tSize = olm_group_encrypt(megolmSession, (uint8_t* )message.data(), message.length(), (uint8_t* )cipherText, cipherTextLength);

                if (tSize == olm_error()) {
                    handleError("olm_group_encrypt()");
                }

                cipherText[tSize] = '\0';

                json retMessage = {
                    { "algorithm", "m.megolm.v1.aes-sha2" },
                    { "sender_key", curve25519 },
                    { "ciphertext", cipherText },
                    { "session_id", megolmSessionID },
                    { "device_id", resp->deviceID }
                };

                free(cipherText);

                return retMessage.dump();
            }

            /**
             * @brief  Pickle an OlmAccount*
             * @param  pickleKey The key that should be used to pickle. The client should (must) store this safely. It is considered sensitive data.
             * @return Returns a pickle for an OlmAccount. The client should (must) store this safely. It is considered sensitive data.
             *
             * Returns a pickle for an OlmAccount
             */
            const std::string pickle(const std::string& pickleKey, OlmAccount* acc) {
                std::size_t pickleLength = olm_pickle_account_length(acc);
                char* Pickle = (char* )malloc(pickleLength + 1);
                const char* pKey = pickleKey.c_str();
                std::size_t pKeyLength = pickleKey.length();

                if (olm_pickle_account(acc, pKey, pKeyLength, Pickle, pickleLength) == olm_error()) {
                    handleError("olm_pickle_account()");
                }

                const std::string ret = Pickle;
                free(Pickle);

                return ret;
            }

            /**
             * @brief  Unpickle an OlmAccount*
             * @param  pickleKey The key that should be used to unpickle. The client should (must) store this safely. It is considered sensitive data.
             * @param  pickleData The pickle data returned by a pickle() function.
             * @return Returns an OlmAccount*.
             *
             * Returns a pickle for an OlmAccount
             */
            OlmAccount* unpickle(const std::string& pickleKey, const std::string& pickleData, OlmAccount* acc) {
                const char* pKey = pickleKey.c_str();
                std::size_t pKeyLength = pickleKey.length();

                std::size_t pickleLength = olm_pickle_account_length(acc);
                char* Pickle = const_cast<char*>(pickleData.c_str());

                if (olm_unpickle_account(acc, pKey, pKeyLength, Pickle, pickleLength) == olm_error()) {
                    handleError("olm_unpickle_account()");
                }

                return acc;
            }

            /**
             * @brief  Pickle an OlmInboundGroupSession*
             * @param  pickleKey The key that should be used to pickle. The client should (must) store this safely. It is considered sensitive data.
             * @return Returns a pickle for an OlmInboundGroupSession. The client should (must) store this safely. It is considered sensitive data.
             *
             * Returns a pickle for an OlmInboundGroupSession
             */
            const std::string pickle(const std::string& pickleKey, OlmInboundGroupSession* session) {
                std::size_t pickleLength = olm_pickle_inbound_group_session_length(session);
                char* Pickle = (char* )malloc(pickleLength + 1);
                const char* pKey = pickleKey.c_str();
                std::size_t pKeyLength = pickleKey.length();

                if (olm_pickle_inbound_group_session(session, pKey, pKeyLength, Pickle, pickleLength) == olm_error()) {
                    handleError("olm_pickle_account()");
                }

                const std::string ret = Pickle;
                free(Pickle);

                return ret;
            }

            /**
             * @brief  Unpickle an OlmInboundGroupSession*
             * @param  pickleKey The key that should be used to unpickle. The client should (must) store this safely. It is considered sensitive data.
             * @param  pickleData The pickle data returned by a pickle() function.
             * @return Returns an OlmInboundGroupSession*.
             *
             * Returns a pickle for an OlmInboundGroupSession
             */
            OlmInboundGroupSession* unpickle(const std::string& pickleKey, const std::string& pickleData, OlmInboundGroupSession* session) {
                const char* pKey = pickleKey.c_str();
                std::size_t pKeyLength = pickleKey.length();

                std::size_t pickleLength = olm_pickle_inbound_group_session_length(session);
                char* Pickle = const_cast<char*>(pickleData.c_str());

                if (olm_unpickle_inbound_group_session(session, pKey, pKeyLength, Pickle, pickleLength) == olm_error()) {
                    handleError("olm_unpickle_account()");
                }

                return session;
            }

            /**
             * @brief  Pickle an OlmOutboundGroupSession*
             * @param  pickleKey The key that should be used to pickle. The client should (must) store this safely. It is considered sensitive data.
             * @return Returns a pickle for an OlmOutboundGroupSession. The client should (must) store this safely. It is considered sensitive data.
             *
             * Returns a pickle for an OlmOutboundGroupSession
             */
            const std::string pickle(const std::string& pickleKey, OlmOutboundGroupSession* session) {
                std::size_t pickleLength = olm_pickle_outbound_group_session_length(session);
                char* Pickle = (char* )malloc(pickleLength + 1);
                const char* pKey = pickleKey.c_str();
                std::size_t pKeyLength = pickleKey.length();

                if (olm_pickle_outbound_group_session(session, pKey, pKeyLength, Pickle, pickleLength) == olm_error()) {
                    handleError("olm_pickle_account()");
                }

                const std::string ret = Pickle;
                free(Pickle);

                return ret;
            }

            /**
             * @brief  Unpickle an OlmOutboundGroupSession*
             * @param  pickleKey The key that should be used to unpickle. The client should (must) store this safely. It is considered sensitive data.
             * @param  pickleData The pickle data returned by a pickle() function.
             * @return Returns an OlmOutboundGroupSession*.
             *
             * Returns a pickle for an OlmOutboundGroupSession
             */
            OlmOutboundGroupSession* unpickle(const std::string& pickleKey, const std::string& pickleData, OlmOutboundGroupSession* session) {
                const char* pKey = pickleKey.c_str();
                std::size_t pKeyLength = pickleKey.length();

                std::size_t pickleLength = olm_pickle_outbound_group_session_length(session);
                char* Pickle = const_cast<char*>(pickleData.c_str());

                if (olm_unpickle_outbound_group_session(session, pKey, pKeyLength, Pickle, pickleLength) == olm_error()) {
                    handleError("olm_unpickle_account()");
                }

                return session;
            }

            /**
             * @brief  Pickle an OlmSession*
             * @param  pickleKey The key that should be used to pickle. The client should (must) store this safely. It is considered sensitive data.
             * @return Returns a pickle for an OlmSession. The client should (must) store this safely. It is considered sensitive data.
             *
             * Returns a pickle for an OlmSession
             */
            const std::string pickle(const std::string& pickleKey, OlmSession* session) {
                std::size_t pickleLength = olm_pickle_session_length(session);
                char* Pickle = (char* )malloc(pickleLength + 1);
                const char* pKey = pickleKey.c_str();
                std::size_t pKeyLength = pickleKey.length();

                if (olm_pickle_session(session, pKey, pKeyLength, Pickle, pickleLength) == olm_error()) {
                    handleError("olm_pickle_account()");
                }

                const std::string ret = Pickle;
                free(Pickle);

                return ret;
            }

            /**
             * @brief  Unpickle an OlmSession*
             * @param  pickleKey The key that should be used to unpickle. The client should (must) store this safely. It is considered sensitive data.
             * @param  pickleData The pickle data returned by a pickle() function.
             * @return Returns an OlmSession*.
             *
             * Returns a pickle for an OlmSession
             */
            OlmSession* unpickle(const std::string& pickleKey, const std::string& pickleData, OlmSession* session) {
                const char* pKey = pickleKey.c_str();
                std::size_t pKeyLength = pickleKey.length();

                std::size_t pickleLength = olm_pickle_session_length(session);
                char* Pickle = const_cast<char*>(pickleData.c_str());

                if (olm_unpickle_session(session, pKey, pKeyLength, Pickle, pickleLength) == olm_error()) {
                    handleError("olm_unpickle_account()");
                }

                return session;
            }

            /**
             * @brief  Clean up by clearing the Olm account.
             */
            void clear() {
                if (accountMemoryAllocated) {
                    olm_clear_account(Account);
                } else {
                    throw std::runtime_error{ "clear(): Cannot clear account because memory is not allocated for the account." };
                }
            }

            /**
             * @brief  Free memory that has been allocated.
             */
            void destroy() {
                if (megolmSessionIDMemoryAllocated) {
                    free(megolmSessionID);
                    megolmSessionIDMemoryAllocated = false;
                }
                if (megolmSessionMemoryAllocated) {
                    free(megolmSessionMemory);
                    megolmSessionMemoryAllocated = false;
                }
                if (accountMemoryAllocated) {
                    free(accountMemory);
                    accountMemoryAllocated = false;
                }
            }
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

            void destroy() {
                if (!Cleaned) {
                    account.clear();
                    account.destroy();
                    Cleaned = true;
                } else {
                    throw std::runtime_error{ "olmAccount: Already destroyed." };
                }
            }
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
     * @param  resp credentialsResponse object, required for authentication.
     * @param  enc Encryption object.
     * @return Returns an Encryption object.
     */
    Encryption uploadKeys(User::credentialsResponse* resp, Encryption* enc);
    /**
     * @brief  Creates a session in a room.
     * @param  resp credentialsResponse object, required for authentication.
     * @param  enc Encryption object.
     * @param  room Room to create a session in.
     * @return Returns an Encryption object.
     */
    Encryption createSessionInRoom(User::credentialsResponse* resp, Encryption* enc, Room::Room* room);
    /**
     * @brief  Sends an encrypted message to a room.
     * @param  resp credentialsResponse object, required for authentication.
     * @param  enc Encryption object.
     * @param  room Room to create a session in.
     * @param  msg Message to send.
     */
    void sendEncryptedMessage(User::credentialsResponse* resp, Encryption* enc, Room::Room* room, Message::Message* msg);

    #endif // #ifndef LEET_NO_ENCRYPTION
}
#endif
