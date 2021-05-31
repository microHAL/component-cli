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

#include <parsers/parameter.h>
#include <cmath>
#include <cstdint>
#include <string_view>
#include <utility>

namespace microhal {
namespace cli {

template <typename Type>
class NumericParser : public Parameter {
 public:
    using this_type = NumericParser<Type>;
    using value_type = Type;

    consteval NumericParser(char shotCommand, string_view command, string_view name, Flag flags, string_view help, Type min, Type max,
                            uint_fast8_t base = 10)
        : Parameter(shotCommand, command, name, flags, help), base(base), min(min), max(max) {}

    [[nodiscard]] static std::pair<Type, Status> parse(string_view str, const this_type &object) {
        str = removeSpaces(str);
        if (str.size() == 0) return {{}, Status::MissingArgument};
        // spaces in the middle of data are not allowed, return error
        if (str.find(' ') != str.npos) return {{}, Status::IncorectArgument};

        auto [value, error] = Parameter::fromStringView<Type>(str, object.base, object.min, object.max);
        return {value, error};
    }

 private:
    const uint_fast8_t base;
    const Type min;
    const Type max;
};

template <>
class NumericParser<float> : public Parameter {
 public:
    using this_type = NumericParser<float>;
    using value_type = float;

    constexpr NumericParser(char shotCommand, string_view command, string_view name, Flag flags, string_view help, float min, float max)
        : Parameter(shotCommand, command, name, flags, help), min(min), max(max) {}

    [[nodiscard]] static std::pair<float, Status> parse(string_view str, const this_type &object);

 private:
    const float min;
    const float max;
};

template <>
class NumericParser<double> : public Parameter {
 public:
    using this_type = NumericParser<double>;
    using value_type = double;

    constexpr NumericParser(char shotCommand, string_view command, string_view name, Flag flags, string_view help, double min, double max)
        : Parameter(shotCommand, command, name, flags, help), min(min), max(max) {}

    [[nodiscard]] static std::pair<double, Status> parse(string_view str, const this_type &object);

 private:
    const double min;
    const double max;
};

}  // namespace cli
}  // namespace microhal

#endif /* SRC_CLI_NUMERICPARSER_H_ */
