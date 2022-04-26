/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef BASE_NOTIFICATION_ANS_STANDARD_TOOLS_DUMP_INCLUDE_SHELL_COMMAND_H
#define BASE_NOTIFICATION_ANS_STANDARD_TOOLS_DUMP_INCLUDE_SHELL_COMMAND_H

#include <map>
#include <string>
#include <functional>
#include <vector>

#include "utils/native/base/include/errors.h"

namespace OHOS {
namespace Notification {
class ShellCommand {
public:
    /**
     * @brief Constructor.
     *
     * @param argc Indicates the count of arguments.
     * @param argv Indicates the arguments.
     * @param name Indicates the tool name.
     */
    ShellCommand(int argc, char *argv[], std::string name);

    /**
     * @brief Destructor.
     */
    virtual ~ShellCommand();

    /**
     * @brief Process the command.
     *
     * @return Indicates the command result code.
     */
    ErrCode OnCommand();

    /**
     * @brief Execute the command.
     *
     * @return Indicates the output text.
     */
    std::string ExecCommand();

    /**
     * @brief Get the error message of the command.
     *
     * @return Indicates the error message.
     */
    std::string GetCommandErrorMsg() const;

    /**
     * @brief Get the error message of the unknown option.
     *
     * @param unknownOption Indicates the unknown option.
     * @return Indicates the error message.
     */
    std::string GetUnknownOptionMsg(std::string &unknownOption) const;

    /**
     * @brief Get the message from the code.
     *
     * @param code Indicates the code.
     * @return Indicates the message.
     */
    std::string GetMessageFromCode(const int32_t code) const;

    /**
     * @brief Create the command map.
     *
     * @return Indicates the result code.
     */
    virtual ErrCode CreateCommandMap() = 0;

    /**
     * @brief Create the message map.
     *
     * @return Indicates the result code.
     */
    virtual ErrCode CreateMessageMap() = 0;

    /**
     * @brief The initialize function.
     *
     * @return Indicates the result code.
     */
    virtual ErrCode init() = 0;

protected:
    static constexpr int MIN_ARGUMENT_NUMBER = 2;

    int argc_;
    char **argv_;

    std::string cmd_;
    std::vector<std::string> argList_;
    std::string name_;
    std::map<std::string, std::function<int()>> commandMap_;
    std::map<int32_t, std::string> messageMap_;
    std::string resultReceiver_ = "";
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_ANS_STANDARD_TOOLS_DUMP_INCLUDE_SHELL_COMMAND_H