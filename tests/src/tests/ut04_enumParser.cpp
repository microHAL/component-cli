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

#include <doctest/doctest.h>

#include "commonTypes/enumMap.h"
#include "parsers/enumParser.h"

using namespace microhal;
using namespace cli;
using namespace std::literals;

TEST_CASE("Test Enum Parser") {
    enum class Variants { Variant1, Variant2 };
    static constexpr const EnumMap<Variants, std::string_view, 2> map{{{{Variants::Variant1, "Variant1"sv}, {Variants::Variant2, "Variant2"sv}}}};

    constexpr EnumParser enumParser(map, 'v', "variant"sv, Parameter::Flag::Optional, "Select option"sv);
    char buffer[30];
    CHECK(enumParser.formatArgument(buffer) == "[-v {Variant1,Variant2}]"sv);
    CHECK(enumParser.formatHelpEntry(buffer) == " -v {...}, --variant {...}"sv);
    {
        auto [value, status] = enumParser.parse("Variant1", enumParser);
        CHECK(status == Status::Success);
        CHECK(value == Variants::Variant1);
    }
    CHECK(enumParser.parse("Variant", enumParser).second == Status::Error);
}
