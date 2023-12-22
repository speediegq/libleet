/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU Lesser General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

#ifndef LEET_NO_ENCRYPTION
#include <iostream>
#include <string>
#include <olm/error.h>
#include <olm/olm.h>
#include <olm/sas.h>

#include "../../include/crypto/olm.hpp"

const std::string leetOlm::pickle(const std::string& pickleKey, OlmAccount* acc) {
    std::size_t pickleLength = olm_pickle_account_length(acc);
    char* Pickle = (char* )malloc(pickleLength + 1);
    const char* pKey = pickleKey.c_str();
    std::size_t pKeyLength = pickleKey.length();

    if (olm_pickle_account(acc, pKey, pKeyLength, Pickle, pickleLength) == olm_error()) {
        throw std::runtime_error("olm_pickle_account()");
    }

    const std::string ret = Pickle;

    free(Pickle);

    return ret;
}

OlmAccount* leetOlm::unpickle(const std::string& pickleKey, const std::string& pickleData, OlmAccount* acc) {
    const char* pKey = pickleKey.c_str();
    std::size_t pKeyLength = pickleKey.length();
    std::size_t pickleLength = olm_pickle_account_length(acc);
    char* Pickle = const_cast<char*>(pickleData.c_str());

    if (olm_unpickle_account(acc, pKey, pKeyLength, Pickle, pickleLength) == olm_error()) {
        throw std::runtime_error("olm_unpickle_account()");
    }

    return acc;
}

const std::string leetOlm::pickle(const std::string& pickleKey, OlmInboundGroupSession* session) {
    std::size_t pickleLength = olm_pickle_inbound_group_session_length(session);
    char* Pickle = (char* )malloc(pickleLength + 1);
    const char* pKey = pickleKey.c_str();
    std::size_t pKeyLength = pickleKey.length();

    if (olm_pickle_inbound_group_session(session, pKey, pKeyLength, Pickle, pickleLength) == olm_error()) {
        throw std::runtime_error("olm_pickle_account()");
    }

    const std::string ret = Pickle;

    free(Pickle);

    return ret;
}

OlmInboundGroupSession* leetOlm::unpickle(const std::string& pickleKey, const std::string& pickleData, OlmInboundGroupSession* session) {
    const char* pKey = pickleKey.c_str();
    std::size_t pKeyLength = pickleKey.length();
    std::size_t pickleLength = olm_pickle_inbound_group_session_length(session);
    char* Pickle = const_cast<char*>(pickleData.c_str());

    if (olm_unpickle_inbound_group_session(session, pKey, pKeyLength, Pickle, pickleLength) == olm_error()) {
        throw std::runtime_error("olm_unpickle_account()");
    }

    return session;
}



const std::string leetOlm::pickle(const std::string& pickleKey, OlmOutboundGroupSession* session) {
    std::size_t pickleLength = olm_pickle_outbound_group_session_length(session);
    char* Pickle = (char* )malloc(pickleLength + 1);
    const char* pKey = pickleKey.c_str();
    std::size_t pKeyLength = pickleKey.length();

    if (olm_pickle_outbound_group_session(session, pKey, pKeyLength, Pickle, pickleLength) == olm_error()) {
        throw std::runtime_error("olm_pickle_account()");
    }

    const std::string ret = Pickle;

    free(Pickle);

    return ret;
}



OlmOutboundGroupSession* leetOlm::unpickle(const std::string& pickleKey, const std::string& pickleData, OlmOutboundGroupSession* session) {
    const char* pKey = pickleKey.c_str();
    std::size_t pKeyLength = pickleKey.length();
    std::size_t pickleLength = olm_pickle_outbound_group_session_length(session);
    char* Pickle = const_cast<char*>(pickleData.c_str());

    if (olm_unpickle_outbound_group_session(session, pKey, pKeyLength, Pickle, pickleLength) == olm_error()) {
        throw std::runtime_error("olm_unpickle_account()");
    }

    return session;
}

const std::string leetOlm::pickle(const std::string& pickleKey, OlmSession* session) {
    std::size_t pickleLength = olm_pickle_session_length(session);
    char* Pickle = (char* )malloc(pickleLength + 1);
    const char* pKey = pickleKey.c_str();
    std::size_t pKeyLength = pickleKey.length();

    if (olm_pickle_session(session, pKey, pKeyLength, Pickle, pickleLength) == olm_error()) {
        throw std::runtime_error("olm_pickle_account()");
    }

    const std::string ret = Pickle;

    free(Pickle);

    return ret;
}



OlmSession* leetOlm::unpickle(const std::string& pickleKey, const std::string& pickleData, OlmSession* session) {
    const char* pKey = pickleKey.c_str();
    std::size_t pKeyLength = pickleKey.length();
    std::size_t pickleLength = olm_pickle_session_length(session);
    char* Pickle = const_cast<char*>(pickleData.c_str());

    if (olm_unpickle_session(session, pKey, pKeyLength, Pickle, pickleLength) == olm_error()) {
        throw std::runtime_error("olm_unpickle_account()");
    }

    return session;
}

#endif
