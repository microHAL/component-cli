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

#include "argumentParser.h"
#include <algorithm>

using namespace std::literals;

namespace microhal {
namespace cli {

Status ArgumentParser::parse(std::string_view argumentsString, IODevice &ioDevice) {
    // remove leading and trailing spaces
    argumentsString = removeSpaces(argumentsString);

    if (argumentsString.size() == 0 && arguments.size() != 0) return Status::NoArguments;
    // decode all parameters
    do {
        const auto pos = argumentsString.find('-');
        if (pos != argumentsString.npos) {
            const auto argument = argumentsString.substr(pos, argumentsString.find(' ', pos));
            if (isHelpArgument(argument)) {
                showUsage(ioDevice);
                return Status::HelpRequested;
            }
            bool argumentConsumed = false;
            for (auto &arg : arguments) {
                if (auto parameterCount = arg->correctCommand(argument); parameterCount >= 0) {
                    const auto parameter = getParameters(argumentsString.substr(pos + argument.size()), parameterCount);
                    if (auto status = arg->parse(parameter); status != Status::Success) return status;

                    argumentsString.remove_prefix(std::distance(argumentsString.begin(), parameter.end()));
                    argumentConsumed = true;
                    break;
                }
            }
            if (!argumentConsumed) {
                ioDevice.write("\n\r\tUnrecognized parameter: "sv);
                ioDevice.write(argument);
                return Status::UnrecognizedParameter;
            }
        }
    } while (argumentsString.size());
    return Status::Success;
}

void ArgumentParser::showUsage(IODevice &ioDevice) {
    constexpr const std::string_view usage = "usage: ";
    constexpr const std::string_view endl = "\n\r";
    ioDevice.write(usage);
    ioDevice.write(name);
    for (auto argument : arguments) {
        ioDevice.putChar(' ');
        char buffer[30];
        auto result = argument->formatArgument(buffer);
        ioDevice.write(result);
    }
    ioDevice.write(endl);
    ioDevice.write(endl);
    ioDevice.write(description);

    ioDevice.write(endl);
    ioDevice.write(endl);
    ioDevice.write("optional arguments:\n\r -h, --help         show this help message and exit"sv);
    static constexpr const auto spaces = "                    "sv;
    for (auto argument : arguments) {
        ioDevice.write(endl);
        char buffer[40];
        const auto result = argument->formatHelpEntry(buffer);
        ioDevice.write(result);
        if (result.size() > 20) {
            ioDevice.write(endl);
            ioDevice.write(spaces);
        } else {
            ioDevice.write(spaces.substr(0, spaces.size() - result.size()));
        }
        ioDevice.write(argument->helpText());
    }
    ioDevice.write(endl);
}

bool ArgumentParser::isHelpArgument(std::string_view argument) {
    // remove leading and trailing spaces
    argument = removeSpaces(argument);

    return argument == "-h"sv || argument == "--help"sv;
}

std::string_view ArgumentParser::getParameters(std::string_view arguments, int8_t argumentsCount) {
    // remove leading and trailing spaces
    arguments = removeSpaces(arguments);

    size_t argumentsBegin = 0;
    do {
        const auto stringQuoteBeginPos = arguments.find('"');
        const auto stringQuoteEndPos = stringQuoteBeginPos != arguments.npos ? arguments.find('"', stringQuoteBeginPos) : arguments.npos;
        auto parameterSeparator = arguments.find(' ', argumentsBegin);
        if (parameterSeparator == arguments.npos) parameterSeparator = arguments.size();
        if (stringQuoteBeginPos != arguments.npos) {
            if (parameterSeparator > stringQuoteBeginPos && parameterSeparator < stringQuoteEndPos) {
                argumentsBegin = stringQuoteEndPos;
            } else {
                argumentsCount--;
            }
        } else {
            argumentsCount--;
            argumentsBegin = parameterSeparator;
        }
    } while (argumentsCount);

    return {arguments.data(), argumentsBegin};
}

}  // namespace cli
}  // namespace microhal
