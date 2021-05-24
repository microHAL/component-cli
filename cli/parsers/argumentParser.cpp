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

void ArgumentParser::parse(std::string_view argumentsString, IODevice &ioDevice) {
    // remove leading and trailing spaces
    argumentsString.remove_prefix(argumentsString.find_first_not_of(' '));
    if (auto pos = argumentsString.find_last_not_of(' '); pos != argumentsString.npos)
        argumentsString.remove_suffix(argumentsString.size() - pos - 1);

    // decode all parameters
    do {
        auto pos = argumentsString.find('-');
        if (pos != argumentsString.npos) {
            auto argument = argumentsString.substr(pos, argumentsString.find(' ', pos));
            if (isHelpArgument(argument)) {
                showUsage(ioDevice);
                return;
            }
            bool argumentConsumed = false;
            for (auto &arg : arguments) {
                if (auto parameterCount = arg->correctCommand(argument); parameterCount >= 0) {
                    auto parameter = getParameters(argumentsString.substr(pos + argument.size()), parameterCount);
                    arg->parse(parameter);
                    argumentsString.remove_prefix(std::distance(argumentsString.begin(), parameter.end()));
                    argumentConsumed = true;
                    break;
                }
            }
            if (!argumentConsumed) {
                ioDevice.write("\n\r\tUnrecognized parameter: "sv);
                ioDevice.write(argument);
                return;  // error::unrecognizedParameter;
            }
        }
    } while (argumentsString.size());
}

void ArgumentParser::showUsage(IODevice &ioDevice) {
    constexpr std::string_view usage = "usage: ";
    constexpr std::string_view endl = "\n\r";
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
    auto spaces = "                    "sv;
    for (auto argument : arguments) {
        ioDevice.write(endl);
        char buffer[40];
        auto result = argument->formatHelpEntry(buffer);
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
    argument.remove_prefix(argument.find_first_not_of(' '));
    if (auto pos = argument.find_last_not_of(' '); pos != argument.npos) argument.remove_suffix(argument.size() - pos - 1);

    return argument == "-h"sv || argument == "--help"sv;
}

std::string_view ArgumentParser::getParameters(std::string_view arguments, int8_t argumentsCount) {
    // remove leading and trailing spaces
    arguments.remove_prefix(arguments.find_first_not_of(' '));
    if (auto pos = arguments.find_last_not_of(' '); pos != arguments.npos) arguments.remove_suffix(arguments.size() - pos - 1);

    auto argumentsBegin = 0;
    do {
        auto stringQuoteBeginPos = arguments.find('"');
        auto stringQuoteEndPos = stringQuoteBeginPos != arguments.npos ? arguments.find('"', stringQuoteBeginPos) : arguments.npos;
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
