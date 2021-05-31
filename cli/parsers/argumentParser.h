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

#ifndef SRC_CLI_PARSERS_ARGUMENTPARSER_H_
#define SRC_CLI_PARSERS_ARGUMENTPARSER_H_

#include <bitset>
#include <span>
#include <string_view>
#include <tuple>
#include "argument.h"
#include "status.h"

namespace microhal {
namespace cli {

template <typename _Tp, size_t... _Idx>
constexpr std::array<char, sizeof...(_Idx)> _to_array(_Tp (&__a)[sizeof...(_Idx)], std::index_sequence<_Idx...>) {
    return {{__a[_Idx]...}};
}

template <size_t N>
struct NameContainer {
    constexpr NameContainer(const char (&name)[N]) : nameM{_to_array(name, std::make_index_sequence<N>{})} {}

    constexpr std::string_view asStringView() const { return {nameM.data(), nameM.size() - 1}; }

    const std::array<char, N> nameM;
};

template <size_t N>
constexpr int index([[maybe_unused]] int i, [[maybe_unused]] const NameContainer<N> &lhs) {
    return -1;
}

template <size_t N>
constexpr int index(int i, const NameContainer<N> &lhs, const auto &rhs, const auto &... rest) {
    if (lhs.asStringView() == rhs.name) return i;
    return index(i + 1, lhs, rest...);
}

class ArgumentParserBase {
 public:
    using string_view = std::string_view;

    ArgumentParserBase(string_view name, string_view description) : name(name), description(description) {}

    static bool isHelpArgument(std::string_view);
    static std::string_view getParameters(std::string_view arguments, int8_t argumentsCount);
    void showUsage(IODevice &ioDevice, const std::span<const Argument *const> arguments) const;

 protected:
    std::string_view name;
    std::string_view description;

    [[nodiscard]] constexpr static string_view removeSpaces(string_view str) {
        // remove leading spaces
        if (auto pos = str.find_first_not_of(' '); pos != str.npos) str.remove_prefix(pos);
        // remove trailing spaces
        if (auto pos = str.find_last_not_of(' '); pos != str.npos) str.remove_suffix(str.size() - pos - 1);
        return str;
    }
};

template <const auto &... parsers>
class ArgumentParser : public ArgumentParserBase {
 public:
    using string_view = std::string_view;

    ArgumentParser(string_view name, string_view description) : ArgumentParserBase(name, description) {}

    [[nodiscard]] Status parse(std::string_view argumentsString, IODevice &ioDevice) {
        using namespace std::literals;
        // remove leading and trailing spaces
        argumentsString = removeSpaces(argumentsString);
        if (argumentsString.size() == 0 && sizeof...(parsers) != 0) return Status::NoArguments;
        wasParsed.reset();
        // decode all parameters
        do {
            const auto pos = argumentsString.find('-');
            if (pos != argumentsString.npos) {
                argumentsString.remove_prefix(pos);
                const auto argument = argumentsString.substr(0, argumentsString.find(' '));
                if (isHelpArgument(argument)) {
                    showUsage(ioDevice);
                    return Status::HelpRequested;
                }
                auto status = parseImpl<0>(argumentsString, parsers...);
                if (status == Status::UnrecognizedParameter) {
                    ioDevice.write("\n\r\tUnrecognized parameter: "sv);
                    ioDevice.write(argument);
                    return Status::UnrecognizedParameter;
                }
                argumentsString.remove_prefix(argument.size());
            } else {
                break;
            }
        } while (argumentsString.size());

        if (!wasRequiredArgumentsParsed<0>(parsers...)) {
            return Status::MissingArgument;
        }
        return Status::Success;
    }

    template <NameContainer name>
    auto get() {
        constexpr int i = index(0, name, parsers...);
        static_assert(i >= 0);
        return std::get<i>(parsedData);
    }

    void showUsage(IODevice &ioDevice) {
        const std::array<const Argument *const, sizeof...(parsers)> argumentParsers{&parsers...};
        ArgumentParserBase::showUsage(ioDevice, argumentParsers);
    }

 private:
    std::bitset<sizeof...(parsers)> wasParsed{};
    std::tuple<typename std::remove_reference<decltype(parsers)>::type::value_type...> parsedData{};

    template <size_t i, typename T>
    Status parseImpl(const std::string_view str, const T &arg) {
        const auto argument = str.substr(0, str.find(' ', 0));
        if (auto parameterCount = arg.correctCommand(argument); parameterCount >= 0) {
            const auto parameter = getParameters(str.substr(0 + argument.size()), parameterCount);
            auto [value, status] = T::parse(parameter, arg);
            if (status == Status::Success) {
                std::get<i>(parsedData) = value;
                wasParsed[i] = 1;
            }
            return status;
        }
        return Status::UnrecognizedParameter;
    }

    template <const size_t index, typename T>
    Status parseImpl(std::string_view str, const T &arg, const auto &... args) {
        const auto argument = str.substr(0, str.find(' ', 0));
        if (const auto parameterCount = arg.correctCommand(argument); parameterCount >= 0) {
            // argument matched parser
            const auto parameter = getParameters(str.substr(0 + argument.size()), parameterCount);
            auto [value, status] = T::parse(parameter, arg);
            if (status == Status::Success) {
                std::get<index>(parsedData) = value;
                wasParsed[index] = 1;
            }
            return status;
        }
        return parseImpl<index + 1>(str, args...);
    }

    template <size_t index, typename T>
    bool wasRequiredArgumentsParsed(const T &arg) {
        if (arg.isRequired() && !wasParsed[index]) return false;
        return true;
    }

    template <size_t index, typename T>
    bool wasRequiredArgumentsParsed(const T &arg, const auto &... args) {
        if (arg.isRequired() && !wasParsed[index]) return false;
        return wasRequiredArgumentsParsed<index + 1>(args...);
    }
};

}  // namespace cli
}  // namespace microhal

#endif /* SRC_CLI_PARSERS_ARGUMENTPARSER_H_ */
