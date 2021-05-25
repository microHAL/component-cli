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

#include "parsers/numericParser.h"

using namespace microhal;
using namespace cli;
using namespace std::literals;

TEST_CASE("Test Numeric Parser") {
    {
        NumericParser<uint8_t> numeric('n', "number", "varName", "Decode number", 10, 100);
        char toShortBuffer[11];
        char buffer[12];
        CHECK(numeric.formatArgument(toShortBuffer) == std::string_view{});
        CHECK(numeric.formatArgument(buffer) == "[-n varName]"sv);
        {
            char buffer[29];
            CHECK(numeric.formatHelpEntry(buffer) == " -n varName, --number varName"sv);
            CHECK(numeric.helpText() == "Decode number"sv);
        }
        CHECK(numeric.parse("10") == Status::Success);
        CHECK(numeric.value() == 10);

        CHECK(numeric.parse("9") == Status::MinViolation);
        CHECK(numeric.value() == 10);

        CHECK(numeric.parse("100") == Status::Success);
        CHECK(numeric.value() == 100);

        CHECK(numeric.parse("101") == Status::MaxViolation);
        CHECK(numeric.value() == 100);

        // conversion with spaces
        CHECK(numeric.parse(" 10") == Status::Success);
        CHECK(numeric.value() == 10);

        CHECK(numeric.parse(" 9") == Status::MinViolation);

        CHECK(numeric.parse(" 100") == Status::Success);
        CHECK(numeric.value() == 100);

        CHECK(numeric.parse(" 101") == Status::MaxViolation);

        CHECK(numeric.parse("10 ") == Status::Success);
        CHECK(numeric.value() == 10);

        CHECK(numeric.parse("9 ") == Status::MinViolation);

        CHECK(numeric.parse("100 ") == Status::Success);
        CHECK(numeric.value() == 100);

        CHECK(numeric.parse("101 ") == Status::MaxViolation);

        CHECK(numeric.parse(" 10 ") == Status::Success);
        CHECK(numeric.value() == 10);

        CHECK(numeric.parse(" 9 ") == Status::MinViolation);

        CHECK(numeric.parse(" 100 ") == Status::Success);
        CHECK(numeric.value() == 100);

        CHECK(numeric.parse(" 101 ") == Status::MaxViolation);

        // space in the middle of number
        CHECK(numeric.parse(" 1 0") == Status::IncorectArgument);
        CHECK(numeric.parse(" 10 0") == Status::IncorectArgument);
        CHECK(numeric.parse(" 10 1") == Status::IncorectArgument);

        CHECK(numeric.correctCommand("-n") >= 0);
        CHECK(numeric.correctCommand(" -n ") >= 0);

        CHECK(numeric.correctCommand("--number") >= 0);
        CHECK(numeric.correctCommand(" --number ") >= 0);

        CHECK(numeric.correctCommand("n") < 0);
        CHECK(numeric.correctCommand("--n") < 0);
        CHECK(numeric.correctCommand("number") < 0);
        CHECK(numeric.correctCommand("-number") < 0);
        CHECK(numeric.correctCommand("---number") < 0);
    }
    {
        NumericParser<uint8_t> numeric(-1, "number", "varName", "Decode number", 10, 100);
        char toShortBuffer[17];
        char buffer[18];
        CHECK(numeric.formatArgument(toShortBuffer) == std::string_view{});
        CHECK(numeric.formatArgument(buffer) == "[--number varName]"sv);
    }
}

TEST_CASE("Test float numeric Parser") {
    {
        NumericParser<float> numeric('n', "number", "varName", "Decode number", 10.0, 100.0);
        CHECK(numeric.parse("10") == Status::Success);
        CHECK(numeric.value() == 10.0f);
    }
    {
        NumericParser<float> numeric('n', "number", "varName", "Decode number", 10.0, 100.0);
        CHECK(numeric.parse("10.5") == Status::Success);
        CHECK(numeric.value() == 10.5f);
    }
    {
        NumericParser<float> numeric('n', "number", "varName", "Decode number", 10.0, 100.0);
        CHECK(numeric.parse("9.5") == Status::MinViolation);
    }
    {
        NumericParser<float> numeric('n', "number", "varName", "Decode number", 10.0, 100.0);
        CHECK(numeric.parse("100.5") == Status::MaxViolation);
    }
}
