/* libleet
 * Matrix client library written in C++
 * Licensed under the GNU Affero General Public License version 3.
 * See included LICENSE file for more information.
 *
 * https://git.speedie.site/speedie/libleet
 */

#pragma once
#ifndef LEET_NO_ENCRYPTION
#include <iostream>
#include <string>
#include <map>
#include <olm/error.h>
#include <olm/olm.h>
#include <olm/sas.h>

namespace leetOlm {
    inline OlmAccount* Account;
    inline OlmUtility* Utility;
    inline OlmOutboundGroupSession* megolmSession;
    inline std::map<std::string, OlmSession*> olmSessions;

    /**
     * @brief  Pickle an OlmAccount*
     * @param  pickleKey The key that should be used to pickle. The client should (must) store this safely. It is considered sensitive data.
     * @return Returns a pickle for an OlmAccount. The client should (must) store this safely. It is considered sensitive data.
     *
     * Returns a pickle for an OlmAccount
     */
    const std::string pickle(const std::string& pickleKey, OlmAccount* acc);

    /**
     * @brief  Unpickle an OlmAccount*
     * @param  pickleKey The key that should be used to unpickle. The client should (must) store this safely. It is considered sensitive data.
     * @param  pickleData The pickle data returned by a pickle() function.
     * @return Returns an OlmAccount*.
     *
     * Returns a pickle for an OlmAccount
     */
    OlmAccount* unpickle(const std::string& pickleKey, const std::string& pickleData, OlmAccount* acc);

    /**
     * @brief  Pickle an OlmInboundGroupSession*
     * @param  pickleKey The key that should be used to pickle. The client should (must) store this safely. It is considered sensitive data.
     * @return Returns a pickle for an OlmInboundGroupSession. The client should (must) store this safely. It is considered sensitive data.
     *
     * Returns a pickle for an OlmInboundGroupSession
     */
    const std::string pickle(const std::string& pickleKey, OlmInboundGroupSession* session);

    /**
     * @brief  Unpickle an OlmInboundGroupSession*
     * @param  pickleKey The key that should be used to unpickle. The client should (must) store this safely. It is considered sensitive data.
     * @param  pickleData The pickle data returned by a pickle() function.
     * @return Returns an OlmInboundGroupSession*.
     *
     * Returns a pickle for an OlmInboundGroupSession
     */
    OlmInboundGroupSession* unpickle(const std::string& pickleKey, const std::string& pickleData, OlmInboundGroupSession* session);

    /**
     * @brief  Pickle an OlmOutboundGroupSession*
     * @param  pickleKey The key that should be used to pickle. The client should (must) store this safely. It is considered sensitive data.
     * @return Returns a pickle for an OlmOutboundGroupSession. The client should (must) store this safely. It is considered sensitive data.
     *
     * Returns a pickle for an OlmOutboundGroupSession
     */
    const std::string pickle(const std::string& pickleKey, OlmOutboundGroupSession* session);

    /**
     * @brief  Unpickle an OlmOutboundGroupSession*
     * @param  pickleKey The key that should be used to unpickle. The client should (must) store this safely. It is considered sensitive data.
     * @param  pickleData The pickle data returned by a pickle() function.
     * @return Returns an OlmOutboundGroupSession*.
     *
     * Returns a pickle for an OlmOutboundGroupSession
     */
    OlmOutboundGroupSession* unpickle(const std::string& pickleKey, const std::string& pickleData, OlmOutboundGroupSession* session);

    /**
     * @brief  Pickle an OlmSession*
     * @param  pickleKey The key that should be used to pickle. The client should (must) store this safely. It is considered sensitive data.
     * @return Returns a pickle for an OlmSession. The client should (must) store this safely. It is considered sensitive data.
     *
     * Returns a pickle for an OlmSession
     */
    const std::string pickle(const std::string& pickleKey, OlmSession* session);

    /**
     * @brief  Unpickle an OlmSession*
     * @param  pickleKey The key that should be used to unpickle. The client should (must) store this safely. It is considered sensitive data.
     * @param  pickleData The pickle data returned by a pickle() function.
     * @return Returns an OlmSession*.
     *
     * Returns a pickle for an OlmSession
     */
    OlmSession* unpickle(const std::string& pickleKey, const std::string& pickleData, OlmSession* session);
}
#endif
