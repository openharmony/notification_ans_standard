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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_DO_NOT_DISTURB_DATE_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_DO_NOT_DISTURB_DATE_H

#include "notification_constant.h"
#include "parcel.h"

namespace OHOS {
namespace Notification {
class NotificationDoNotDisturbDate : public Parcelable {
public:
    /**
     * Default constructor used to create a NotificationDoNotDisturbDate instance.
     */
    NotificationDoNotDisturbDate() = default;

    /**
     * A constructor used to create a NotificationDoNotDisturbDate instance with the input parameters passed.
     * @param doNotDisturbType Indicates the do not disturb type to add.
     * @param beginDate Indicates the begin time to add.
     * @param endDate Indicates the begin time to add.
     */
    NotificationDoNotDisturbDate(NotificationConstant::DoNotDisturbType doNotDisturbType,
        int64_t beginDate, int64_t endDate);

    /**
     * Default deconstructor used to deconstruct.
     */
    ~NotificationDoNotDisturbDate() = default;

    /**
     * Sets do not disturb type for this NotificationDoNotDisturbDate.
     * @param doNotDisturbType Indicates the do not disturb type to add.
     * For available values, see NotificationConstant::DoNotDisturbType.
     */
    void SetDoNotDisturbType(NotificationConstant::DoNotDisturbType doNotDisturbType);

    /**
     * Obtains the do not disturb type of this NotificationDoNotDisturbDate.
     * @return the do not disturb type of this NotificationDoNotDisturbDate,
     * as enumerated in NotificationConstant::DoNotDisturbType.
     */
    NotificationConstant::DoNotDisturbType GetDoNotDisturbType() const;

    /**
     * Sets begin time for this NotificationDoNotDisturbDate.
     * @param beginDate Indicates the begin time to add.
     */
    void SetBeginDate(const int64_t beginDate);

    /**
     * Obtains the begin time of this NotificationDoNotDisturbDate.
     * @return the begin time of this NotificationDoNotDisturbDate.
     */
    int64_t GetBeginDate() const;

    /**
     * Sets end time for this NotificationDoNotDisturbDate.
     * @param endDate Indicates the end time to add.
     */
    void SetEndDate(const int64_t endDate);

    /**
     * Obtains the end time of this NotificationDoNotDisturbDate.
     * @return the end time of this NotificationDoNotDisturbDate.
     */
    int64_t GetEndDate() const;

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
     * @return the NotificationDoNotDisturbDate
     */
    static NotificationDoNotDisturbDate *Unmarshalling(Parcel &parcel);

private:
    /**
     * Read a NotificationDoNotDisturbDate object from a Parcel.
     * @param parcel the parcel
     */
    bool ReadFromParcel(Parcel &parcel);

private:
    NotificationConstant::DoNotDisturbType doNotDisturbType_ {NotificationConstant::DoNotDisturbType::NONE};
    int64_t beginDate_ {0};
    int64_t endDate_ {0};
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_DO_NOT_DISTURB_DATE_H