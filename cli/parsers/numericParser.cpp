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

#include "numericParser.h"

namespace microhal {
namespace cli {

std::pair<float, Status> NumericParser<float>::parse(string_view str, const this_type &object) {
    str = removeSpaces(str);
    if (str.size() == 0) return {{}, Status::MissingArgument};
    // spaces in the middle of data are not allowed, return error
    if (str.find(' ') != str.npos) return {{}, Status::IncorectArgument};

    std::array<char, 30> buffer;
    if (str.size() > buffer.size()) return {{}, Status::Error};
    std::copy_n(str.begin(), str.size(), buffer.data());
    buffer[str.size()] = 0;
    char *str_end;
    float tmp = strtof(buffer.data(), &str_end);
    if (str_end == buffer.data()) return {{}, Status::IncorectArgument};
    if (tmp > object.max || tmp == HUGE_VALF) return {{}, Status::MaxViolation};
    if (tmp < object.min) return {{}, Status::MinViolation};
    return {tmp, Status::Success};
}

std::pair<double, Status> NumericParser<double>::parse(string_view str, const this_type &object) {
    str = removeSpaces(str);
    if (str.size() == 0) return {{}, Status::MissingArgument};
    // spaces in the middle of data are not allowed, return error
    if (str.find(' ') != str.npos) return {{}, Status::IncorectArgument};

    std::array<char, 30> buffer;
    if (str.size() > buffer.size()) return {{}, Status::Error};
    std::copy_n(str.begin(), str.size(), buffer.data());
    buffer[str.size()] = 0;
    char *str_end;
    float tmp = strtod(buffer.data(), &str_end);
    if (str_end == buffer.data()) return {{}, Status::IncorectArgument};
    if (tmp > object.max || tmp == HUGE_VAL) return {{}, Status::MaxViolation};
    if (tmp < object.min) return {{}, Status::MinViolation};
    return {tmp, Status::Success};
}

}  // namespace cli
}  // namespace microhal
