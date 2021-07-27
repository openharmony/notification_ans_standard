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

#include "ans_const_define.h"
#include "ans_inner_errors.h"
#include "ans_log_wrapper.h"

namespace OHOS {
namespace Notification {

NotificationPreferences::NotificationPreferences()
{
    preferncesDB_ = std::make_unique<NotificationPreferencesDatabase>();
    preferncesDB_->ParseFromDisturbeDB(preferencesInfo_);
}

NotificationPreferences::~NotificationPreferences()
{}

NotificationPreferences &NotificationPreferences::GetInstance()
{
    return DelayedRefSingleton<NotificationPreferences>::GetInstance();
}

ErrCode NotificationPreferences::AddNotificationSlots(
    const std::string &bundleName, const std::vector<sptr<NotificationSlot>> &slots)
{

    ANS_LOGD("%{public}s", __FUNCTION__);
    if (bundleName.empty() || slots.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    ErrCode result = ERR_OK;
    for (auto slot : slots) {
        result = CheckSlotForCreateSlot(bundleName, slot, preferencesInfo);
        if (result != ERR_OK) {
            return result;
        }
    }

    if (result == ERR_OK && (!preferncesDB_->PutSlotsToDisturbeDB(bundleName, slots))) {
        return ERR_ANS_PREFERENCES_NOTIFICATION_DB_OPERATION_FAILED;
    }

    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }
    return result;
}

ErrCode NotificationPreferences::AddNotificationSlotGroups(
    const std::string &bundleName, const std::vector<sptr<NotificationSlotGroup>> &groups)
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    if (bundleName.empty() || groups.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    ErrCode result = ERR_OK;
    for (auto group : groups) {
        result = CheckGroupForCreateSlotGroup(bundleName, group, preferencesInfo);
        if (result != ERR_OK) {
            return result;
        }
    }

    if (result == ERR_OK && (!preferncesDB_->PutGroupsToDisturbeDB(bundleName, groups))) {
        result = ERR_ANS_PREFERENCES_NOTIFICATION_DB_OPERATION_FAILED;
        return result;
    }

    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }
    return result;
}

ErrCode NotificationPreferences::RemoveNotificationSlot(
    const std::string &bundleName, const NotificationConstant::SlotType &slotType)
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    ErrCode result = ERR_OK;
    result = CheckSlotForRemoveSlot(bundleName, slotType, preferencesInfo);

    if (result == ERR_OK && (!preferncesDB_->RemoveSlotFromDisturbeDB(bundleName, slotType))) {
        result = ERR_ANS_PREFERENCES_NOTIFICATION_DB_OPERATION_FAILED;
        return result;
    }

    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }
    return result;
}

ErrCode NotificationPreferences::RemoveNotificationSlotGroups(
    const std::string &bundleName, const std::vector<std::string> &groupIds)
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    if (bundleName.empty() || groupIds.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    ErrCode result = ERR_OK;
    for (auto groupId : groupIds) {
        result = CheckGroupForRemoveSlotGroup(bundleName, groupId, preferencesInfo);
        if (result != ERR_OK) {
            return result;
        }
    }
    if (result == ERR_OK && (!preferncesDB_->RemoveGroupsFromDisturbeDB(bundleName, groupIds))) {
        result = ERR_ANS_PREFERENCES_NOTIFICATION_DB_OPERATION_FAILED;
        return result;
    }

    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }
    return result;
}

ErrCode NotificationPreferences::RemoveNotificationForBundle(const std::string &bundleName)
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;

    ErrCode result = ERR_OK;
    if (preferencesInfo.IsExsitBundleInfo(bundleName)) {
        preferencesInfo.RemoveBundleInfo(bundleName);
        if (!preferncesDB_->RemoveBundleFromDisturbeDB(bundleName)) {
            result = ERR_ANS_PREFERENCES_NOTIFICATION_DB_OPERATION_FAILED;
        }
    } else {
        result = ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }

    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }

    return result;
}

ErrCode NotificationPreferences::UpdateNotificationSlots(
    const std::string &bundleName, const std::vector<sptr<NotificationSlot>> &slots)
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    if (bundleName.empty() || slots.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    ErrCode result = ERR_OK;
    for (auto slotIter : slots) {
        result = CheckSlotForUpdateSlot(bundleName, slotIter, preferencesInfo);
        if (result != ERR_OK) {
            return result;
        }
    }

    if (result == ERR_OK && (!preferncesDB_->PutSlotsToDisturbeDB(bundleName, slots))) {
        result = ERR_ANS_PREFERENCES_NOTIFICATION_DB_OPERATION_FAILED;
        return result;
    }

    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }

    return result;
}

ErrCode NotificationPreferences::UpdateNotificationSlotGroups(
    const std::string &bundleName, const std::vector<sptr<NotificationSlotGroup>> &groups)
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    if (bundleName.empty() || groups.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    ErrCode result = ERR_OK;
    for (auto groupIter : groups) {
        result = CheckGroupForUpdateSlotGroup(bundleName, groupIter, preferencesInfo);
        if (result != ERR_OK) {
            return result;
        }
    }

    if (result == ERR_OK && (!preferncesDB_->PutGroupsToDisturbeDB(bundleName, groups))) {
        result = ERR_ANS_PREFERENCES_NOTIFICATION_DB_OPERATION_FAILED;
        return result;
    }

    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }
    return result;
}

ErrCode NotificationPreferences::GetNotificationSlot(
    const std::string &bundleName, const NotificationConstant::SlotType &type, sptr<NotificationSlot> &slot)
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode result = ERR_OK;
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (preferencesInfo_.GetBundleInfo(bundleName, bundleInfo)) {
        if (!bundleInfo.GetSlot(type, slot)) {
            result = ERR_ANS_PREFERENCES_NOTIFICATION_SLOT_TYPE_NOT_EXIST;
        }
    } else {
        result = ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }
    ANS_LOGD("%{public}s status  = %{public}d ", __FUNCTION__, result);
    return result;
}

ErrCode NotificationPreferences::GetNotificationAllSlots(
    const std::string &bundleName, std::vector<sptr<NotificationSlot>> &slots)
{
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode result = ERR_OK;
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (preferencesInfo_.GetBundleInfo(bundleName, bundleInfo)) {
        bundleInfo.GetAllSlots(slots);
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
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (preferencesInfo_.GetBundleInfo(bundleName, bundleInfo)) {
        if (!bundleInfo.GetGroup(groupId, group)) {
            result = ERR_ANS_PREFERENCES_NOTIFICATION_SLOTGROUP_NOT_EXIST;
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
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (preferencesInfo_.GetBundleInfo(bundleName, bundleInfo)) {
        bundleInfo.GetAllGroups(groups);
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
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (preferencesInfo_.GetBundleInfo(bundleName, bundleInfo)) {
        bundleInfo.GetAllSlotsInGroup(groupId, slots);
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
    result = GetBundleProperty(bundleName, BundleType::kBundleShowBadgeType, enable);
    return result;
}

ErrCode NotificationPreferences::SetShowBadge(const std::string &bundleName, const bool enable)
{
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    ErrCode result = SetBundleProperty(preferencesInfo, bundleName, BundleType::kBundleShowBadgeType, enable);
    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }
    return result;
}

ErrCode NotificationPreferences::GetImportance(const std::string &bundleName, int &importance)
{
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode result = GetBundleProperty(bundleName, BundleType::kBundleImportanceType, importance);
    return result;
}

ErrCode NotificationPreferences::SetImportance(const std::string &bundleName, const int &importance)
{
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    ErrCode result = SetBundleProperty(preferencesInfo, bundleName, BundleType::kBundleImportanceType, importance);
    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }
    return result;
}

ErrCode NotificationPreferences::GetTotalBadgeNums(const std::string &bundleName, int &totalBadgeNum)
{
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    ErrCode result = GetBundleProperty(bundleName, BundleType::kBundleBadgeTotalNumType, totalBadgeNum);
    return result;
}

ErrCode NotificationPreferences::SetTotalBadgeNums(const std::string &bundleName, const int num)
{
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    ErrCode result = SetBundleProperty(preferencesInfo, bundleName, BundleType::kBundleBadgeTotalNumType, num);
    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }
    return result;
}

ErrCode NotificationPreferences::GetPrivateNotificationsAllowed(const std::string &bundleName, bool &allow)
{
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    ErrCode result = GetBundleProperty(bundleName, BundleType::kBundlePrivateAllowedType, allow);
    return result;
}

ErrCode NotificationPreferences::SetPrivateNotificationsAllowed(const std::string &bundleName, const bool allow)
{
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    ErrCode result = SetBundleProperty(preferencesInfo, bundleName, BundleType::kBundlePrivateAllowedType, allow);
    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }
    return result;
}

ErrCode NotificationPreferences::GetNotificationsEnabledForBundle(const std::string &bundleName, bool &enabled)
{
    if (bundleName.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    ErrCode result = GetBundleProperty(bundleName, BundleType::kBundleEnableNotificationType, enabled);
    return result;
}

ErrCode NotificationPreferences::SetNotificationsEnabledForBundle(const std::string &bundleName, const bool enabled)
{
    if (bundleName.empty()) {
        return ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }
    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    ErrCode result = SetBundleProperty(preferencesInfo, bundleName, BundleType::kBundleEnableNotificationType, enabled);

    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }
    return result;
}

ErrCode NotificationPreferences::GetNotificationsEnabled(bool &enabled)
{
    enabled = preferencesInfo_.GetEnabledAllNotification();
    return ERR_OK;
}

ErrCode NotificationPreferences::SetNotificationsEnabled(const bool &enabled)
{
    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    preferencesInfo.SetEnabledAllNotification(enabled);
    ErrCode result = ERR_OK;
    if (!preferncesDB_->PutNotificationsEnabled(enabled)) {
        result = ERR_ANS_PREFERENCES_NOTIFICATION_DB_OPERATION_FAILED;
    }
    return result;
}

ErrCode NotificationPreferences::GetDisturbMode(NotificationConstant::DisturbMode &mode)
{
    mode = preferencesInfo_.GetDisturbMode();
    return ERR_OK;
}

ErrCode NotificationPreferences::SetDisturbMode(const NotificationConstant::DisturbMode &mode)
{
    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    preferencesInfo.SetDisturbMode(mode);
    ErrCode result = ERR_OK;
    if (!preferncesDB_->PutDisturbMode(mode)) {
        result = ERR_ANS_PREFERENCES_NOTIFICATION_DB_OPERATION_FAILED;
    }
    return result;
}

ErrCode NotificationPreferences::ClearNotificationInRestoreFactorySettings()
{
    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    preferencesInfo.ClearBundleInfo();
    ErrCode result = ERR_OK;
    if (!preferncesDB_->RemoveAllBundleFromDisturbeDB()) {
        result = ERR_ANS_PREFERENCES_NOTIFICATION_DB_OPERATION_FAILED;
    }
    return result;
}

ErrCode NotificationPreferences::CheckSlotForCreateSlot(const std::string &bundleName,
    const sptr<NotificationSlot> &slot, NotificationPreferencesInfo &preferencesInfo) const
{
    if (slot == nullptr) {
        ANS_LOGE("Notification slot is nullptr.");
        return ERR_ANS_PREFERENCES_NOTIFICATION_SLOT_NOT_EXIST;
    }

    ErrCode result = ERR_OK;
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (!preferencesInfo.GetBundleInfo(bundleName, bundleInfo)) {
        bundleInfo.SetBundleName(bundleName);
    }
    bundleInfo.SetSlot(slot);
    preferencesInfo.SetBundleInfo(bundleInfo);

    return result;
}

ErrCode NotificationPreferences::CheckGroupForCreateSlotGroup(const std::string &bundleName,
    const sptr<NotificationSlotGroup> &group, NotificationPreferencesInfo &preferencesInfo) const
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
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (!preferencesInfo.GetBundleInfo(bundleName, bundleInfo)) {
        bundleInfo.SetBundleName(bundleName);
    } else {
        if (bundleInfo.GetGroupSize() >= MAX_SLOT_GROUP_NUM) {
            result = ERR_ANS_PREFERENCES_NOTIFICATION_SLOTGROUP_EXCEED_MAX_NUM;
            return result;
        }
    }

    bundleInfo.SetGroup(group);
    preferencesInfo.SetBundleInfo(bundleInfo);

    return result;
}

ErrCode NotificationPreferences::CheckSlotForRemoveSlot(const std::string &bundleName,
    const NotificationConstant::SlotType &slotType, NotificationPreferencesInfo &preferencesInfo) const
{
    ErrCode result = ERR_OK;
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (preferencesInfo.GetBundleInfo(bundleName, bundleInfo)) {
        if (bundleInfo.IsExsitSlot(slotType)) {
            bundleInfo.RemoveSlot(slotType);
            preferencesInfo.SetBundleInfo(bundleInfo);
        } else {
            ANS_LOGE("Notification slot type does not exsited.");
            result = ERR_ANS_PREFERENCES_NOTIFICATION_SLOT_TYPE_NOT_EXIST;
        }
    } else {
        ANS_LOGE("Notification bundle does not exsit.");
        result = ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }
    return result;
}

ErrCode NotificationPreferences::CheckGroupForRemoveSlotGroup(
    const std::string &bundleName, const std::string &groupId, NotificationPreferencesInfo &preferencesInfo) const
{
    ErrCode result = ERR_OK;
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (preferencesInfo.GetBundleInfo(bundleName, bundleInfo)) {
        if (bundleInfo.IsExsitSlotGroup(groupId)) {
            bundleInfo.RemoveSlotGroup(groupId);
            preferencesInfo.SetBundleInfo(bundleInfo);
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

ErrCode NotificationPreferences::CheckSlotForUpdateSlot(const std::string &bundleName,
    const sptr<NotificationSlot> &slot, NotificationPreferencesInfo &preferencesInfo) const
{
    if (slot == nullptr) {
        ANS_LOGE("Notification slot is nullptr.");
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode result = ERR_OK;
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (preferencesInfo.GetBundleInfo(bundleName, bundleInfo)) {
        if (bundleInfo.IsExsitSlot(slot->GetType())) {
            bundleInfo.SetBundleName(bundleName);
            bundleInfo.SetSlot(slot);
            preferencesInfo.SetBundleInfo(bundleInfo);
        } else {
            result = ERR_ANS_PREFERENCES_NOTIFICATION_SLOT_TYPE_NOT_EXIST;
        }
    } else {
        ANS_LOGE("Notification slot is nullptr.");
        return ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }

    return result;
}

ErrCode NotificationPreferences::CheckGroupForUpdateSlotGroup(const std::string &bundleName,
    const sptr<NotificationSlotGroup> &group, NotificationPreferencesInfo &preferencesInfo) const
{
    ErrCode result = ERR_OK;
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (preferencesInfo.GetBundleInfo(bundleName, bundleInfo)) {
        if (bundleInfo.IsExsitSlotGroup(group->GetId())) {
            bundleInfo.SetBundleName(bundleName);
            bundleInfo.SetGroup(group);
            preferencesInfo.SetBundleInfo(bundleInfo);
        } else {
            result = ERR_ANS_PREFERENCES_NOTIFICATION_SLOTGROUP_NOT_EXIST;
        }
    } else {
        ANS_LOGE("Notification slot is nullptr.");
        return ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }
    return result;
}

template <typename T>
ErrCode NotificationPreferences::SetBundleProperty(
    NotificationPreferencesInfo &preferencesInfo, const std::string &bundleName, const BundleType &type, const T &value)
{
    ErrCode result = ERR_OK;
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (!preferencesInfo_.GetBundleInfo(bundleName, bundleInfo)) {
        bundleInfo.SetBundleName(bundleName);
    }

    result = SaveBundleProperty(bundleInfo, bundleName, type, value);
    preferencesInfo.SetBundleInfo(bundleInfo);

    return result;
}

template <typename T>
ErrCode NotificationPreferences::SaveBundleProperty(NotificationPreferencesInfo::BundleInfo &bundleInfo,
    const std::string &bundleName, const BundleType &type, const T &value)
{
    bool storeDBResult = true;
    switch (type) {
        case BundleType::kBundleImportanceType:
            bundleInfo.SetImportance(value);
            storeDBResult = preferncesDB_->PutImportance(bundleName, value);
            break;
        case BundleType::kBundleBadgeTotalNumType:
            bundleInfo.SetBadgeTotalNum(value);
            storeDBResult = preferncesDB_->PutTotalBadgeNums(bundleName, value);
            break;
        case BundleType::kBundleShowBadgeType:
            bundleInfo.SetIsShowBadge(value);
            storeDBResult = preferncesDB_->PutShowBadge(bundleName, value);
            break;
        case BundleType::kBundlePrivateAllowedType:
            bundleInfo.SetIsPrivateAllowed(value);
            storeDBResult = preferncesDB_->PutPrivateNotificationsAllowed(bundleName, value);
            break;
        case BundleType::kBundleEnableNotificationType:
            bundleInfo.SetEnableNotification(value);
            storeDBResult = preferncesDB_->PutNotificationsEnabledForBundle(bundleName, value);
            break;
        default:
            break;
    }
    ErrCode result = ERR_OK;
    if (!storeDBResult) {
        result = ERR_ANS_PREFERENCES_NOTIFICATION_DB_OPERATION_FAILED;
    }
    return result;
}

template <typename T>
ErrCode NotificationPreferences::GetBundleProperty(
    const std::string &bundleName, const BundleType &type, T &value) const
{
    ErrCode result = ERR_OK;
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (preferencesInfo_.GetBundleInfo(bundleName, bundleInfo)) {
        switch (type) {
            case BundleType::kBundleImportanceType:
                value = bundleInfo.GetImportance();
                break;
            case BundleType::kBundleBadgeTotalNumType:
                value = bundleInfo.GetBadgeTotalNum();
                break;
            case BundleType::kBundleShowBadgeType:
                value = bundleInfo.GetIsShowBadge();
                break;
            case BundleType::kBundlePrivateAllowedType:
                value = bundleInfo.GetIsPrivateAllowed();
                break;
            case BundleType::kBundleEnableNotificationType:
                value = bundleInfo.GetEnableNotification();
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

void NotificationPreferences::OnDistributedKvStoreDeathRecipient()
{
    if (preferncesDB_ != nullptr) {
        preferncesDB_->StoreDeathRecipient(preferencesInfo_);
    }
}

}  // namespace Notification
}  // namespace OHOS