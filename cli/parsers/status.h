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

#ifndef SRC_CLI_PARSERS_STATUS_H_
#define SRC_CLI_PARSERS_STATUS_H_

namespace microhal {
namespace cli {

enum class Status {
    Success = 0,
    UnrecognizedParameter = 0b10,
    MaxViolation = 0b100,           // argument value bigger than permitted (set biggest)
    MinViolation = 0b1000,          // argument value lesser than permitted (set smallest)
    LengthViolation = 0b1'0000,     // argument string do not suit given buffer (done nothing)
    MissingArgument = 0b10'0000,    // no argument given
    IncorectArgument = 0b100'0000,  // argument has wrong format
    NoArguments = 0b1000'0000,
    HelpRequested = 1'0000'0000,
    Error = ~Success,
};

constexpr Status operator|(const Status lhs, const Status rhs) {
    return static_cast<Status>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

}  // namespace cli
}  // namespace microhal

#endif /* SRC_CLI_PARSERS_STATUS_H_ */
