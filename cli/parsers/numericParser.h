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

#ifndef SRC_CLI_NUMERICPARSER_H_
#define SRC_CLI_NUMERICPARSER_H_

#include <cmath>
#include <cstdint>
#include <string_view>
#include "argument.h"

namespace microhal {
namespace cli {

template <typename Type>
class NumericParser : public Argument {
 public:
    constexpr NumericParser(char shotCommand, string_view command, string_view name, string_view help, Type min, Type max, uint_fast8_t base = 10)
        : Argument(shotCommand, command, name, help), base(base), min(min), max(max) {}

    [[nodiscard]] Status parse(string_view str) final {
        str = removeSpaces(str);
        if (str.size() == 0) return Status::MissingArgument;
        // spaces in the middle of data are not allowed, return error
        if (str.find(' ') != str.npos) return Status::IncorectArgument;

        auto [value, error] = fromStringView<Type>(str, base, min, max);
        if (error != Status::Success) return error;
        parsedValue = value;
        return Status::Success;
    }

    [[nodiscard]] Type value() const { return parsedValue; }

 private:
    const uint_fast8_t base;
    const Type min;
    const Type max;
    Type parsedValue{};
};

template <>
class NumericParser<float> : public Argument {
 public:
    constexpr NumericParser(char shotCommand, string_view command, string_view name, string_view help, float min, float max)
        : Argument(shotCommand, command, name, help), min(min), max(max) {}

    [[nodiscard]] Status parse(string_view str) final;
    [[nodiscard]] float value() const { return parsedValue; }

 private:
    const float min;
    const float max;
    float parsedValue = NAN;
};

template <>
class NumericParser<double> : public Argument {
 public:
    constexpr NumericParser(char shotCommand, string_view command, string_view name, string_view help, double min, double max)
        : Argument(shotCommand, command, name, help), min(min), max(max) {}

    [[nodiscard]] Status parse(string_view str) final;
    [[nodiscard]] double value() const { return parsedValue; }

 private:
    const double min;
    const double max;
    double parsedValue{};
};

}  // namespace cli
}  // namespace microhal

#endif /* SRC_CLI_NUMERICPARSER_H_ */
