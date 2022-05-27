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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_FLAGS_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_FLAGS_H

#include <memory>
#include "parcel.h"

#include "notification_constant.h"
#include "notification_json_convert.h"

namespace OHOS {
namespace Notification {
class NotificationFlags : public Parcelable, public NotificationJsonConvertionBase {
public:
    /**
     * Default constructor used to create an empty NotificationFlags instance.
     */
    NotificationFlags() = default;

    /**
     * Default deconstructor used to deconstruct.
     */
    ~NotificationFlags() = default;

    /**
     * Sets the notification whether enable sound.
     * @param soundEnabled whether enable sound.
     */
    void SetSoundEnabled(NotificationConstant::FlagStatus soundEnabled);

    /**
     * Checks whether enable sound.
     * @return sound enable.
     */
    NotificationConstant::FlagStatus IsSoundEnabled() const;

    /**
     * Sets the notification whether enable vibration.
     * @param vibrationEnabled whether enable vibration.
     */
    void SetVibrationEnabled(NotificationConstant::FlagStatus vibrationEnabled);

    /**
     * Checks whether enable vibration.
     * @return vibration enable.
     */
    NotificationConstant::FlagStatus IsVibrationEnabled() const;

    /**
     * Returns a string representation of the object.
     * @return a string representation of the object.
     */
    std::string Dump();

    /**
     * Converts a NotificationFlags object into a Json.
     * @param jsonObject Indicates the Json object.
     */
    bool ToJson(nlohmann::json &jsonObject) const override;

    /**
     * Creates a NotificationFlags object from a Json.
     * @param jsonObject Indicates the Json object.
     * @return the NotificationFlags.
     */
    static NotificationFlags *FromJson(const nlohmann::json &jsonObject);

    /**
     * Marshal a object into a Parcel.
     * @param parcel the object into the parcel
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * Unmarshal object from a Parcel.
     * @return the NotificationFlags
     */
    static NotificationFlags *Unmarshalling(Parcel &parcel);

private:
    /**
     * Read a NotificationFlags object from a Parcel.
     * @param parcel the parcel
     */
    bool ReadFromParcel(Parcel &parcel);

private:
    NotificationConstant::FlagStatus soundEnabled_ {NotificationConstant::FlagStatus::NONE};
    NotificationConstant::FlagStatus vibrationEnabled_ {NotificationConstant::FlagStatus::NONE};
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_FLAGS_H

