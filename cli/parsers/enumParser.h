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

#ifndef SRC_CLI_PARSERS_ENUMPARSER_H_
#define SRC_CLI_PARSERS_ENUMPARSER_H_

#include <optional>
#include "argument.h"

namespace microhal {
namespace cli {

template <typename Map>
class EnumParser : public Argument {
 public:
    using key_t = typename Map::key_t;

    constexpr EnumParser(Map &map, char shortCommand, string_view command, string_view help)
        : Argument(shortCommand, command, "{...}", help), map(map) {}

    constexpr ~EnumParser() = default;

    [[nodiscard]] constexpr ParserStatus parse(string_view str) final {
        str = removeSpaces(str);
        auto result = map.keyFor(str);
        if (result.ec == std::errc()) {
            m_key = result.key;
            return ParserStatus::Success;
        }
        return ParserStatus::Error;
    }

    [[nodiscard]] string_view formatArgument(std::span<char> buffer) final {
        buffer[0] = '[';
        buffer[1] = '-';
        char *ptr = &buffer[3];
        if (shortCommand > 0) {
            buffer[2] = shortCommand;
        } else {
            buffer[2] = '-';
            ptr = std::copy_n(command.begin(), command.size(), &buffer[3]);
        }

        size_t spaceLeft = buffer.data() + buffer.size() - ptr;
        if (spaceLeft < 3) return {buffer.data(), ptr};
        *ptr++ = ' ';
        *ptr++ = '{';
        for (auto &x : map) {
            if (spaceLeft < x.second.size() + 1) break;
            ptr = std::copy_n(x.second.begin(), x.second.size(), ptr);
            *ptr++ = ',';
            spaceLeft -= x.second.size() + 1;
        }
        *(ptr - 1) = '}';
        *ptr++ = ']';

        return {buffer.data(), ptr};
    }

    [[nodiscard]] key_t key() const { return m_key; }

 private:
    const Map map;
    key_t m_key{};
};  // namespace cli

}  // namespace cli
}  // namespace microhal

#endif /* SRC_CLI_PARSERS_ENUMPARSER_H_ */
