/* libleet example client
 * ======================
 *
 * This is a libleet example client which hardcodes credentials and allows messages to be read
 * and sent.
 */
#include <iostream>
#include <string>
#include <algorithm>
#include <filesystem>
#include <libleet/libleet.hpp>

int checkError() {
    if (leet::errorCode != 0) { // Oh no, failed to login
        std::cerr << "Failed. " << leet::friendlyError << " (" << leet::Error << ")\n"; // leet::friendlyError is a (usually) human-friendly error returned by the Matrix server. leet::Error is a more specific error code
        return 1;
    }
    return 0;
}

int main() {
    leet::User::Credentials cred; // Create a credentials object which we'll pass to the login functionm.

    cred.Identifier = LEET_IDENTIFIER_USERID; // Our identifier. We're using a user ID, not a third party ID or phone number.
    cred.Type = LEET_TYPE_PASSWORD; // Our type. We're authenticating using a password, and that's the only supported method as of now.

    /* Get the username */
    std::cout << "\033[2J\033[1;1H";
    std::cout << "Enter a Matrix username (@<username>:<home server>)\n> ";
    std::getline(std::cin, cred.Username);

    /* Get the password */
    std::cout << "\033[2J\033[1;1H";
    std::cout << "Enter a Matrix password\n> ";
    std::getline(std::cin, cred.Password);

    /* In most Matrix clients, the device ID is simply a six character string of characters.
     * This device ID is most likely issued by the home server, but for this example we explicitly
     * set one for simplicity. If the device ID is not specified like this, the client must store
     * this data in some way, for example by writing to a file and restoring it later.
     */
    cred.deviceID = "libleet test client"; // Our device ID

    /* leet::Homeserver specifies which Matrix server to use when making API requests.
     * In most cases, this should be the same home server as you'd log in with.
     */
    cred.Homeserver = leet::Homeserver; /* matrix.org is the default home server */

    cred.Homeserver = leet::returnServerDiscovery(leet::returnHomeServerFromString(cred.Username));

    leet::User::credentialsResponse resp;

    /* Check if we should consider this an attempt to register. Then we will
     * either log in or register.
     */
    if (leet::checkIfUsernameIsAvailable(cred.Username)) {
        resp = leet::registerAccount(&cred);
    } else {
        resp = leet::loginAccount(&cred);
    }

    /* Now that we don't need the credentials anymore, let's get rid of them for security reasons */
    cred.clearCredentials();

    if (checkError() == true) { // Yeah, appears something went wrong.
        return false;
    }

    /* While libleet provides functions for saving a transaction ID to file and loading it:
     *
     * leet::transID = 0; // Initial value is zero, saved if no file exists
     * leet::loadTransID("/tmp/basic-chat-program-transid"); // Now it will be set to the value saved to the file
     * leet::saveTransID("/tmp/basic-chat-program-transid"); // Now we save the value
     *
     * These functions exist purely for demonstration purposes and should not be used in a serious project,
     * because it's a very crude way to get a unique integer each time.
     *
     * A much better approach used by programs such as Element on the client side and
     * Synapse on the server side is to simply use the amount of milliseconds passed
     * since the Unix epoch, like this:
     *
     * leet::transID = leet::returnUnixTimestamp(); // This libleet function simply returns the amount of UNIX millis since January 1st 1970
     *
     * And then we can simply do this again each time we use a function that uses a transaction ID.
     * Please note that the functions do not automatically increment this value, you are responsible
     * for doing so. If you do not, the event will be considered a duplicate by the server as per the
     * Matrix specification and as such most likely ignored.
     *
     * All functions that use a transaction ID will use leet::transID, functions will never require an integer to be manually specified.
     */
    leet::transID = leet::returnUnixTimestamp();

    std::string myRoom{""};
    std::cout << "\033[2J\033[1;1H"; // Clear the screen on UNIX-like operating systems

    /* Now we need to get a list of rooms. For simplicity,
     * we won't bother to support spaces but all clients actually worth using
     * should support spaces, because they are a basic feature of the Matrix spec.
     */

    std::vector<leet::Room::Room> vector = leet::returnRooms(&resp, 9999);

    std::cout << "\n";

    /* Iterate through the room vector we received. It contains a lot of data,
     * but let's just present the room alias (or ID if no alias is specified)
     * and the topic. (if one is specified)
     */
    for (auto& it : vector) {
        const std::string Alias = it.Alias.compare("") ? it.Alias : it.roomID;
        const std::string Topic = it.Topic.compare("") ? it.Topic : "No room topic specified.";
        std::cout << Alias << " - " << Topic << "\n";
    }

    std::cout << "\n";

    std::cout << "Enter a matrix channel:\n> ";
    std::getline(std::cin, myRoom);

    /* This finds the room ID used internally (i.e. #speedie:matrix.org -> !somelongstringoftext), something that is critical
     * because Matrix operates using IDs, while humans probably don't want to memorize the actual room ID. From here on, functions
     * will require the ID, and as such if you do not have an ID then you'll want to use leet::findRoomID() or leet::findUserID() among others.
     */
    leet::Room::Room room;
    room.roomID = leet::findRoomID(myRoom);

    /* Now let's fill it with room properties */
    room = leet::returnRoom(&resp, &room);

    /* Set read marker */
    leet::Event::Event event = leet::returnLatestEvent(&resp, &room);
    leet::setReadMarkerPosition(&resp, &room, &event, &event, &event);

    if (checkError() == true) { // Yeah, appears something went wrong. Most likely means the room ID is invalid.
        std::cout << "Are you stupid? That isn't a valid channel... I think.\n";
        return false;
    }

#ifndef NO_ENCRYPTION
    /* Let's create an Olm account */
    leet::Encryption enc = leet::initEncryption();

    /* Now let's create and upload our device keys */
    enc = leet::uploadKeys(&resp, &enc);

    /* Now let's create an Olm session with each device in the room and upload our new Megolm session */
    enc = leet::createSessionInRoom(&resp, &enc, &room);
#endif

    leet::Sync::Sync sync = leet::returnSync(&resp);

    /* Read user messages in a loop */
    for (;;) {
        std::cout << "\033[2J\033[1;1H"; // Clear the screen on UNIX-like operating systems
        std::vector<leet::Message::Message> messages = leet::returnMessages(&resp, &room, 25);
        std::reverse(messages.begin(), messages.end());

        int i{1};
        for (auto &message : messages) {
            std::cout << "\033[0;31m" << i << ". \033[0m" << message.Sender << " - " << message.messageText << "\n";
            ++i;
        }

        std::string myMessage{""};
        std::cout << "> ";
        std::getline(std::cin, myMessage);

        /* Exiting, so we need to save the transaction ID. Vim-style keybinds are cool, so we'll use them for this example.
	     * And, you know, if you have no quit key, bad things will occur and you might have to unplug your computer.
	     */
        if (!myMessage.compare(":q")) {
#ifndef NO_ENCRYPTION
            enc.destroy(); // We're done with encryption now
#endif
            std::exit(0);
        }

        /* Message class, this will contain message information, and the message itself */
        leet::Message::Message msg;

        msg.messageText = myMessage;
        msg.messageType = "m.text";

        /* We upload a file if it exists
         * Please note that this is a terrible implementation, ideally you
         * should check if the file is an audio file, video file, image, and finally a generic file if all else fails.
         *
         * But for the simplicity of this example, we're going to use m.file, which will tell Matrix that it is a generic file.
         * Below is an example of uploading a video, though.
         */
        std::filesystem::path file{ myMessage };
        if (std::filesystem::exists(file)) {
            msg.messageType = "m.file";
            msg.messageText = file.filename();

            leet::Attachment::Attachment attachment = leet::uploadFile(&resp, myMessage);

            msg.attachmentURL = attachment.URL;

            if (leet::errorCode != 0) { /* Something went wrong */
                continue;
            }
        }

        /* Example of uploading a file embedded as a video.
	     * The main difference is, when you specify 'm.video' or any other more specific type, clients will
	     * treat the file differently. For example, on Element a .mp4 file will be displayed as a generic file if 'm.file'
	     * is used as a type, but if a more specific type such as 'm.video' in this case is used then it will attempt to embed and
	     * preview the video for users.
	     *
	     * Below is an example of a video file with 'm.video' rather than 'm.file'.
	     *
         * msg.messageText = "test.mp4"; // Text, doesn't matter that much but Element does this so we'll copy it
         * msg.messageType = "m.video"; // m.audio, m.video, m.image, m.text, m.file, ...
         *
         * leet::Attachment::Attachment attachment = leet::uploadFile(&resp, "/home/speedie/test.mp4"); // This uploads the file from our computer to the Matrix home server(s).
         *
         * msg.attachmentURL = attachment.URL; // Now our message has an attachment.
         */

        /* Get a new transaction ID */
        leet::transID = leet::returnUnixTimestamp();

        /* Send the encrypted message
         * You can use leet::sendMessage() to send plain text messages. leet::sendEncryptedMessage() should be used if you need to send an encrypted message.
	     * In a proper client, you'd check events to determine whether or not the room is encrypted or not, but that takes effort and I am a lazy programmer, just like you
	     * so I will only put in the bare minimum amount of effort into this example.
         */
#ifndef NO_ENCRYPTION
        leet::sendEncryptedMessage(&resp, &enc, &room, &msg);
#else
        leet::sendMessage(&resp, &room, &msg);
#endif

        if (leet::errorCode != 0) {
#ifndef NO_ENCRYPTION
            enc.destroy(); // We're done with encryption now
#endif
            std::exit(1);
        }
    }

#ifndef NO_ENCRYPTION
    enc.destroy(); // We're done with encryption now
#endif

    /* Alright, so now that you've read the source code there are a few things you must keep in mind.
     * This is not completely representative of what a fully featured client might look like. This is fine for prototype clients,
     * or clients that serve a single purpose such as a Matrix bot, but when writing a proper client you should attempt to
     * cache as much data as possible and asynchronously retrieve new data. This is for speed reasons, and because Matrix isn't
     * particularly well designed.
     *
     * As for encryption, it is important to store sessions, both sent and received. But once again, for simplicity we're not doing that.
     * Especially considering encryption in libleet is still in early development and not to be considered finalized. Encryption is easily
     * the most difficult part of writing a Matrix client, and even with libleet that is no different.
     */
}
