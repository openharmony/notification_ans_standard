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

#ifndef BASE_NOTIFICATION_ANS_STANDARD_NATIVE_INCLUDE_NOTIFICATION_DISTRIBUTED_OPTIONS_H
#define BASE_NOTIFICATION_ANS_STANDARD_NATIVE_INCLUDE_NOTIFICATION_DISTRIBUTED_OPTIONS_H

#include "notification_json_convert.h"
#include "parcel.h"

namespace OHOS {
namespace Notification {
class NotificationDistributedOptions : public Parcelable, public NotificationJsonConvertionBase {
public:
    /**
     * Default constructor used to create a NotificationDistributedOptions instance.
     */
    NotificationDistributedOptions() = default;

    /**
     * Constructor used to create a NotificationDistributedOptions instance.
     * @param distribute Specifies whether a notification is distributed.
     * @param dvsDisplay The devices that support display.
     * @param dvsOperate The devices that support operate.
     */
    NotificationDistributedOptions(
        bool distribute, const std::vector<std::string> &dvsDisplay, const std::vector<std::string> &dvsOperate);

    /**
     * Default deconstructor used to deconstruct.
     */
    ~NotificationDistributedOptions() = default;

    /**
     * Sets whether a notification is distributed.
     * @param distribute Specifies whether a notification is distributed.
     */
    void SetDistributed(bool distribute);

    /**
     * Checks whether a notification is distributed.
     * @return true if the notification is distributed; returns false otherwise.
     */
    bool IsDistributed() const;

    /**
     * Sets devices that support display.
     * @param devices The devices that support display.
     */
    void SetDevicesSupportDisplay(const std::vector<std::string> &devices);

    /**
     * Obtains the devices that support display.
     * @return the devices that support display.
     */
    std::vector<std::string> GetDevicesSupportDisplay() const;

    /**
     * Sets devices that support operate.
     * @param devices The devices that support operate.
     */
    void SetDevicesSupportOperate(const std::vector<std::string> &devices);

    /**
     * Obtains the devices that support operate.
     * @return the devices that support operate.
     */
    std::vector<std::string> GetDevicesSupportOperate() const;

    /**
     * Returns a string representation of the object.
     * @return a string representation of the object.
     */
    std::string Dump();

    /**
     * Converts a NotificationDistributedOptions object into a Json.
     * @param jsonObject Indicates the Json object.
     */
    bool ToJson(nlohmann::json &jsonObject) const override;

    /**
     * Creates a NotificationDistributedOptions object from a Json.
     * @param jsonObject Indicates the Json object.
     * @return the NotificationDistributedOptions.
     */
    static NotificationDistributedOptions *FromJson(const nlohmann::json &jsonObject);

    /**
     * Marshal a object into a Parcel.
     * @param parcel the object into the parcel
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * Unmarshal object from a Parcel.
     * @return the NotificationDistributedOptions
     */
    static NotificationDistributedOptions *Unmarshalling(Parcel &parcel);

private:
    /**
     * Read a NotificationDistributedOptions object from a Parcel.
     * @param parcel the parcel
     */
    bool ReadFromParcel(Parcel &parcel);

private:
    bool isDistributed_ {true};
    std::vector<std::string> devicesSupportDisplay_ {};
    std::vector<std::string> devicesSupportOperate_ {};
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_ANS_STANDARD_NATIVE_INCLUDE_NOTIFICATION_DISTRIBUTED_OPTIONS_H