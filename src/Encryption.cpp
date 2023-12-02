/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU Affero General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

/* Note that most of these functions are simply convenient wrappers which do most of the work for you.
 * If you want, you can set up an olmAccount and Encryption object manually.
 */
#ifndef LEET_NO_ENCRYPTION

leet::Encryption leet::initEncryption() {
    leet::Encryption enc;
    enc.account.createAccount();
    enc.hasCreatedAccount = true;
    return enc;
}

leet::Encryption leet::initEncryptionFromPickle(const std::string& pickleKey, const std::string& pickleData) {
    leet::Encryption enc;

    enc.account.loadAccount(pickleKey, pickleData);
    enc.hasCreatedAccount = true;

    return enc;
}

leet::Encryption leet::uploadKeys(leet::User::CredentialsResponse* resp, leet::Encryption* enc) {
    leet::Encryption ret = *enc;

    if (!ret.hasCreatedAccount) {
        throw std::runtime_error{ "olmAccount: Account has not been created." };
    }
    if (ret.Cleaned) {
        throw std::runtime_error{ "olmAccount: Memory has already been cleared." };
    }

    ret.account.createIdentity();
    ret.account.upload(resp);
    ret.hasUploadedKeys = true;

    return ret;
}

leet::Encryption leet::createSessionInRoom(leet::User::CredentialsResponse* resp, leet::Encryption* enc, leet::Room::Room* room) {
    leet::Encryption ret = *enc;

    if (!ret.hasCreatedAccount) {
        throw std::runtime_error{ "olmAccount: Account has not been created." };
    }
    if (ret.Cleaned) {
        throw std::runtime_error{ "olmAccount: Memory has already been cleared." };
    }
    if (!ret.hasUploadedKeys) {
        throw std::runtime_error{ "olmAccount: Keys have not been uploaded. (!ret.hasUploadedKeys)" };
    }

    // TODO: Store megolm sessions
    ret.account.createMegolmSession();

    std::vector<leet::User::Profile> users = leet::returnUsersInRoom(resp, room);

    ret.account.createSession(resp, room, users);

    return ret;
}

#endif // !LEET_NO_ENCRYPTION
