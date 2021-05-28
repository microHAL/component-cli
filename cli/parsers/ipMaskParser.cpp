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

#include "ipMaskParser.h"

namespace microhal {
namespace cli {

Status IPMaskParser::parse(string_view str) {
    // Parse string: 255.255.255.0

    str = removeSpaces(str);
    IP tmpIp;
    for (uint_fast8_t i = 0; i < 4; i++) {
        auto dotPos = str.find('.');
        auto number = str.substr(0, dotPos);
        if (number.find(' ') != number.npos) return Status::Error;
        auto [value, error] = fromStringView<uint8_t>(number);
        if (error != Status::Success) return Status::Error;
        tmpIp.ip[3 - i] = value;
        str.remove_prefix(dotPos + 1);
    }

    if (validateMask(tmpIp)) {
        m_ip = tmpIp;
        flag = flag | Flag::LastTimeParsed | Flag::Parsed;
        return Status::Success;
    }

    return Status::Error;
}

bool IPMaskParser::validateMask(IP mask) {
    uint32_t tmp = mask.asUint32_t();

    bool expectZeros = true;
    if (tmp & 0b1) {
        // LSB is equal 1, so mask should be exual 255.255.255.255
        return tmp == IP{255, 255, 255, 255}.asUint32_t();
    }

    for (uint_fast8_t pos = 0; pos < 32; pos++) {
        if (expectZeros) {
            if (tmp & 0b1) {
                expectZeros = false;
            }
        } else {
            if (!(tmp & 0b1)) return false;
        }
        tmp >>= 1;
    }
    return true;
}

}  // namespace cli
}  // namespace microhal
