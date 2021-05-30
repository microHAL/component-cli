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

#include "parsers/ipMaskParser.h"
#include "parsers/ipParser.h"

using namespace microhal;
using namespace cli;
using namespace std::literals;

TEST_CASE("Test IP Parser") {
    static constexpr IPParser ipParser("ip", "ip", "Static network addres.");
    char buffer[30];
    CHECK(ipParser.formatArgument(buffer) == "[--ip ip]"sv);
    CHECK(ipParser.formatHelpEntry(buffer) == " --ip ip"sv);

    {
        auto [value, status] = ipParser.parse("0.0.0.0", ipParser);
        CHECK(status == Status::Success);
        CHECK(value == IP{0, 0, 0, 0});
    }
    {
        auto [value, status] = ipParser.parse("255.255.255.255", ipParser);
        CHECK(status == Status::Success);
        CHECK(value == IP{255, 255, 255, 255});
    }
    {
        auto [value, status] = ipParser.parse("1.1.1.1", ipParser);
        CHECK(status == Status::Success);
        CHECK(value == IP{1, 1, 1, 1});
    }
    {
        auto [value, status] = ipParser.parse("10.10.10.10", ipParser);
        CHECK(status == Status::Success);
        CHECK(value == IP{10, 10, 10, 10});
    }
    {
        auto [value, status] = ipParser.parse("100.100.100.100", ipParser);
        CHECK(status == Status::Success);
        CHECK(value == IP{100, 100, 100, 100});
    }
    {
        auto [value, status] = ipParser.parse("192.168.1.1", ipParser);
        CHECK(status == Status::Success);
        CHECK(value == IP{192, 168, 1, 1});
    }
    {
        auto [value, status] = ipParser.parse(" 192.168.1.1 ", ipParser);
        CHECK(status == Status::Success);
        CHECK(value == IP{192, 168, 1, 1});
    }
    CHECK(ipParser.parse("1.1.1. 1", ipParser).second == Status::IncorectArgument);

    CHECK(ipParser.parse("1.1.1 .1", ipParser).second == Status::IncorectArgument);

    CHECK(ipParser.parse("1.1 . 1.1", ipParser).second == Status::IncorectArgument);

    CHECK(ipParser.parse("1.256.1.1", ipParser).second == Status::IncorectArgument);
}

TEST_CASE("Test IP Mask Parser") {
    constexpr IPMaskParser mask("mask", "mask", "Network mask");
    {
        auto [value, status] = mask.parse("255.255.255.0", mask);
        CHECK(status == Status::Success);
        CHECK(value == IP{255, 255, 255, 0});
    }
    {
        auto [value, status] = mask.parse("255.255.255.255", mask);
        CHECK(status == Status::Success);
        CHECK(value == IP{255, 255, 255, 255});
    }
    {
        auto [value, status] = mask.parse("0.0.0.0", mask);
        CHECK(status == Status::Success);
        CHECK(value == IP{0, 0, 0, 0});
    }
    CHECK(mask.parse("255.255.0.255", mask).second == Status::IncorectArgument);
    CHECK(mask.parse("0.255.255.255", mask).second == Status::IncorectArgument);
    CHECK(mask.parse("255.255.255.253", mask).second == Status::IncorectArgument);
}
