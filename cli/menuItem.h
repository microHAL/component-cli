/**
 * @license    BSD 3-Clause
 * @copyright  microHAL
 * @version    $Id$
 * @brief    Example of a project that incorporates CLI module.
 *
 * @authors    Kubaszek Mateusz <mathir.km.microhal@gmail.com>
 * created on: <23-01-2016>
 * last modification: <24-01-2016>
 *
 * @copyright Copyright (c) 2015, microHAL
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 *     1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *        2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the
 *           documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this
 *        software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @defgroup microhal
 * @{
 */

/**
 * @defgroup components
 * @{
 */

/**
 * @defgroup cli
 * @{
 */

#ifndef _CLI_MENUITEM_H_
#define _CLI_MENUITEM_H_

#include <span>
#include <string_view>
#include "IODevice/IODevice.h"

namespace microhal {

class MainMenu;
class SubMenu;
class CLI;

/**
 * @brief MenuItem class, the base of all menu elements. Friend of all inheriting items classes.
 */

class MenuItem {
    friend MainMenu;
    friend SubMenu;
    friend CLI;

 public:
    /**
     * @brief Constructs MenuItem instance.
     * @param name - name of MenuItem object visible in CLI.
     * @param help - default help string.
     */
    MenuItem(std::string_view name) : name(name) {}
    virtual ~MenuItem() = default;

    /**
     * @brief Wrapper for executing commands. Checks whether the invocation contains default
     *        command help before the function will be called.
     * @param words
     * @param port
     */
    void command(std::span<std::string_view> words, IODevice& port) {
        port.write("\n\r");
        execute(words, port);
    }

    /**
     * @brief Executes command.
     * @param words - list of command arguments.
     * @param port - a console stream.
     * @return Execution return value (for further implementation).
     */
    virtual int execute(std::span<std::string_view> words, IODevice& port) { return 0; }

    /**
     * @brief	Function for recognition whether it has children list or not. For recognition between itself
     * 		or inheriting class.
     * @return	False.
     */
    virtual inline bool hasChildrens(void) { return false; }

 protected:
    /**
     * @brief CLI object name.
     */
    const std::string_view name;
};

}  // namespace microhal

#endif  // _CLI_MENUITEM_H_

/**
 * cli
 * @}
 */

/**
 * components
 * @}
 */

/**
 * microhal
 * @}
 */
