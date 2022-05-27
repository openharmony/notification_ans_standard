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

#include "notification_distributed_options.h"
#include "ans_log_wrapper.h"

namespace OHOS {
namespace Notification {
NotificationDistributedOptions::NotificationDistributedOptions(
    bool distribute, const std::vector<std::string> &dvsDisplay, const std::vector<std::string> &dvsOperate)
    : isDistributed_(distribute), devicesSupportDisplay_(dvsDisplay), devicesSupportOperate_(dvsOperate)
{}

void NotificationDistributedOptions::SetDistributed(bool distribute)
{
    isDistributed_ = distribute;
}

bool NotificationDistributedOptions::IsDistributed() const
{
    return isDistributed_;
}

void NotificationDistributedOptions::SetDevicesSupportDisplay(const std::vector<std::string> &devices)
{
    devicesSupportDisplay_ = devices;
}

std::vector<std::string> NotificationDistributedOptions::GetDevicesSupportDisplay() const
{
    return devicesSupportDisplay_;
}

void NotificationDistributedOptions::SetDevicesSupportOperate(const std::vector<std::string> &devices)
{
    devicesSupportOperate_ = devices;
}

std::vector<std::string> NotificationDistributedOptions::GetDevicesSupportOperate() const
{
    return devicesSupportOperate_;
}

std::string NotificationDistributedOptions::Dump()
{
    std::string devicesSupportDisplay = "";
    for (auto &device : devicesSupportDisplay_) {
        devicesSupportDisplay += device;
        devicesSupportDisplay += ", ";
    }

    std::string devicesSupportOperate = "";
    for (auto &device : devicesSupportOperate_) {
        devicesSupportOperate += device;
        devicesSupportOperate += ", ";
    }

    return "NotificationDistributedOptions{ "
            "isDistributed = " + std::string((isDistributed_ ? "true" : "false")) +
            ", devicesSupportDisplay = [" + devicesSupportDisplay + "]" +
            ", devicesSupportOperate = [" + devicesSupportOperate + "]" +
            " }";
}

bool NotificationDistributedOptions::ToJson(nlohmann::json &jsonObject) const
{
    jsonObject["isDistributed"]         = isDistributed_;
    jsonObject["devicesSupportDisplay"] = nlohmann::json(devicesSupportDisplay_);
    jsonObject["devicesSupportOperate"] = nlohmann::json(devicesSupportOperate_);

    return true;
}

NotificationDistributedOptions *NotificationDistributedOptions::FromJson(const nlohmann::json &jsonObject)
{
    if (jsonObject.is_null() or !jsonObject.is_object()) {
        ANS_LOGE("Invalid JSON object");
        return nullptr;
    }

    auto pOpt = new (std::nothrow) NotificationDistributedOptions();
    if (pOpt == nullptr) {
        ANS_LOGE("Failed to create distributedOptions instance");
        return nullptr;
    }

    const auto &jsonEnd = jsonObject.cend();
    if (jsonObject.find("isDistributed") != jsonEnd) {
        pOpt->isDistributed_ = jsonObject.at("isDistributed").get<bool>();
    }

    if (jsonObject.find("devicesSupportDisplay") != jsonEnd) {
        pOpt->devicesSupportDisplay_ = jsonObject.at("devicesSupportDisplay").get<std::vector<std::string>>();
    }

    if (jsonObject.find("devicesSupportOperate") != jsonEnd) {
        pOpt->devicesSupportOperate_ = jsonObject.at("devicesSupportOperate").get<std::vector<std::string>>();
    }

    return pOpt;
}

bool NotificationDistributedOptions::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteBool(isDistributed_)) {
        ANS_LOGE("Failed to write flag isdistributed");
        return false;
    }

    if (!parcel.WriteStringVector(devicesSupportDisplay_)) {
        ANS_LOGE("Failed to write devicesSupportDisplay");
        return false;
    }

    if (!parcel.WriteStringVector(devicesSupportOperate_)) {
        ANS_LOGE("Failed to write devicesSupportOperate");
        return false;
    }

    return true;
}

NotificationDistributedOptions *NotificationDistributedOptions::Unmarshalling(Parcel &parcel)
{
    auto objptr = new (std::nothrow) NotificationDistributedOptions();
    if ((objptr != nullptr) && !objptr->ReadFromParcel(parcel)) {
        delete objptr;
        objptr = nullptr;
    }

    return objptr;
}

bool NotificationDistributedOptions::ReadFromParcel(Parcel &parcel)
{
    isDistributed_ = parcel.ReadBool();

    if (!parcel.ReadStringVector(&devicesSupportDisplay_)) {
        ANS_LOGE("Failed to read devicesSupportDisplay");
        return false;
    }

    if (!parcel.ReadStringVector(&devicesSupportOperate_)) {
        ANS_LOGE("Failed to read devicesSupportOperate");
        return false;
    }

    return true;
}
}  // namespace Notification
}  // namespace OHOS