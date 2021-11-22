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

#include "notification_do_not_disturb_date.h"
#include "ans_log_wrapper.h"

namespace OHOS {
namespace Notification {
NotificationDoNotDisturbDate::NotificationDoNotDisturbDate(NotificationConstant::DoNotDisturbType doNotDisturbType,
    int64_t beginDate, int64_t endDate)
    : doNotDisturbType_(doNotDisturbType), beginDate_(beginDate), endDate_(endDate)
{}

void NotificationDoNotDisturbDate::SetDoNotDisturbType(NotificationConstant::DoNotDisturbType doNotDisturbType)
{
    doNotDisturbType_ = doNotDisturbType;
}

NotificationConstant::DoNotDisturbType NotificationDoNotDisturbDate::GetDoNotDisturbType() const
{
    return doNotDisturbType_;
}

void NotificationDoNotDisturbDate::SetBeginDate(const int64_t beginDate)
{
    beginDate_ = beginDate;
}

int64_t NotificationDoNotDisturbDate::GetBeginDate() const
{
    return beginDate_;
}

void NotificationDoNotDisturbDate::SetEndDate(const int64_t endDate)
{
    endDate_ = endDate;
}

int64_t NotificationDoNotDisturbDate::GetEndDate() const
{
    return endDate_;
}

std::string NotificationDoNotDisturbDate::Dump()
{
    return "NotificationDoNotDisturbDate[ "\
           "doNotDisturbType = " + std::to_string(static_cast<int32_t>(doNotDisturbType_)) +
           ", beginDate = " + std::to_string(beginDate_) +
           ", endDate = " + std::to_string(endDate_) + " ]";
}

bool NotificationDoNotDisturbDate::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteInt32(static_cast<int32_t>(doNotDisturbType_))) {
        ANS_LOGE("Failed to write doNotDisturbType");
        return false;
    }

    if (!parcel.WriteInt64(beginDate_)) {
        ANS_LOGE("Failed to write begin time");
        return false;
    }

    if (!parcel.WriteInt64(endDate_)) {
        ANS_LOGE("Failed to write end time");
        return false;
    }

    return true;
}

NotificationDoNotDisturbDate *NotificationDoNotDisturbDate::Unmarshalling(Parcel &parcel)
{
    auto objptr = new (std::nothrow) NotificationDoNotDisturbDate();
    if ((objptr != nullptr) && !objptr->ReadFromParcel(parcel)) {
        delete objptr;
        objptr = nullptr;
    }

    return objptr;
}

bool NotificationDoNotDisturbDate::ReadFromParcel(Parcel &parcel)
{
    doNotDisturbType_ = static_cast<NotificationConstant::DoNotDisturbType>(parcel.ReadInt32());
    beginDate_        = parcel.ReadInt64();
    endDate_          = parcel.ReadInt64();

    return true;
}
}  // namespace Notification
}  // namespace OHOS