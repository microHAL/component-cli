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

#include "IODevice/ioDeviceNull/IODeviceNull.h"
#include "parsers/argumentParser.h"
#include "parsers/ipParser.h"
#include "parsers/numericParser.h"

using namespace microhal;
using namespace cli;
using namespace std::literals;

class Console : public IODevice {
 public:
    int open([[maybe_unused]] OpenMode mode) noexcept final { return true; }

    void close() noexcept final {}
    int isOpen() const noexcept { return true; }

    ssize_t read([[maybe_unused]] char *buffer, [[maybe_unused]] size_t length) noexcept final { return 0; }

    ssize_t availableBytes() const noexcept final { return 0; }

    ssize_t write(const char *data, size_t length) noexcept final {
        const size_t bufferSpace = sizeof(buffer) - bufferPos;
        if (bufferSpace < length) length = bufferSpace;
        std::copy_n(data, length, &buffer[bufferPos]);
        bufferPos += length;
        return length;
    }

    char buffer[500];
    size_t bufferPos = 0;

    std::string_view text() { return {buffer, bufferPos}; }
};

TEST_CASE("Test Parser") {
    static constexpr const NumericParser<uint32_t> baud('b', "baudrate", "baud", Parameter::Flag::Optional, "Baudrate", 10, 200000);
    static constexpr const NumericParser<uint8_t> dataBits(-1, "dataBits", "data_bits", Parameter::Flag::Optional, "Data bits count.", 1, 9);

    ArgumentParser<baud, dataBits> parser("USART", "USART configuration.");

    IODeviceNull ioDevice;
    CHECK(parser.parse("-b 115200 --dataBits 8", ioDevice) == Status::Success);
    CHECK(parser.get<{"baud"}>() == 115200);
    CHECK(parser.get<{"data_bits"}>() == 8);

    const std::string_view result =
        "usage: USART [-b baud] [--dataBits data_bits]\n\r"
        "\n\r"
        "USART configuration.\n\r"
        "\n\r"
        "optional arguments:\n\r"
        " -h, --help         show this help message and exit\n\r"
        " -b baud, --baudrate baud\n\r"
        "                    Baudrate\n\r"
        " --dataBits data_bits\n\r"
        "                    Data bits count.\n\r";

    Console console;
    parser.showUsage(console);
    CHECK(console.text() == result);
}
