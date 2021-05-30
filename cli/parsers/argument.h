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

#ifndef SRC_CLI_PARSERS_PARAMETERPARSER_H_
#define SRC_CLI_PARSERS_PARAMETERPARSER_H_

#include <charconv>
#include <cstdint>
#include <numeric>
#include <span>
#include <string_view>
#include "IODevice/IODevice.h"
#include "status.h"

namespace microhal {
namespace cli {

namespace implementationDetail {
enum class Flag : uint8_t { Required = 0b1, Parsed = 0b10, LastTimeParsed = 0b100 };

constexpr Flag operator|(Flag lhs, Flag rhs) {
    return static_cast<Flag>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr Flag operator&(Flag lhs, Flag rhs) {
    return static_cast<Flag>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

}  // namespace implementationDetail

class Argument {
 public:
    using string_view = std::string_view;
    using Flag = implementationDetail::Flag;

    [[nodiscard]] constexpr bool isRequired() const noexcept { return (flag & Flag::Required) == Flag::Required; }

    [[nodiscard]] int_fast8_t correctCommand(string_view cmd) const;
    [[nodiscard]] string_view formatArgument(std::span<char> buffer) const;

    [[nodiscard]] string_view formatHelpEntry(std::span<char> buffer) const;
    [[nodiscard]] string_view helpText() const { return help; }

    const signed char shortCommand;
    const string_view command;
    const string_view name;

 protected:
    constexpr Argument(signed char shortCommand, string_view command, string_view name, string_view help)
        : shortCommand(shortCommand), command(command), name(name), help(help) {}

    template <typename Type>
    [[nodiscard]] static auto fromStringView(string_view str, uint_fast8_t base = 10, Type min = std::numeric_limits<Type>::min(),
                                             Type max = std::numeric_limits<Type>::max()) {
        Type result;
        auto [ptr, error] = std::from_chars(str.begin(), str.end(), result, base);
        struct OUT {
            Type value;
            Status ec;
        };
        if (error == std::errc()) {
            if (result > max) return OUT{{}, Status::MaxViolation};
            if (result < min) return OUT{{}, Status::MinViolation};
            return OUT{result, Status::Success};
        }
        if (error == std::errc::result_out_of_range) return OUT{{}, Status::MaxViolation | Status::MinViolation};
        if (error == std::errc::invalid_argument) return OUT{{}, Status::IncorectArgument};
        return OUT{{}, Status::Error};
    }

    [[nodiscard]] constexpr static string_view removeSpaces(string_view str) {
        // remove leading spaces
        if (auto pos = str.find_first_not_of(' '); pos != str.npos) str.remove_prefix(pos);
        // remove trailing spaces
        if (auto pos = str.find_last_not_of(' '); pos != str.npos) str.remove_suffix(str.size() - pos - 1);
        return str;
    }

    const Flag flag{};

    const string_view help;
};

}  // namespace cli
}  // namespace microhal

#endif /* SRC_CLI_PARSERS_PARAMETERPARSER_H_ */
