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
 * @defgroup microHAL
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

#ifndef MAINMENU_H_
#define MAINMENU_H_

#include <array>
#include <cstdint>
#include "menuItem.h"
#include "subMenu.h"

namespace microhal {
class CLI;

/**
 * @brief Processes the text given by CLI. Implemented functions for moving through the
 *        menu tree, executing commands, command completion, and others.
 */

class MainMenuBase {
    friend CLI;

 private:
    /**
     * @brief Console port.
     */
    IODevice& port;
    /**
     * @brief List indicating current position in folder tree.
     */
    std::vector<SubMenuBase*> activeMenu{};

    /**
     * @brief Explores the tree of catalogs. Go into sub-folders, executes commands. Puts
     *        text on screen as a result of its work.
     * @param words - list of words to process (order matters of course).
     */
    void processCommand(std::string_view command, std::string_view parameters);

    /**
     * @brief Goes count steps back to root folder. Safe.
     * @param count
     */
    void goBack(int count);
    /**
     * @brief Draws a prompt accordingly to current path.
     */
    void drawPrompt();

    /**
     * @brief Function for showing sub-folder content. Additionally word completion is done.
     * @param words - list of given words.
     * @param maxAppend - count of letters that could be added (without null termination).
     * @return  Returns:
     *          - string_view with letters to appended (if there was just one command conforming given name)
     *          - empty string_view if there is no given sub-folder or there was more conforming items
     */
    std::string_view showCommands(std::string_view command);

 public:
    /**
     * @brief Creates a menu.
     * @param port - IODevice console port.
     */
    MainMenuBase(IODevice& port, SubMenuBase& base) : port(port), activeMenu({&base}) {}
};

template <size_t size>
class MainMenu : public MainMenuBase {
 public:
    template <typename... Args>
    MainMenu(IODevice& port, Args&... args) : MainMenuBase(port, submenu), submenu({}, args...) {}

    SubMenu<size> submenu;
};

}  // namespace microhal

#endif /* MAINMENU_H_ */

/**
 * example
 * @}
 */

/**
 * cli
 * @}
 */

/**
 * components
 * @}
 */
