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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_ENABLED_CALLBACK_DATA_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_ENABLED_CALLBACK_DATA_H

#include "notification_constant.h"
#include "parcel.h"

namespace OHOS {
namespace Notification {
class EnabledNotificationCallbackData : public Parcelable {
public:
    /**
     * Default constructor used to create a EnabledNotificationCallbackData instance.
     */
    EnabledNotificationCallbackData() = default;

    /**
     * A constructor used to create a EnabledNotificationCallbackData instance with the input parameters passed.
     * @param bundle Indicates the name of the application.
     * @param uid Indicates the uid of the application.
     * @param enable Indicates whether the application is allowed to send or receive notification.
     */
    EnabledNotificationCallbackData(std::string bundle, uid_t uid, bool enable);

    /**
     * Default deconstructor used to deconstruct.
     */
    ~EnabledNotificationCallbackData() = default;

    void SetBundle(const std::string bundle);

    std::string GetBundle() const;

    void SetUid(const uid_t uid);

    uid_t GetUid() const;

    void SetEnable(const bool enable);

    bool GetEnable() const;

    /**
     * Returns a string representation of the object.
     * @return a string representation of the object.
     */
    std::string Dump();

    /**
     * Marshal a object into a Parcel.
     * @param parcel the object into the parcel
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * Unmarshal object from a Parcel.
     * @return the EnabledNotificationCallbackData
     */
    static EnabledNotificationCallbackData *Unmarshalling(Parcel &parcel);

private:
    /**
     * Read a NotificationDoNotDisturbDate object from a Parcel.
     * @param parcel the parcel
     */
    bool ReadFromParcel(Parcel &parcel);

    std::string bundle_;
    uid_t uid_;
    bool enable_;
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_ENABLED_CALLBACK_DATA_H