/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef BASE_NOTIFICATION_ANS_STANDARD_NOTIFICATION_JSON_CONVERT_H
#define BASE_NOTIFICATION_ANS_STANDARD_NOTIFICATION_JSON_CONVERT_H

#include <string>
#include "ans_log_wrapper.h"
#include "nlohmann/json.hpp"

namespace OHOS {
namespace Notification {
class NotificationJsonConvertionBase {
public:
    /**
     * Default deconstructor used to deconstruct.
     */
    virtual ~NotificationJsonConvertionBase() = default;

    virtual bool ToJson(nlohmann::json &jsonObject) const = 0;
};

class NotificationJsonConverter {
public:
    /**
     * Convert NotificationJsonConvertionBase object to json object.
     */
    static bool ConvertToJosn(const NotificationJsonConvertionBase *convertionBase, nlohmann::json &jsonObject)
    {
        if (convertionBase == nullptr) {
            ANS_LOGE("Converter : Invalid base object");
            return false;
        }

        return convertionBase->ToJson(jsonObject);
    }

    static bool ConvertToJosnString(const NotificationJsonConvertionBase *convertionBase, std::string &jsonString)
    {
        if (convertionBase == nullptr) {
            ANS_LOGE("Converter : Invalid base object");
            return false;
        }

        nlohmann::json jsonObject;
        if (!convertionBase->ToJson(jsonObject)) {
            ANS_LOGE("Converter : Cannot convert to JSON object");
            return false;
        }
        jsonString = jsonObject.dump();

        return true;
    }

    template <typename T>
    static T *ConvertFromJosn(const nlohmann::json &jsonObject)
    {
        if (jsonObject.is_null() or !jsonObject.is_object()) {
            ANS_LOGE("Converter : Invalid JSON object");
            return nullptr;
        }

        return T::FromJson(jsonObject);
    }

    template <typename T>
    static T *ConvertFromJosnString(const std::string &jsonString)
    {
        if (jsonString.empty()) {
            ANS_LOGE("Converter : Invalid JSON string");
            return nullptr;
        }

        auto jsonObject = nlohmann::json::parse(jsonString);
        if (jsonObject.is_null() or !jsonObject.is_object()) {
            ANS_LOGE("Converter : Invalid JSON object");
            return nullptr;
        }

        return T::FromJson(jsonObject);
    }
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_ANS_STANDARD_NOTIFICATION_JSON_CONVERT_H
