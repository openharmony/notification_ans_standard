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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_DISTRIBUTED_OPTIONS_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_DISTRIBUTED_OPTIONS_H

#include "notification_json_convert.h"
#include "parcel.h"

namespace OHOS {
namespace Notification {
class NotificationDistributedOptions : public Parcelable, public NotificationJsonConvertionBase {
public:
    NotificationDistributedOptions() = default;

    /**
     * @brief Constructor used to create a NotificationDistributedOptions instance.
     *
     * @param distribute Specifies whether a notification is distributed.
     * @param dvsDisplay Indicates the devices that support display.
     * @param dvsOperate Indicates the devices that support operate.
     */
    NotificationDistributedOptions(
        bool distribute, const std::vector<std::string> &dvsDisplay, const std::vector<std::string> &dvsOperate);

    ~NotificationDistributedOptions() = default;

    /**
     * @brief Sets whether a notification is distributed.
     *
     * @param distribute Specifies whether a notification is distributed.
     */
    void SetDistributed(bool distribute);

    /**
     * @brief Checks whether a notification is distributed.
     *
     * @return Returns true if the notification is distributed; returns false otherwise.
     */
    bool IsDistributed() const;

    /**
     * @brief Sets devices that support display.
     *
     * @param devices Indicates the devices that support display.
     */
    void SetDevicesSupportDisplay(const std::vector<std::string> &devices);

    /**
     * @brief Obtains the devices that support display.
     *
     * @return Returns the devices that support display.
     */
    std::vector<std::string> GetDevicesSupportDisplay() const;

    /**
     * @brief Sets devices that support operate.
     *
     * @param devices Indicates the devices that support operate.
     */
    void SetDevicesSupportOperate(const std::vector<std::string> &devices);

    /**
     * @brief Obtains the devices that support operate.
     *
     * @return Returns the devices that support operate.
     */
    std::vector<std::string> GetDevicesSupportOperate() const;

    /**
     * @brief Returns a string representation of the object.
     *
     * @return Returns a string representation of the object.
     */
    std::string Dump();

    /**
     * @brief Converts a NotificationDistributedOptions object into a Json.
     *
     * @param jsonObject Indicates the Json object.
     * @return Returns true if succeed; returns false otherwise.
     */
    bool ToJson(nlohmann::json &jsonObject) const override;

    /**
     * @brief Creates a NotificationDistributedOptions object from a Json.
     *
     * @param jsonObject Indicates the Json object.
     * @return Returns the NotificationDistributedOptions.
     */
    static NotificationDistributedOptions *FromJson(const nlohmann::json &jsonObject);

    /**
     * @brief Marshal a object into a Parcel.
     *
     * @param parcel Indicates the object into the parcel.
     * @return Returns true if succeed; returns false otherwise.
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Unmarshal object from a Parcel.
     *
     * @param parcel Indicates the parcel object.
     * @return Returns the NotificationDistributedOptions object.
     */
    static NotificationDistributedOptions *Unmarshalling(Parcel &parcel);

private:
    /**
     * @brief Read a NotificationDistributedOptions object from a Parcel.
     *
     * @param parcel Indicates the parcel object.
     * @return Returns true if succeed; returns false otherwise.
     */
    bool ReadFromParcel(Parcel &parcel);

private:
    bool isDistributed_ {true};
    std::vector<std::string> devicesSupportDisplay_ {};
    std::vector<std::string> devicesSupportOperate_ {};
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_DISTRIBUTED_OPTIONS_H