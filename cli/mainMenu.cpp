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

#include "mainMenu.h"
#include <string_view>
#include "IODevice/IODevice.h"

using namespace std::literals;

namespace microhal {
void MainMenuBase::goBack(int count) {
    for (int c = 0; c < count; ++c) {
        if (activeMenu.size() > 1) {
            activeMenu.pop_back();
        } else
            break;
    }
}

void MainMenuBase::processCommand(std::string_view command, std::string_view parameters) {
    if (command.size()) {
        /* SubMenu branches searching */
        if ("exit"sv == command) {
            /* Returning to root folder */
            while (activeMenu.size() > 1)
                activeMenu.pop_back();
            return;
        }
        if (".."sv == command) {
            /* Switching menu */
            if (activeMenu.size() > 1) activeMenu.pop_back();
            return;
        }
        if ("ls"sv == command) {
            showCommands({});
            return;
        }

        int visitedFolders = 0;
        bool commandFound = false;
        /* Searching the tree, getting words from string */
        SubMenuBase* activeSubMenu = activeMenu.back();
        for (auto subMenu : activeSubMenu->items) {
            if (subMenu->command(command, parameters, port)) {
                if (subMenu->hasChildrens()) {
                    ++visitedFolders;
                    activeMenu.push_back(static_cast<SubMenuBase*>(subMenu));
                }
                commandFound = true;
                break;
            }
        }
        if (commandFound == false) {
            port.write("\n\r\tno such command..."sv);
            goBack(visitedFolders);
            return;
        }
    }
}

std::string_view MainMenuBase::showCommands(std::string_view command) {
    SubMenuBase* pSubMenu = activeMenu.back();
    /* Just show commands */
    if (command.empty()) {
        for (auto item : pSubMenu->items) {
            port.write("\n\r\t"sv);
            port.write(item->name);
        }
        return {};
    }
    /* Processing and appending letters, pSubMenu is the current subfolder */
    /* Counting how many candidates has the same prefix */
    int samePrefixCnt = 0;
    for (auto item : pSubMenu->items) {
        if (item->name.starts_with(command)) {
            ++samePrefixCnt;
        }
    }
    /* Show candidates with the same prefix */
    for (auto item : pSubMenu->items) {
        if (item->name.starts_with(command)) {
            if (1 == samePrefixCnt) {
                /* Append letters */
                return item->name.substr(command.size());
            } else if (samePrefixCnt > 1) {
                port.write("\n\r\t\t "sv);
                port.write(item->name);
            }
        }
    }
    return {};
}

void MainMenuBase::drawPrompt() {
    port.write("\n\r"sv);
    auto it = activeMenu.begin();
    ++it;
    for (; it != activeMenu.end(); ++it) {
        port.write("> "sv);
        port.write((*it)->name);
        port.write(" "sv);
    }
    port.write("> "sv);
}

}  // namespace microhal

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
