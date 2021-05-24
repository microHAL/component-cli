/**
 * @license    BSD 3-Clause
 * @copyright  Pawel Okas
 * @version    $Id$
 * @brief
 *
 * @authors    Pawel Okas
 *
 * @copyright Copyright (c) 2021, Pawel Okas
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 *     1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
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

#include "argument.h"
#include <algorithm>

namespace microhal {
namespace cli {

int_fast8_t Argument::correctCommand(string_view cmd) {
    using namespace std::literals;

    cmd.remove_prefix(cmd.find_first_not_of(' '));
    if (auto pos = cmd.find_last_not_of(' '); pos != cmd.npos) cmd.remove_suffix(cmd.size() - pos - 1);
    if (cmd.starts_with("--"sv)) {
        if (cmd.substr(2) == command) return 1;
    } else if (shortCommand > 0 && cmd.starts_with('-') && cmd.size() == 2) {
        if (cmd[1] == shortCommand) return 1;
    }
    return -1;
}

Argument::string_view Argument::formatArgument(std::span<char> buffer) {
    const auto nameSize = name.size() > 0 ? name.size() + 1 : 0;
    if (shortCommand > 0) {
        if (buffer.size() >= 4 + nameSize) {
            buffer[0] = '[';
            buffer[1] = '-';
            buffer[2] = shortCommand;
            auto end = &buffer[3];
            if (nameSize) {
                *end = ' ';
                end++;
                end = std::copy_n(name.begin(), name.size(), end);
            }
            *end = ']';
            return {buffer.data(), end + 1};
        }
    } else {
        if (buffer.size() >= command.size() + 4 + nameSize) {
            buffer[0] = '[';
            buffer[1] = '-';
            buffer[2] = '-';
            auto end = std::copy_n(command.begin(), command.size(), &buffer[3]);
            if (nameSize) {
                *end = ' ';
                end++;
                end = std::copy_n(name.begin(), name.size(), end);
            }
            *end = ']';
            return {buffer.data(), end + 1};
        }
    }

    return {};
}

Argument::string_view Argument::formatHelpEntry(std::span<char> buffer) {
    auto requiredSize = shortCommand > 0 ? name.size() + 6 : 1;
    requiredSize += command.size() ? command.size() + name.size() + 3 : 0;
    if (buffer.size() < requiredSize) return {};

    auto last = buffer.data();
    *last++ = ' ';
    if (shortCommand > 0) {
        *last++ = '-';
        *last++ = shortCommand;
        *last++ = ' ';
        last = std::copy_n(name.begin(), name.size(), last);
        *last++ = ',';
        *last++ = ' ';
    }
    if (command.size()) {
        *last++ = '-';
        *last++ = '-';
        last = std::copy_n(command.begin(), command.size(), last);
        *last++ = ' ';
        last = std::copy_n(name.begin(), name.size(), last);
    }
    return {buffer.data(), last};
}

}  // namespace cli
}  // namespace microhal
