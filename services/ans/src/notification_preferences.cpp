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

#include "notification_preferences.h"

#include <fstream>
#include <ios>
#include <sys/stat.h>

#include "directory_ex.h"
#include "uri.h"

#include "ans_const_define.h"
#include "ans_log_wrapper.h"

namespace OHOS {
namespace Notification {

namespace {
const static std::string kStorePath = "/data/ans";
const static std::string kStoreFile = kStorePath + "/Notification.json";
const static std::string kDisturbMode = "disturbMode";
const static std::string kEnable = "enable";
const static std::string kBundle = "bundle";
const static std::string kBundleName = "bundleName";
const static std::string kBundleImportance = "importance";
const static std::string kBundleShowBadge = "isShowBadge";
const static std::string kBundleBadgeTotalNum = "badgeTotalNum";
const static std::string kBundlePrivateAllowed = "isPrivateAllowed";
const static std::string kBundleEnableNotification = "isEnabledNotification";

const static std::string kGroup = "slotGroup";
const static std::string kGroupId = "groupId";
const static std::string kGroupName = "groupName";
const static std::string kGroupDescription = "groupDescription";
const static std::string kGroupDisable = "isDisable";

const static std::string kSlot = "slot";
const static std::string kSlotGroupId = "groupId";
const static std::string kSlotId = "slotId";
const static std::string kSlotType = "slotType";
const static std::string kSlotName = "slotName";
const static std::string kSlotDescription = "slotDescription";
const static std::string kSlotLevel = "slotLevel";
const static std::string kSlotShowBadge = "isShowBadge";
const static std::string kSlotEnableLight = "isEnableLight";
const static std::string kSlotEnableVrbration = "isEnableVibration";
const static std::string kSlotLedLightColor = "ledLightColor";
const static std::string kSlotLockscreenVisibleness = "isLockscreenVisibleness";
const static std::string kSlotSound = "sound";
const static std::string kSlotVibrationSytle = "vibrationSytle";
const static std::string kSlotEnableBypassDnd = "isEnableBypassDnd";

const static uint8_t kJsonDump = 4;

}  // namespace

enum class NotificationPreferences::BundlePropertyType {
    kBundleImportanceType = 1,
    kBundleShowBadgeType,
    kBundleBadgeTotalNumType,
    kBundlePrivateAllowedType,
    kBundleEnableNotificationType,
};

NotificationPreferences::NotificationPreferences()
{
    InitDir();
    InitFile();
    ParseNotification();
}

NotificationPreferences::~NotificationPreferences()
{
    StoreNotification(paraseJson_);
}

NotificationPreferences &NotificationPreferences::GetInstance()
{
    return DelayedRefSingleton<NotificationPreferences>::GetInstance();
}

ErrCode NotificationPreferences::AddNotificationSlots(
    const std::string &bundleName, const std::vector<sptr<NotificationSlot>> &slots)
{
    if (bundleName.empty() || slots.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    PreferencesInfo preferencesInfo = preferencesInfo_;
    nlohmann::json paraseJson = paraseJson_;
    ErrCode result = ERR_OK;
    for (auto slot : slots) {
        result = CheckSlotForCreateSlot(bundleName, slot, preferencesInfo, paraseJson);
        if (result != ERR_OK) {
            return result;
        }
    }
    result = CommitNotification(paraseJson, preferencesInfo);
    return result;
}

ErrCode NotificationPreferences::AddNotificationSlotGroups(
    const std::string &bundleName, const std::vector<sptr<NotificationSlotGroup>> &groups)
{
    if (bundleName.empty() || groups.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    PreferencesInfo preferencesInfo = preferencesInfo_;
    nlohmann::json paraseJson = paraseJson_;
    ErrCode result = ERR_OK;
    for (auto group : groups) {
        result = CheckGroupForCreateSlotGroup(bundleName, group, preferencesInfo, paraseJson);
        if (result != ERR_OK) {
            return result;
        }
    }
    result = CommitNotification(paraseJson, preferencesInfo);
    return result;
}

ErrCode NotificationPreferences::RemoveNotificationSlot(
    const std::string &bundleName, const NotificationConstant::SlotType &slotType)
{
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    PreferencesInfo preferencesInfo = preferencesInfo_;
    nlohmann::json paraseJson = paraseJson_;
    ErrCode result = ERR_OK;
    result = CheckSlotForRemoveSlot(bundleName, slotType, preferencesInfo, paraseJson);
    if (result != ERR_OK) {
        return result;
    }
    result = CommitNotification(paraseJson, preferencesInfo);
    return result;
}

ErrCode NotificationPreferences::RemoveNotificationSlotGroups(
    const std::string &bundleName, const std::vector<std::string> &groupIds)
{
    if (bundleName.empty() || groupIds.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    PreferencesInfo preferencesInfo = preferencesInfo_;
    nlohmann::json paraseJson = paraseJson_;
    ErrCode result = ERR_OK;
    for (auto groupId : groupIds) {
        result = CheckGroupForRemoveSlotGroup(bundleName, groupId, preferencesInfo, paraseJson);
        if (result != ERR_OK) {
            return result;
        }
    }
    result = CommitNotification(paraseJson, preferencesInfo);
    return result;
}

ErrCode NotificationPreferences::RemoveNotificationForBundle(const std::string &bundleName)
{
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    PreferencesInfo preferencesInfo = preferencesInfo_;
    nlohmann::json paraseJson = paraseJson_;

    ErrCode result = ERR_OK;
    auto iter = preferencesInfo.infos.find(bundleName);
    if (iter != preferencesInfo.infos.end()) {
        preferencesInfo.infos.erase(iter);
        RemoveBundleInJson(paraseJson, bundleName);
        result = CommitNotification(paraseJson, preferencesInfo);
    } else {
        result = ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }
    return result;
}

ErrCode NotificationPreferences::UpdateNotificationSlots(
    const std::string &bundleName, const std::vector<sptr<NotificationSlot>> &slots)
{
    if (bundleName.empty() || slots.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    PreferencesInfo preferencesInfo = preferencesInfo_;
    nlohmann::json paraseJson = paraseJson_;
    ErrCode result = ERR_OK;
    for (auto slotIter : slots) {
        result = CheckSlotForUpdateSlot(bundleName, slotIter, preferencesInfo, paraseJson);
        if (result != ERR_OK) {
            return result;
        }
    }

    result = CommitNotification(paraseJson, preferencesInfo);
    return result;
}

ErrCode NotificationPreferences::UpdateNotificationSlotGroups(
    const std::string &bundleName, const std::vector<sptr<NotificationSlotGroup>> &groups)
{
    if (bundleName.empty() || groups.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    PreferencesInfo preferencesInfo = preferencesInfo_;
    nlohmann::json paraseJson = paraseJson_;
    ErrCode result = ERR_OK;
    for (auto groupIter : groups) {
        result = CheckGroupForUpdateSlotGroup(bundleName, groupIter, preferencesInfo, paraseJson);
        if (result != ERR_OK) {
            return result;
        }
    }

    result = CommitNotification(paraseJson, preferencesInfo);
    return result;
}

ErrCode NotificationPreferences::GetNotificationSlot(
    const std::string &bundleName, const NotificationConstant::SlotType &type, sptr<NotificationSlot> &slot)
{
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode result = ERR_OK;
    auto preferencesIter = preferencesInfo_.infos.find(bundleName);
    if (preferencesIter != preferencesInfo_.infos.end()) {
        auto slotIter = preferencesIter->second.slots.find(type);
        if (slotIter != preferencesIter->second.slots.end()) {
            slot = slotIter->second;
        } else {
            result = CreateSlot(bundleName, type, slot);
        }
    } else {
        result = CreateSlot(bundleName, type, slot);
    }
    return result;
}

ErrCode NotificationPreferences::GetNotificationAllSlots(
    const std::string &bundleName, std::vector<sptr<NotificationSlot>> &slots)
{
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode result = ERR_OK;
    auto iter = preferencesInfo_.infos.find(bundleName);
    if (iter != preferencesInfo_.infos.end()) {
        std::for_each(iter->second.slots.begin(),
            iter->second.slots.end(),
            [&](std::map<NotificationConstant::SlotType, sptr<NotificationSlot>>::reference slot) {
                slots.emplace_back(slot.second);
            });
    } else {
        ANS_LOGE("Notification bundle does not exsit.");
        result = ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }

    return result;
}

ErrCode NotificationPreferences::GetNotificationSlotGroup(
    const std::string &bundleName, const std::string &groupId, sptr<NotificationSlotGroup> &group)
{
    if (bundleName.empty() || groupId.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode result = ERR_OK;
    auto preferencesIter = preferencesInfo_.infos.find(bundleName);
    if (preferencesIter != preferencesInfo_.infos.end()) {
        auto groupIter = preferencesIter->second.groups.begin();
        while (groupIter != preferencesIter->second.groups.end()) {
            if (!groupId.compare(groupIter->first)) {
                group = groupIter->second;
                break;
            }
            ++groupIter;
        }
    } else {
        ANS_LOGE("Notification bundle does not exsit.");
        result = ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }
    return result;
}

ErrCode NotificationPreferences::GetNotificationAllSlotGroups(
    const std::string &bundleName, std::vector<sptr<NotificationSlotGroup>> &groups)
{
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode result = ERR_OK;
    auto preferencesIter = preferencesInfo_.infos.find(bundleName);
    if (preferencesIter != preferencesInfo_.infos.end()) {
        std::for_each(preferencesIter->second.groups.begin(),
            preferencesIter->second.groups.end(),
            [&](std::map<std::string, sptr<NotificationSlotGroup>>::reference group) {
                groups.emplace_back(group.second);
            });
    } else {
        ANS_LOGE("Notification bundle does not exsit.");
        result = ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }
    return result;
}

ErrCode NotificationPreferences::GetNotificationAllSlotInSlotGroup(
    const std::string &bundleName, const std::string &groupId, std::vector<sptr<NotificationSlot>> &slots)
{
    if (bundleName.empty() || groupId.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode result = ERR_OK;
    auto preferencesIter = preferencesInfo_.infos.find(bundleName);
    if (preferencesIter != preferencesInfo_.infos.end()) {
        auto slotIter = preferencesIter->second.slots.begin();
        while (slotIter != preferencesIter->second.slots.end()) {
            if ((slotIter->second != nullptr) && (!groupId.compare(slotIter->second->GetSlotGroup()))) {
                slots.emplace_back(slotIter->second);
            }
            ++slotIter;
        }
    } else {
        ANS_LOGE("Notification bundle does not exsit.");
        result = ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }
    return result;
}

ErrCode NotificationPreferences::IsShowBadge(const std::string &bundleName, bool &enable)
{
    ErrCode result = ERR_ANS_INVALID_PARAM;
    if (bundleName.empty()) {
        return result;
    }
    result = GetBundleProperty(bundleName, BundlePropertyType::kBundleShowBadgeType, enable);
    return result;
}

ErrCode NotificationPreferences::SetShowBadge(const std::string &bundleName, const bool enable)
{
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    PreferencesInfo preferencesInfo = preferencesInfo_;
    nlohmann::json paraseJson = paraseJson_;

    SetBundleProperty(preferencesInfo, paraseJson, bundleName, BundlePropertyType::kBundleShowBadgeType, enable);
    return CommitNotification(paraseJson, preferencesInfo);
}

ErrCode NotificationPreferences::GetImportance(const std::string &bundleName, int &importance)
{
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode result = GetBundleProperty(bundleName, BundlePropertyType::kBundleImportanceType, importance);
    return result;
}

ErrCode NotificationPreferences::SetImportance(const std::string &bundleName, const int importance)
{
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    PreferencesInfo preferencesInfo = preferencesInfo_;
    nlohmann::json paraseJson = paraseJson_;
    SetBundleProperty(preferencesInfo, paraseJson, bundleName, BundlePropertyType::kBundleImportanceType, importance);
    return CommitNotification(paraseJson, preferencesInfo);
}

ErrCode NotificationPreferences::GetTotalBadgeNums(const std::string &bundleName, int &totalBadgeNum)
{
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    ErrCode result = GetBundleProperty(bundleName, BundlePropertyType::kBundleBadgeTotalNumType, totalBadgeNum);
    return result;
}

ErrCode NotificationPreferences::SetTotalBadgeNums(const std::string &bundleName, const int num)
{
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    PreferencesInfo preferencesInfo = preferencesInfo_;
    nlohmann::json paraseJson = paraseJson_;
    SetBundleProperty(preferencesInfo, paraseJson, bundleName, BundlePropertyType::kBundleBadgeTotalNumType, num);
    return CommitNotification(paraseJson, preferencesInfo);
}

ErrCode NotificationPreferences::GetPrivateNotificationsAllowed(const std::string &bundleName, bool &allow)
{
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    ErrCode result = GetBundleProperty(bundleName, BundlePropertyType::kBundlePrivateAllowedType, allow);
    return result;
}

ErrCode NotificationPreferences::SetPrivateNotificationsAllowed(const std::string &bundleName, const bool allow)
{
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    PreferencesInfo preferencesInfo = preferencesInfo_;
    nlohmann::json paraseJson = paraseJson_;
    SetBundleProperty(preferencesInfo, paraseJson, bundleName, BundlePropertyType::kBundlePrivateAllowedType, allow);
    return CommitNotification(paraseJson, preferencesInfo);
}

ErrCode NotificationPreferences::GetNotificationsEnabledForBundle(const std::string &bundleName, bool &enabled)
{
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    ErrCode result = GetBundleProperty(bundleName, BundlePropertyType::kBundleEnableNotificationType, enabled);
    return result;
}

ErrCode NotificationPreferences::SetNotificationsEnabledForBundle(const std::string &bundleName, const bool enabled)
{
    if (bundleName.empty()) {
        return ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }
    PreferencesInfo preferencesInfo = preferencesInfo_;
    nlohmann::json paraseJson = paraseJson_;
    SetBundleProperty(
        preferencesInfo, paraseJson, bundleName, BundlePropertyType::kBundleEnableNotificationType, enabled);
    return CommitNotification(paraseJson, preferencesInfo);
}

ErrCode NotificationPreferences::GetNotificationsEnabled(bool &enabled)
{
    enabled = preferencesInfo_.isEnabledAllNotification;
    return ERR_OK;
}

ErrCode NotificationPreferences::SetNotificationsEnabled(const bool &enabled)
{
    PreferencesInfo preferencesInfo = preferencesInfo_;
    nlohmann::json paraseJson = paraseJson_;
    preferencesInfo.isEnabledAllNotification = enabled;
    paraseJson[kEnable] = enabled;
    return CommitNotification(paraseJson, preferencesInfo);
}

ErrCode NotificationPreferences::GetDisturbMode(NotificationConstant::DisturbMode &mode)
{
    mode = preferencesInfo_.disturbMode;
    return ERR_OK;
}

ErrCode NotificationPreferences::SetDisturbMode(const NotificationConstant::DisturbMode &mode)
{
    PreferencesInfo preferencesInfo = preferencesInfo_;
    nlohmann::json paraseJson = paraseJson_;
    preferencesInfo.disturbMode = mode;
    paraseJson[kDisturbMode] = mode;
    return CommitNotification(paraseJson, preferencesInfo);
}

ErrCode NotificationPreferences::ClearNotificationInRestoreFactorySettings()
{
    PreferencesInfo preferencesInfo = preferencesInfo_;
    nlohmann::json paraseJson = paraseJson_;
    preferencesInfo.disturbMode = NotificationConstant::DisturbMode::ALLOW_ALL;
    preferencesInfo.isEnabledAllNotification = true;
    preferencesInfo.infos.clear();
    paraseJson.clear();
    return CommitNotification(paraseJson, preferencesInfo);
}

ErrCode NotificationPreferences::CreateSlot(
    const std::string &bundleName, const NotificationConstant::SlotType &type, sptr<NotificationSlot> &slot)
{
    sptr<NotificationSlot> tmpSlot;
    switch (type) {
        case NotificationConstant::SlotType::SOCIAL_COMMUNICATION:
            tmpSlot = new NotificationSlot(NotificationConstant::SlotType::SOCIAL_COMMUNICATION);
            break;
        case NotificationConstant::SlotType::SERVICE_REMINDER:
            tmpSlot = new NotificationSlot(NotificationConstant::SlotType::SERVICE_REMINDER);
            break;
        case NotificationConstant::SlotType::CONTENT_INFORMATION:
            tmpSlot = new NotificationSlot(NotificationConstant::SlotType::CONTENT_INFORMATION);
            break;
        case NotificationConstant::SlotType::OTHER:
        case NotificationConstant::SlotType::CUSTOM:
            tmpSlot = new NotificationSlot(NotificationConstant::SlotType::OTHER);
            break;
        default:
            break;
    }
    std::vector<sptr<NotificationSlot>> slots;
    slots.push_back(tmpSlot);
    ErrCode result = AddNotificationSlots(bundleName, slots);
    if (result == ERR_OK) {
        slot = tmpSlot;
    }
    return result;
}

ErrCode NotificationPreferences::CheckSlotForCreateSlot(const std::string &bundleName,
    const sptr<NotificationSlot> &slot, PreferencesInfo &preferencesInfo, nlohmann::json &paraseJson) const
{
    if (slot == nullptr) {
        ANS_LOGE("Notification slot is nullptr.");
        return ERR_ANS_PREFERENCES_NOTIFICATION_SLOT_NOT_EXIST;
    }

    ErrCode result = ERR_OK;
    auto preferencesIter = preferencesInfo.infos.find(bundleName);
    if (preferencesIter != preferencesInfo.infos.end()) {
        if (preferencesIter->second.slots.find(slot->GetType()) != preferencesIter->second.slots.end()) {
            preferencesIter->second.slots.insert_or_assign(slot->GetType(), slot);
            UpdateExsitSlotTypeInJson(paraseJson, bundleName, slot);
        } else {
            preferencesIter->second.slots.insert_or_assign(slot->GetType(), slot);
            AddSlotToJson(paraseJson, bundleName, slot);
        }
    } else {
        BundleInfo bundle;
        bundle.bundleName = bundleName;
        bundle.slots.insert_or_assign(slot->GetType(), slot);
        preferencesInfo.infos.insert_or_assign(bundleName, bundle);
        AddJson(paraseJson, bundle, preferencesInfo.disturbMode, preferencesInfo.isEnabledAllNotification);
    }

    return result;
}

ErrCode NotificationPreferences::CheckGroupForCreateSlotGroup(const std::string &bundleName,
    const sptr<NotificationSlotGroup> &group, PreferencesInfo &preferencesInfo, nlohmann::json &paraseJson) const
{
    if (group == nullptr) {
        ANS_LOGE("Notification slot group is nullptr.");
        return ERR_ANS_INVALID_PARAM;
    }

    if (group->GetId().empty()) {
        ANS_LOGE("Notification slot group id is invalid.");
        return ERR_ANS_PREFERENCES_NOTIFICATION_SLOTGROUP_ID_INVALID;
    }

    ErrCode result = ERR_OK;
    auto preferencesIter = preferencesInfo.infos.find(bundleName);
    if (preferencesIter != preferencesInfo.infos.end()) {
        if (preferencesIter->second.groups.find(group->GetId()) != preferencesIter->second.groups.end()) {
            result = ERR_ANS_PREFERENCES_NOTIFICATION_SLOTGROUP_HAS_EXISTED;
        } else {
            if (preferencesIter->second.groups.size() >= MAX_SLOT_GROUP_NUM) {
                result = ERR_ANS_PREFERENCES_NOTIFICATION_SLOTGROUP_EXCEED_MAX_NUM;
            } else {
                preferencesIter->second.groups.insert_or_assign(group->GetId(), group);
                AddSlotGroupToJson(paraseJson, bundleName, group);
            }
        }
    } else {
        BundleInfo bundle;
        bundle.bundleName = bundleName;
        bundle.groups.insert_or_assign(group->GetId(), group);
        preferencesInfo.infos.insert_or_assign(bundleName, bundle);
        AddJson(paraseJson, bundle, preferencesInfo.disturbMode, preferencesInfo.isEnabledAllNotification);
    }

    return result;
}

ErrCode NotificationPreferences::CheckSlotForRemoveSlot(const std::string &bundleName,
    const NotificationConstant::SlotType &slotType, PreferencesInfo &preferencesInfo, nlohmann::json &paraseJson) const
{
    ErrCode result = ERR_OK;
    auto preferencesIter = preferencesInfo.infos.find(bundleName);
    if (preferencesIter != preferencesInfo.infos.end()) {
        if (preferencesIter->second.slots.empty()) {
            return ERR_ANS_PREFERENCES_NOTIFICATION_SLOT_NOT_EXIST;
        }
        auto slotIter = preferencesIter->second.slots.find(slotType);
        if (slotIter != preferencesIter->second.slots.end()) {
            preferencesIter->second.slots.erase(slotIter);
            RemoveSlotInJson(paraseJson, bundleName, slotType);
        } else {
            ANS_LOGE("Notification slot id is invalid.");
            result = ERR_ANS_PREFERENCES_NOTIFICATION_SLOT_ID_INVALID;
        }
    } else {
        ANS_LOGE("Notification bundle does not exsit.");
        result = ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }
    return result;
}

ErrCode NotificationPreferences::CheckGroupForRemoveSlotGroup(const std::string &bundleName, const std::string &groupId,
    PreferencesInfo &preferencesInfo, nlohmann::json &paraseJson) const
{
    ErrCode result = ERR_OK;
    auto preferencesIter = preferencesInfo.infos.find(bundleName);
    if (preferencesIter != preferencesInfo.infos.end()) {
        if (preferencesIter->second.groups.empty()) {
            return ERR_ANS_PREFERENCES_NOTIFICATION_SLOTGROUP_NOT_EXIST;
        }
        auto groupIter = preferencesIter->second.groups.find(groupId);
        if (groupIter != preferencesIter->second.groups.end()) {
            preferencesIter->second.groups.erase(groupIter);
            RemoveSlotGroupInJson(paraseJson, bundleName, groupId);
        } else {
            ANS_LOGE("Notification slot id is invalid.");
            result = ERR_ANS_PREFERENCES_NOTIFICATION_SLOTGROUP_ID_INVALID;
        }
    } else {
        ANS_LOGE("Notification bundle does not exsit.");
        result = ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }
    return result;
}

ErrCode NotificationPreferences::CheckSlotForUpdateSlot(const std::string &bundleName,
    const sptr<NotificationSlot> &slot, PreferencesInfo &preferencesInfo, nlohmann::json &paraseJson) const
{
    if (slot == nullptr) {
        ANS_LOGE("Notification slot is nullptr.");
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode result = ERR_OK;
    auto preferencesIter = preferencesInfo.infos.find(bundleName);
    if (preferencesIter != preferencesInfo.infos.end()) {
        if (preferencesIter->second.slots.empty()) {
            return ERR_ANS_PREFERENCES_NOTIFICATION_SLOT_NOT_EXIST;
        }
        auto slotIter = preferencesIter->second.slots.find(slot->GetType());
        if (slotIter != preferencesIter->second.slots.end()) {
            preferencesIter->second.slots.insert_or_assign(slot->GetType(), slot);
            UpdateSlotInJson(paraseJson, bundleName, slot);
        } else {
            ANS_LOGE("Notification slot id is invalid.");
            result = ERR_ANS_PREFERENCES_NOTIFICATION_SLOT_ID_INVALID;
        }
    } else {
        ANS_LOGE("Notification bundle does not exsit.");
        result = ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }
    return result;
}

ErrCode NotificationPreferences::CheckGroupForUpdateSlotGroup(const std::string &bundleName,
    const sptr<NotificationSlotGroup> &group, PreferencesInfo &preferencesInfo, nlohmann::json &paraseJson) const
{
    ErrCode result = ERR_OK;
    auto preferencesIter = preferencesInfo.infos.find(bundleName);
    if (preferencesIter != preferencesInfo.infos.end()) {
        if (preferencesIter->second.groups.empty()) {
            return ERR_ANS_PREFERENCES_NOTIFICATION_SLOTGROUP_NOT_EXIST;
        }
        auto groupIter = preferencesIter->second.groups.find(group->GetId());
        if (groupIter != preferencesIter->second.groups.end()) {
            preferencesIter->second.groups.insert_or_assign(group->GetId(), group);
            UpdateSlotGroupInJson(paraseJson, bundleName, group);
        } else {
            ANS_LOGE("Notification slot group id is invalid.");
            result = ERR_ANS_PREFERENCES_NOTIFICATION_SLOTGROUP_ID_INVALID;
        }
    } else {
        ANS_LOGE("Notification bundle does not exsit.");
        result = ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }
    return result;
}

template <typename T>
void NotificationPreferences::SetBundleProperty(PreferencesInfo &preferencesInfo, nlohmann::json &paraseJson,
    const std::string &bundleName, const BundlePropertyType &type, const T &value)
{
    auto preferencesIter = preferencesInfo.infos.find(bundleName);
    if (preferencesIter != preferencesInfo.infos.end()) {
        UpdateBundleProperty(preferencesIter->second, paraseJson, bundleName, type, value);
    } else {
        CreateBundleProperty(preferencesInfo, paraseJson, bundleName, type, value);
    }
}

template <typename T>
void NotificationPreferences::CreateBundleProperty(PreferencesInfo &preferencesInfo, nlohmann::json &paraseJson,
    const std::string &bundleName, const BundlePropertyType &type, const T &value)
{
    BundleInfo bundle;
    bundle.bundleName = bundleName;
    switch (type) {
        case BundlePropertyType::kBundleImportanceType:
            bundle.importance = value;
            break;
        case BundlePropertyType::kBundleBadgeTotalNumType:
            bundle.badgeTotalNum = value;
            break;
        case BundlePropertyType::kBundleShowBadgeType:
            bundle.isShowBadge = value;
            break;
        case BundlePropertyType::kBundlePrivateAllowedType:
            bundle.isPrivateAllowed = value;
            break;
        case BundlePropertyType::kBundleEnableNotificationType:
            bundle.isEnabledNotification = value;
            break;
        default:
            break;
    }
    preferencesInfo.infos.emplace(bundleName, bundle);
    AddJson(paraseJson, bundle, preferencesInfo.disturbMode, preferencesInfo.isEnabledAllNotification);
}

template <typename T>
void NotificationPreferences::UpdateBundleProperty(BundleInfo &bundleInfo, nlohmann::json &paraseJson,
    const std::string &bundleName, const BundlePropertyType &type, const T &value)
{
    auto packageJsonIter = paraseJson[kBundle].begin();
    while (packageJsonIter != paraseJson[kBundle].end()) {
        if (!bundleName.compare((*packageJsonIter)[kBundleName].get<std::string>())) {
            break;
        }
        ++packageJsonIter;
    }
    switch (type) {
        case BundlePropertyType::kBundleImportanceType:
            bundleInfo.importance = value;
            packageJsonIter->at(kBundleImportance) = value;
            break;
        case BundlePropertyType::kBundleBadgeTotalNumType:
            bundleInfo.badgeTotalNum = value;
            packageJsonIter->at(kBundleBadgeTotalNum) = value;
            break;
        case BundlePropertyType::kBundleShowBadgeType:
            bundleInfo.isShowBadge = value;
            packageJsonIter->at(kBundleShowBadge) = value;
            break;
        case BundlePropertyType::kBundlePrivateAllowedType:
            bundleInfo.isPrivateAllowed = value;
            packageJsonIter->at(kBundlePrivateAllowed) = value;
            break;
        case BundlePropertyType::kBundleEnableNotificationType:
            bundleInfo.isEnabledNotification = value;
            packageJsonIter->at(kBundleEnableNotification) = value;
            break;
        default:
            break;
    }
}

template <typename T>
ErrCode NotificationPreferences::GetBundleProperty(
    const std::string &bundleName, const BundlePropertyType &type, T &value)
{
    ErrCode result = ERR_OK;
    auto preferencesIter = preferencesInfo_.infos.find(bundleName);
    if (preferencesIter != preferencesInfo_.infos.end()) {
        switch (type) {
            case BundlePropertyType::kBundleImportanceType:
                value = preferencesIter->second.importance;
                break;
            case BundlePropertyType::kBundleBadgeTotalNumType:
                value = preferencesIter->second.badgeTotalNum;
                break;
            case BundlePropertyType::kBundleShowBadgeType:
                value = preferencesIter->second.isShowBadge;
                break;
            case BundlePropertyType::kBundlePrivateAllowedType:
                value = preferencesIter->second.isPrivateAllowed;
                break;
            case BundlePropertyType::kBundleEnableNotificationType:
                value = preferencesIter->second.isEnabledNotification;
                break;
            default:
                result = ERR_ANS_INVALID_PARAM;
                break;
        }
    } else {
        ANS_LOGE("Notification bundle does not exsit.");
        result = ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }
    return result;
}

void NotificationPreferences::AddSlotToJson(
    nlohmann::json &json, const std::string &bundleName, const sptr<NotificationSlot> &slot) const
{
    auto packageJsonIter = json[kBundle].begin();
    while (packageJsonIter != json[kBundle].end()) {
        if (!bundleName.compare((*packageJsonIter)[kBundleName].get<std::string>())) {
            (*packageJsonIter)[kSlot].emplace_back(SlotToJson(slot));
            break;
        }
        ++packageJsonIter;
    }
}

void NotificationPreferences::AddSlotGroupToJson(
    nlohmann::json &json, const std::string &bundleName, const sptr<NotificationSlotGroup> &group) const
{
    auto packageJsonIter = json[kBundle].begin();
    while (packageJsonIter != json[kBundle].end()) {
        if (!bundleName.compare((*packageJsonIter)[kBundleName].get<std::string>())) {
            (*packageJsonIter)[kGroup].emplace_back(GroupToJson(group));
            break;
        }
        ++packageJsonIter;
    }
}

void NotificationPreferences::UpdateExsitSlotTypeInJson(
    nlohmann::json &json, const std::string &bundleName, const sptr<NotificationSlot> &slot) const
{
    for (auto &packageJsonIter : json[kBundle]) {
        if (bundleName.compare(packageJsonIter[kBundleName].get<std::string>())) {
            continue;
        }

        for (auto &slotJsonIter : packageJsonIter[kSlot]) {
            if (slot->GetType() == slotJsonIter[kSlotType].get<NotificationConstant::SlotType>()) {
                slotJsonIter[kSlotId] = slot->GetId();
                slotJsonIter[kSlotName] = slot->GetName();
                slotJsonIter[kSlotDescription] = slot->GetDescription();
                break;
            }
        }
    }
}

void NotificationPreferences::RemoveSlotInJson(
    nlohmann::json &json, const std::string &bundleName, const NotificationConstant::SlotType &type) const
{
    for (auto &packageJsonIter : json[kBundle]) {
        if (bundleName.compare(packageJsonIter[kBundleName].get<std::string>())) {
            continue;
        }

        auto slotJsonIter = packageJsonIter[kSlot].begin();
        while (slotJsonIter != packageJsonIter[kSlot].end()) {
            if (type == (*slotJsonIter)[kSlotType].get<NotificationConstant::SlotType>()) {
                packageJsonIter[kSlot].erase(slotJsonIter);
                break;
            }
            ++slotJsonIter;
        }
    }
}

void NotificationPreferences::UpdateSlotInJson(
    nlohmann::json &json, const std::string &bundleName, const sptr<NotificationSlot> &slot) const
{
    for (auto &packageJsonIter : json[kBundle]) {
        if (bundleName.compare(packageJsonIter[kBundleName].get<std::string>())) {
            continue;
        }

        auto slotJsonIter = packageJsonIter[kSlot].begin();
        while (slotJsonIter != packageJsonIter[kSlot].end()) {
            if (!slot->GetId().compare((*slotJsonIter)[kSlotId].get<std::string>())) {
                nlohmann::json tempSlotJson = SlotToJson(slot);
                (*slotJsonIter).update(tempSlotJson.begin(), tempSlotJson.end());
                break;
            }
            ++slotJsonIter;
        }
    }
}

void NotificationPreferences::RemoveSlotGroupInJson(
    nlohmann::json &json, const std::string &bundleName, const std::string &groupId) const
{
    for (auto &packageJsonIter : json[kBundle]) {
        if (bundleName.compare(packageJsonIter[kBundleName].get<std::string>())) {
            continue;
        }

        auto groupJsonIter = packageJsonIter[kGroup].begin();
        while (groupJsonIter != packageJsonIter[kGroup].end()) {
            if (!groupId.compare((*groupJsonIter)[kGroupId].get<std::string>())) {
                packageJsonIter[kGroup].erase(groupJsonIter);
                break;
            }
            ++groupJsonIter;
        }
    }
}

void NotificationPreferences::UpdateSlotGroupInJson(
    nlohmann::json &json, const std::string &bundleName, const sptr<NotificationSlotGroup> &group) const
{
    for (auto &packageJsonIter : json[kBundle]) {
        if (bundleName.compare(packageJsonIter[kBundleName].get<std::string>())) {
            continue;
        }

        auto groupJsonIter = packageJsonIter[kGroup].begin();
        while (groupJsonIter != packageJsonIter[kGroup].end()) {
            if (!group->GetId().compare((*groupJsonIter)[kGroupId].get<std::string>())) {
                nlohmann::json tempGroupJson = GroupToJson(group);
                (*groupJsonIter).update(tempGroupJson.begin(), tempGroupJson.end());
                break;
            }
            ++groupJsonIter;
        }
    }
}

void NotificationPreferences::RemoveBundleInJson(nlohmann::json &json, const std::string &bundleName)
{
    auto packageJsonIter = json[kBundle].begin();
    while (packageJsonIter != json[kBundle].end()) {
        if (!bundleName.compare((*packageJsonIter)[kBundleName].get<std::string>())) {
            json[kBundle].erase(packageJsonIter);
            break;
        }
        ++packageJsonIter;
    }
}

void NotificationPreferences::AddJson(nlohmann::json &json, const BundleInfo &info,
    const NotificationConstant::DisturbMode &mode, const bool &enable) const
{
    nlohmann::json preferencesJson = BundlePropertyToJson(info);
    if (!info.groups.empty()) {
        for (auto groupIter : info.groups) {
            preferencesJson[kGroup].emplace_back(GroupToJson(groupIter.second));
        }
    }

    if (!info.slots.empty()) {
        for (auto slotIter : info.slots) {
            preferencesJson[kSlot].emplace_back(SlotToJson(slotIter.second));
        }
    }
    json[kBundle].emplace_back(preferencesJson);

    if (json[kDisturbMode].is_null()) {
        json[kDisturbMode] = mode;
    }

    if (json[kEnable].is_null()) {
        json[kEnable] = enable;
    }
}

void NotificationPreferences::ParseFromJson()
{
    if (paraseJson_.empty() || !paraseJson_.contains(kBundle)) {
        ANS_LOGE("Current file is empty.");
        return;
    }
    preferencesInfo_.disturbMode = paraseJson_[kDisturbMode].get<NotificationConstant::DisturbMode>();
    preferencesInfo_.isEnabledAllNotification = paraseJson_[kEnable].get<bool>();
    for (auto packageIter : paraseJson_[kBundle]) {
        BundleInfo bundleInfo;
        JsonToBundleProperty(packageIter, bundleInfo);
        preferencesInfo_.infos.emplace(std::make_pair(bundleInfo.bundleName, bundleInfo));
        for (auto groupIter : packageIter[kGroup]) {
            sptr<NotificationSlotGroup> group = new NotificationSlotGroup(
                groupIter[kGroupId].get<std::string>(), groupIter[kGroupName].get<std::string>());
            JsonToGroup(groupIter, group);
            preferencesInfo_.infos.at(bundleInfo.bundleName).groups.emplace(std::make_pair(group->GetId(), group));
        }

        for (auto &slotIter : packageIter[kSlot]) {
            sptr<NotificationSlot> slot =
                new NotificationSlot(slotIter[kSlotType].get<NotificationConstant::SlotType>());
            JsonToSlot(slotIter, slot);
            preferencesInfo_.infos.at(bundleInfo.bundleName).slots.emplace(std::make_pair(slot->GetType(), slot));
        }
    }
}

ErrCode NotificationPreferences::ParseNotification()
{
    std::ifstream inputFile(kStoreFile, std::ifstream::in);
    CheckInvalidPath(inputFile);
    if (IsEmptyInputFile(inputFile)) {
        return ERR_ANS_PREFERENCES_FILE_IS_NULL;
    }

    ErrCode result = ERR_OK;
    try {
        paraseJson_ = nlohmann::json::parse(inputFile);
        inputFile.close();
    } catch (const nlohmann::detail::exception &e) {
        ANS_LOGE("Exception file error is: %{public}s", e.what());
        paraseJson_.clear();
        result = ERR_ANS_PREFERENCES_FILE_EXCEPTION;
    }
    if (result == ERR_OK) {
        ParseFromJson();
    }
    return result;
}

ErrCode NotificationPreferences::StoreNotification(const nlohmann::json &json) const
{
    std::ofstream outFile(kStoreFile, std::ofstream::out);
    CheckInvalidPath(outFile);

    outFile.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    ErrCode result = ERR_OK;
    try {
        outFile << json.dump(kJsonDump);
        outFile.flush();
        outFile.close();
    } catch (std::ofstream::failure e) {
        ANS_LOGE("Exception file error is: %{public}s", e.what());
        result = ERR_ANS_PREFERENCES_FILE_EXCEPTION;
    }
    return result;
}

ErrCode NotificationPreferences::CommitNotification(
    const nlohmann::json &paraseJson, const PreferencesInfo &preferencesInfo)
{
    ErrCode result = StoreNotification(paraseJson);

    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
        paraseJson_ = paraseJson;
    }
    return result;
}

template <typename T>
void NotificationPreferences::CheckInvalidPath(T &t) const
{
    if (!t.is_open()) {
        ANS_LOGI("File does not open, create a new dir");
        InitDir();
        InitFile();
    }
}

bool NotificationPreferences::IsEmptyInputFile(std::ifstream &inputFile) const
{
    inputFile.seekg(0, inputFile.end);
    if (0 == inputFile.tellg()) {
        ANS_LOGI("InputFile is empty, does not parase json file.");
        return true;
    }
    inputFile.seekg(0, inputFile.beg);
    return false;
}

void NotificationPreferences::InitDir() const
{
    if (IsExistDir(kStorePath)) {
        ANS_LOGI("Path already exists.");
        return;
    }

    if (!CreateDirectory(kStorePath)) {
        ANS_LOGE("create path failed %{public}s", strerror(errno));
    }
}

void NotificationPreferences::InitFile() const
{
    if (IsExistFile(kStorePath, kStoreFile)) {
        ANS_LOGI("File already exists.");
        return;
    }

    if (!CreateAndChangeFileAttr(kStoreFile)) {
        ANS_LOGE("Change file attr failed %{public}s", strerror(errno));
    }
}

bool NotificationPreferences::IsExistDir(const std::string &path) const
{
    struct stat buf = {};
    if (stat(path.c_str(), &buf) != 0) {
        return false;
    }
    return S_ISDIR(buf.st_mode);
}

bool NotificationPreferences::IsExistFile(const std::string &dirPath, const std::string &filePath) const
{
    std::vector<std::string> files;
    OHOS::GetDirFiles(dirPath, files);
    auto iter =
        std::find_if(files.begin(), files.end(), [&filePath](std::string &path) { return !path.compare(filePath); });

    if (iter != files.end()) {
        return true;
    }
    return false;
}

bool NotificationPreferences::CreateDirectory(const std::string &path) const
{
    if (!OHOS::ForceCreateDirectory(path)) {
        ANS_LOGE("mkdir failed");
        return false;
    }
    mode_t mode = S_IRWXU | S_IRGRP;
    return OHOS::ChangeModeDirectory(path, mode);
}

bool NotificationPreferences::CreateAndChangeFileAttr(const std::string &filePath) const
{
    std::ofstream outFile(filePath, std::ofstream::out);
    if (outFile.is_open()) {
        outFile.clear();
        outFile.close();
        ANS_LOGI("Create a json file.");
    }
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP;
    return OHOS::ChangeModeFile(filePath, mode);
}

nlohmann::json NotificationPreferences::BundlePropertyToJson(const BundleInfo &bundleInfo) const
{
    return nlohmann::json{{kBundleName, bundleInfo.bundleName},
        {kBundleImportance, bundleInfo.importance},
        {kBundleShowBadge, bundleInfo.isShowBadge},
        {kBundleBadgeTotalNum, bundleInfo.badgeTotalNum},
        {kBundlePrivateAllowed, bundleInfo.isPrivateAllowed},
        {kBundleEnableNotification, bundleInfo.isEnabledNotification}};
}

nlohmann::json NotificationPreferences::GroupToJson(const sptr<NotificationSlotGroup> &group) const
{
    return nlohmann::json{{kGroupId, group->GetId()},
        {kGroupName, group->GetName()},
        {kGroupDescription, group->GetDescription()},
        {kGroupDisable, group->IsDisabled()}};
}

nlohmann::json NotificationPreferences::SlotToJson(const sptr<NotificationSlot> &slot) const
{
    return nlohmann::json{{kSlotGroupId, slot->GetSlotGroup()},
        {kSlotId, slot->GetId()},
        {kSlotName, slot->GetName()},
        {kSlotDescription, slot->GetDescription()},
        {kSlotLevel, slot->GetLevel()},
        {kSlotShowBadge, slot->IsShowBadge()},
        {kSlotEnableLight, slot->CanEnableLight()},
        {kSlotEnableVrbration, slot->CanVibrate()},
        {kSlotLedLightColor, slot->GetLedLightColor()},
        {kSlotLockscreenVisibleness, slot->GetLockScreenVisibleness()},
        {kSlotSound, slot->GetSound().ToString()},
        {kSlotVibrationSytle, slot->GetVibrationStyle()},
        {kSlotEnableBypassDnd, slot->IsEnableBypassDnd()},
        {kSlotType, slot->GetType()}};
}

void NotificationPreferences::JsonToBundleProperty(nlohmann::json &json, BundleInfo &bundleInfo) const
{
    json.at(kBundleName).get_to(bundleInfo.bundleName);
    json.at(kBundleImportance).get_to(bundleInfo.importance);
    json.at(kBundleShowBadge).get_to(bundleInfo.isShowBadge);
    json.at(kBundleBadgeTotalNum).get_to(bundleInfo.badgeTotalNum);
    json.at(kBundlePrivateAllowed).get_to(bundleInfo.isPrivateAllowed);
    json.at(kBundleEnableNotification).get_to(bundleInfo.isEnabledNotification);
}

void NotificationPreferences::JsonToGroup(nlohmann::json &json, sptr<NotificationSlotGroup> &group) const
{
    group->SetDescription(json[kGroupDescription].get<std::string>());
}

void NotificationPreferences::JsonToSlot(nlohmann::json &json, sptr<NotificationSlot> &slot) const
{
    slot->SetSlotGroup(json[kSlotGroupId].get<std::string>());
    slot->SetDescription(json[kSlotDescription].get<std::string>());
    slot->SetLevel(json[kSlotLevel].get<NotificationSlot::NotificationLevel>());
    slot->EnableBadge(json[kSlotShowBadge].get<bool>());
    slot->SetEnableLight(json[kSlotEnableLight].get<bool>());
    slot->SetLedLightColor(json[kSlotLedLightColor].get<int32_t>());
    slot->SetLockscreenVisibleness(json[kSlotLockscreenVisibleness].get<NotificationConstant::VisiblenessType>());
    Uri uri(json[kSlotSound].get<std::string>());
    slot->SetSound(uri);
    slot->SetVibrationStyle(json[kSlotVibrationSytle].get<std::vector<int64_t>>());
    slot->EnableBypassDnd(json[kSlotEnableBypassDnd].get<bool>());
    slot->SetType(json[kSlotType].get<NotificationConstant::SlotType>());
}

}  // namespace Notification
}  // namespace OHOS