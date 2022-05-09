/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "ans_const_define.h"
#include "ans_inner_errors.h"
#include "ans_log_wrapper.h"
#include "bundle_manager_helper.h"
#include "nlohmann/json.hpp"
#include "os_account_manager.h"

namespace OHOS {
namespace Notification {
NotificationPreferences::NotificationPreferences()
{
    preferncesDB_ = std::make_unique<NotificationPreferencesDatabase>();
    InitSettingFromDisturbDB();
}

NotificationPreferences::~NotificationPreferences()
{}

NotificationPreferences &NotificationPreferences::GetInstance()
{
    return DelayedRefSingleton<NotificationPreferences>::GetInstance();
}

ErrCode NotificationPreferences::AddNotificationSlots(
    const sptr<NotificationBundleOption> &bundleOption, const std::vector<sptr<NotificationSlot>> &slots)
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty() || slots.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    ErrCode result = ERR_OK;
    for (auto slot : slots) {
        result = CheckSlotForCreateSlot(bundleOption, slot, preferencesInfo);
        if (result != ERR_OK) {
            return result;
        }
    }

    if (result == ERR_OK &&
        (!preferncesDB_->PutSlotsToDisturbeDB(bundleOption->GetBundleName(), bundleOption->GetUid(), slots))) {
        return ERR_ANS_PREFERENCES_NOTIFICATION_DB_OPERATION_FAILED;
    }

    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }
    return result;
}

ErrCode NotificationPreferences::AddNotificationSlotGroups(
    const sptr<NotificationBundleOption> &bundleOption, const std::vector<sptr<NotificationSlotGroup>> &groups)
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty() || groups.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    ErrCode result = ERR_OK;
    for (auto group : groups) {
        result = CheckGroupForCreateSlotGroup(bundleOption, group, preferencesInfo);
        if (result != ERR_OK) {
            return result;
        }
    }

    if ((result == ERR_OK) &&
        (!preferncesDB_->PutGroupsToDisturbeDB(bundleOption->GetBundleName(), bundleOption->GetUid(), groups))) {
        return ERR_ANS_PREFERENCES_NOTIFICATION_DB_OPERATION_FAILED;
    }

    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }
    return result;
}

ErrCode NotificationPreferences::AddNotificationBundleProperty(const sptr<NotificationBundleOption> &bundleOption)
{
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    preferencesInfo.SetBundleInfo(bundleInfo);
    ErrCode result = ERR_OK;
    if (preferncesDB_->PutBundlePropertyToDisturbeDB(bundleInfo)) {
        preferencesInfo_ = preferencesInfo;
    } else {
        result = ERR_ANS_PREFERENCES_NOTIFICATION_DB_OPERATION_FAILED;
    }

    return result;
}

ErrCode NotificationPreferences::RemoveNotificationSlot(
    const sptr<NotificationBundleOption> &bundleOption, const NotificationConstant::SlotType &slotType)
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    ErrCode result = ERR_OK;
    result = CheckSlotForRemoveSlot(bundleOption, slotType, preferencesInfo);
    if (result == ERR_OK && (!preferncesDB_->RemoveSlotFromDisturbeDB(GenerateBundleKey(bundleOption), slotType))) {
        return ERR_ANS_PREFERENCES_NOTIFICATION_DB_OPERATION_FAILED;
    }

    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }
    return result;
}

ErrCode NotificationPreferences::RemoveNotificationAllSlots(const sptr<NotificationBundleOption> &bundleOption)
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    ErrCode result = ERR_OK;
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (preferencesInfo.GetBundleInfo(bundleOption, bundleInfo)) {
        bundleInfo.RemoveAllSlots();
        preferencesInfo.SetBundleInfo(bundleInfo);
        if (!preferncesDB_->RemoveAllSlotsFromDisturbeDB(GenerateBundleKey(bundleOption))) {
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

ErrCode NotificationPreferences::RemoveNotificationSlotGroups(
    const sptr<NotificationBundleOption> &bundleOption, const std::vector<std::string> &groupIds)
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty() || groupIds.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    ErrCode result = ERR_OK;
    for (auto groupId : groupIds) {
        result = CheckGroupForRemoveSlotGroup(bundleOption, groupId, preferencesInfo);
        if (result != ERR_OK) {
            return result;
        }
    }
    if ((result == ERR_OK) && (!preferncesDB_->RemoveGroupsFromDisturbeDB(GenerateBundleKey(bundleOption), groupIds))) {
        return ERR_ANS_PREFERENCES_NOTIFICATION_DB_OPERATION_FAILED;
    }

    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }
    return result;
}

ErrCode NotificationPreferences::RemoveNotificationForBundle(const sptr<NotificationBundleOption> &bundleOption)
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;

    ErrCode result = ERR_OK;
    if (preferencesInfo.IsExsitBundleInfo(bundleOption)) {
        preferencesInfo.RemoveBundleInfo(bundleOption);
        if (!preferncesDB_->RemoveBundleFromDisturbeDB(GenerateBundleKey(bundleOption))) {
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
    const sptr<NotificationBundleOption> &bundleOption, const std::vector<sptr<NotificationSlot>> &slots)
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty() || slots.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    ErrCode result = ERR_OK;
    for (auto slotIter : slots) {
        result = CheckSlotForUpdateSlot(bundleOption, slotIter, preferencesInfo);
        if (result != ERR_OK) {
            return result;
        }
    }

    if ((result == ERR_OK) &&
        (!preferncesDB_->PutSlotsToDisturbeDB(bundleOption->GetBundleName(), bundleOption->GetUid(), slots))) {
        return ERR_ANS_PREFERENCES_NOTIFICATION_DB_OPERATION_FAILED;
    }

    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }

    return result;
}

ErrCode NotificationPreferences::UpdateNotificationSlotGroups(
    const sptr<NotificationBundleOption> &bundleOption, const std::vector<sptr<NotificationSlotGroup>> &groups)
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty() || groups.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    ErrCode result = ERR_OK;
    for (auto groupIter : groups) {
        result = CheckGroupForUpdateSlotGroup(bundleOption, groupIter, preferencesInfo);
        if (result != ERR_OK) {
            return result;
        }
    }

    if ((result == ERR_OK) &&
        (!preferncesDB_->PutGroupsToDisturbeDB(bundleOption->GetBundleName(), bundleOption->GetUid(), groups))) {
        return ERR_ANS_PREFERENCES_NOTIFICATION_DB_OPERATION_FAILED;
    }

    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }
    return result;
}

ErrCode NotificationPreferences::GetNotificationSlot(const sptr<NotificationBundleOption> &bundleOption,
    const NotificationConstant::SlotType &type, sptr<NotificationSlot> &slot)
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode result = ERR_OK;
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (preferencesInfo_.GetBundleInfo(bundleOption, bundleInfo)) {
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
    const sptr<NotificationBundleOption> &bundleOption, std::vector<sptr<NotificationSlot>> &slots)
{
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode result = ERR_OK;
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (preferencesInfo_.GetBundleInfo(bundleOption, bundleInfo)) {
        bundleInfo.GetAllSlots(slots);
    } else {
        ANS_LOGW("Notification bundle does not exsit.");
        result = ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }

    return result;
}

ErrCode NotificationPreferences::GetNotificationSlotsNumForBundle(
    const sptr<NotificationBundleOption> &bundleOption, uint64_t &num)
{
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode result = ERR_OK;
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (preferencesInfo_.GetBundleInfo(bundleOption, bundleInfo)) {
        num = static_cast<uint64_t>(bundleInfo.GetAllSlotsSize());
    } else {
        result = ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }
    return result;
}

ErrCode NotificationPreferences::GetNotificationSlotGroup(
    const sptr<NotificationBundleOption> &bundleOption, const std::string &groupId, sptr<NotificationSlotGroup> &group)
{
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty() || groupId.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode result = ERR_OK;
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (preferencesInfo_.GetBundleInfo(bundleOption, bundleInfo)) {
        if (!bundleInfo.GetGroup(groupId, group)) {
            result = ERR_ANS_PREFERENCES_NOTIFICATION_SLOTGROUP_NOT_EXIST;
        }
    } else {
        ANS_LOGW("Notification bundle does not exsit.");
        result = ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }
    return result;
}

ErrCode NotificationPreferences::GetNotificationAllSlotGroups(
    const sptr<NotificationBundleOption> &bundleOption, std::vector<sptr<NotificationSlotGroup>> &groups)
{
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode result = ERR_OK;
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (preferencesInfo_.GetBundleInfo(bundleOption, bundleInfo)) {
        bundleInfo.GetAllGroups(groups);
    } else {
        ANS_LOGW("Notification bundle does not exsit.");
        result = ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }
    return result;
}

ErrCode NotificationPreferences::GetNotificationAllSlotInSlotGroup(const sptr<NotificationBundleOption> &bundleOption,
    const std::string &groupId, std::vector<sptr<NotificationSlot>> &slots)
{
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty() || groupId.empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode result = ERR_OK;
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (preferencesInfo_.GetBundleInfo(bundleOption, bundleInfo)) {
        bundleInfo.GetAllSlotsInGroup(groupId, slots);
    } else {
        ANS_LOGW("Notification bundle does not exsit.");
        result = ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }
    return result;
}

ErrCode NotificationPreferences::IsShowBadge(const sptr<NotificationBundleOption> &bundleOption, bool &enable)
{
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    return GetBundleProperty(bundleOption, BundleType::BUNDLE_SHOW_BADGE_TYPE, enable);
}

ErrCode NotificationPreferences::SetShowBadge(const sptr<NotificationBundleOption> &bundleOption, const bool enable)
{
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    ErrCode result = SetBundleProperty(preferencesInfo, bundleOption, BundleType::BUNDLE_SHOW_BADGE_TYPE, enable);
    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }
    return result;
}

ErrCode NotificationPreferences::GetImportance(const sptr<NotificationBundleOption> &bundleOption, int32_t &importance)
{
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    return GetBundleProperty(bundleOption, BundleType::BUNDLE_IMPORTANCE_TYPE, importance);
}

ErrCode NotificationPreferences::SetImportance(
    const sptr<NotificationBundleOption> &bundleOption, const int32_t &importance)
{
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    ErrCode result = SetBundleProperty(preferencesInfo, bundleOption, BundleType::BUNDLE_IMPORTANCE_TYPE, importance);
    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }
    return result;
}

ErrCode NotificationPreferences::GetTotalBadgeNums(
    const sptr<NotificationBundleOption> &bundleOption, int32_t &totalBadgeNum)
{
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    return GetBundleProperty(bundleOption, BundleType::BUNDLE_BADGE_TOTAL_NUM_TYPE, totalBadgeNum);
}

ErrCode NotificationPreferences::SetTotalBadgeNums(
    const sptr<NotificationBundleOption> &bundleOption, const int32_t num)
{
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    ErrCode result = SetBundleProperty(preferencesInfo, bundleOption, BundleType::BUNDLE_BADGE_TOTAL_NUM_TYPE, num);
    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }
    return result;
}

ErrCode NotificationPreferences::GetPrivateNotificationsAllowed(
    const sptr<NotificationBundleOption> &bundleOption, bool &allow)
{
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    return GetBundleProperty(bundleOption, BundleType::BUNDLE_PRIVATE_ALLOWED_TYPE, allow);
}

ErrCode NotificationPreferences::SetPrivateNotificationsAllowed(
    const sptr<NotificationBundleOption> &bundleOption, const bool allow)
{
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    ErrCode result = SetBundleProperty(preferencesInfo, bundleOption, BundleType::BUNDLE_PRIVATE_ALLOWED_TYPE, allow);
    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }
    return result;
}

ErrCode NotificationPreferences::GetNotificationsEnabledForBundle(
    const sptr<NotificationBundleOption> &bundleOption, bool &enabled)
{
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    return GetBundleProperty(bundleOption, BundleType::BUNDLE_ENABLE_NOTIFICATION_TYPE, enabled);
}

ErrCode NotificationPreferences::SetNotificationsEnabledForBundle(
    const sptr<NotificationBundleOption> &bundleOption, const bool enabled)
{
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty()) {
        return ERR_ANS_INVALID_PARAM;
    }

    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    ErrCode result =
        SetBundleProperty(preferencesInfo, bundleOption, BundleType::BUNDLE_ENABLE_NOTIFICATION_TYPE, enabled);
    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }
    return result;
}

ErrCode NotificationPreferences::GetNotificationsEnabled(const int32_t &userId, bool &enabled)
{
    if (userId <= SUBSCRIBE_USER_INIT) {
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode result = ERR_OK;
    if (!preferencesInfo_.GetEnabledAllNotification(userId, enabled)) {
        result = ERR_ANS_INVALID_PARAM;
    }
    return result;
}

ErrCode NotificationPreferences::SetNotificationsEnabled(const int32_t &userId, const bool &enabled)
{
    if (userId <= SUBSCRIBE_USER_INIT) {
        return ERR_ANS_INVALID_PARAM;
    }

    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    preferencesInfo.SetEnabledAllNotification(userId, enabled);
    ErrCode result = ERR_OK;
    if (!preferncesDB_->PutNotificationsEnabled(userId, enabled)) {
        result = ERR_ANS_PREFERENCES_NOTIFICATION_DB_OPERATION_FAILED;
    }

    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }
    return result;
}

ErrCode NotificationPreferences::GetHasPoppedDialog(const sptr<NotificationBundleOption> &bundleOption, bool &hasPopped)
{
    if (bundleOption == nullptr || bundleOption->GetBundleName().empty()) {
        return ERR_ANS_INVALID_PARAM;
    }
    return GetBundleProperty(bundleOption, BundleType::BUNDLE_POPPED_DIALOG_TYPE, hasPopped);
}

ErrCode NotificationPreferences::SetHasPoppedDialog(const sptr<NotificationBundleOption> &bundleOption, bool hasPopped)
{
    if (bundleOption == nullptr) {
        return ERR_ANS_INVALID_PARAM;
    }

    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    ErrCode result = ERR_OK;
    result = SetBundleProperty(preferencesInfo, bundleOption, BundleType::BUNDLE_POPPED_DIALOG_TYPE, hasPopped);
    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }
    return result;
}

ErrCode NotificationPreferences::GetDoNotDisturbDate(const int32_t &userId,
    sptr<NotificationDoNotDisturbDate> &date)
{
    if (userId <= SUBSCRIBE_USER_INIT) {
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode result = ERR_OK;
    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    if (!preferencesInfo.GetDoNotDisturbDate(userId, date)) {
        result = ERR_ANS_INVALID_PARAM;
    }
    return result;
}

ErrCode NotificationPreferences::SetDoNotDisturbDate(const int32_t &userId,
    const sptr<NotificationDoNotDisturbDate> date)
{
    ANS_LOGE("enter.");
    if (userId <= SUBSCRIBE_USER_INIT) {
        return ERR_ANS_INVALID_PARAM;
    }

    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    preferencesInfo.SetDoNotDisturbDate(userId, date);

    ErrCode result = ERR_OK;
    if (!preferncesDB_->PutDoNotDisturbDate(userId, date)) {
        result = ERR_ANS_PREFERENCES_NOTIFICATION_DB_OPERATION_FAILED;
    }

    if (result == ERR_OK) {
        preferencesInfo_ = preferencesInfo;
    }
    return result;
}

ErrCode NotificationPreferences::ClearNotificationInRestoreFactorySettings()
{
    ErrCode result = ERR_OK;
    if (!preferncesDB_->RemoveAllDataFromDisturbeDB()) {
        result = ERR_ANS_PREFERENCES_NOTIFICATION_DB_OPERATION_FAILED;
    }

    if (result == ERR_OK) {
        preferencesInfo_ = NotificationPreferencesInfo();
    }
    return result;
}

ErrCode NotificationPreferences::CheckSlotForCreateSlot(const sptr<NotificationBundleOption> &bundleOption,
    const sptr<NotificationSlot> &slot, NotificationPreferencesInfo &preferencesInfo) const
{
    if (slot == nullptr) {
        ANS_LOGE("Notification slot is nullptr.");
        return ERR_ANS_PREFERENCES_NOTIFICATION_SLOT_NOT_EXIST;
    }

    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (!preferencesInfo.GetBundleInfo(bundleOption, bundleInfo)) {
        bundleInfo.SetBundleName(bundleOption->GetBundleName());
        bundleInfo.SetBundleUid(bundleOption->GetUid());
        bundleInfo.SetEnableNotification(CheckApiCompatibility(bundleOption));
    }
    bundleInfo.SetSlot(slot);
    preferencesInfo.SetBundleInfo(bundleInfo);

    return ERR_OK;
}

ErrCode NotificationPreferences::CheckGroupForCreateSlotGroup(const sptr<NotificationBundleOption> &bundleOption,
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

    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (!preferencesInfo.GetBundleInfo(bundleOption, bundleInfo)) {
        bundleInfo.SetBundleName(bundleOption->GetBundleName());
        bundleInfo.SetBundleUid(bundleOption->GetUid());
        bundleInfo.SetEnableNotification(CheckApiCompatibility(bundleOption));
    } else {
        if (bundleInfo.GetGroupSize() >= MAX_SLOT_GROUP_NUM) {
            return ERR_ANS_PREFERENCES_NOTIFICATION_SLOTGROUP_EXCEED_MAX_NUM;
        }
    }

    bundleInfo.SetGroup(group);
    preferencesInfo.SetBundleInfo(bundleInfo);

    return ERR_OK;
}

ErrCode NotificationPreferences::CheckSlotForRemoveSlot(const sptr<NotificationBundleOption> &bundleOption,
    const NotificationConstant::SlotType &slotType, NotificationPreferencesInfo &preferencesInfo) const
{
    ErrCode result = ERR_OK;
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (preferencesInfo.GetBundleInfo(bundleOption, bundleInfo)) {
        if (bundleInfo.IsExsitSlot(slotType)) {
            bundleInfo.RemoveSlot(slotType);
            preferencesInfo.SetBundleInfo(bundleInfo);
        } else {
            ANS_LOGE("Notification slot type does not exsited.");
            result = ERR_ANS_PREFERENCES_NOTIFICATION_SLOT_TYPE_NOT_EXIST;
        }
    } else {
        ANS_LOGW("Notification bundle does not exsit.");
        result = ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }
    return result;
}

ErrCode NotificationPreferences::CheckGroupForRemoveSlotGroup(const sptr<NotificationBundleOption> &bundleOption,
    const std::string &groupId, NotificationPreferencesInfo &preferencesInfo) const
{
    ErrCode result = ERR_OK;
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (preferencesInfo.GetBundleInfo(bundleOption, bundleInfo)) {
        if (bundleInfo.IsExsitSlotGroup(groupId)) {
            bundleInfo.RemoveSlotGroup(groupId);
            preferencesInfo.SetBundleInfo(bundleInfo);
        } else {
            ANS_LOGE("Notification slot group id is invalid.");
            result = ERR_ANS_PREFERENCES_NOTIFICATION_SLOTGROUP_ID_INVALID;
        }
    } else {
        ANS_LOGW("Notification bundle does not exsit.");
        result = ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }
    return result;
}

ErrCode NotificationPreferences::CheckSlotForUpdateSlot(const sptr<NotificationBundleOption> &bundleOption,
    const sptr<NotificationSlot> &slot, NotificationPreferencesInfo &preferencesInfo) const
{
    if (slot == nullptr) {
        ANS_LOGE("Notification slot is nullptr.");
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode result = ERR_OK;
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (preferencesInfo.GetBundleInfo(bundleOption, bundleInfo)) {
        if (bundleInfo.IsExsitSlot(slot->GetType())) {
            bundleInfo.SetBundleName(bundleOption->GetBundleName());
            bundleInfo.SetBundleUid(bundleOption->GetUid());
            bundleInfo.SetSlot(slot);
            preferencesInfo.SetBundleInfo(bundleInfo);
        } else {
            ANS_LOGE("Notification slot type does not exist.");
            result = ERR_ANS_PREFERENCES_NOTIFICATION_SLOT_TYPE_NOT_EXIST;
        }
    } else {
        ANS_LOGW("Notification bundle does not exsit.");
        result = ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }

    return result;
}

ErrCode NotificationPreferences::CheckGroupForUpdateSlotGroup(const sptr<NotificationBundleOption> &bundleOption,
    const sptr<NotificationSlotGroup> &group, NotificationPreferencesInfo &preferencesInfo) const
{
    ErrCode result = ERR_OK;
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (preferencesInfo.GetBundleInfo(bundleOption, bundleInfo)) {
        if (bundleInfo.IsExsitSlotGroup(group->GetId())) {
            bundleInfo.SetBundleName(bundleOption->GetBundleName());
            bundleInfo.SetBundleUid(bundleOption->GetUid());
            bundleInfo.SetGroup(group);
            preferencesInfo.SetBundleInfo(bundleInfo);
        } else {
            result = ERR_ANS_PREFERENCES_NOTIFICATION_SLOTGROUP_NOT_EXIST;
        }
    } else {
        ANS_LOGW("Notification slot is nullptr.");
        result = ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }
    return result;
}

template <typename T>
ErrCode NotificationPreferences::SetBundleProperty(NotificationPreferencesInfo &preferencesInfo,
    const sptr<NotificationBundleOption> &bundleOption, const BundleType &type, const T &value)
{
    ErrCode result = ERR_OK;
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (!preferencesInfo_.GetBundleInfo(bundleOption, bundleInfo)) {
        bundleInfo.SetBundleName(bundleOption->GetBundleName());
        bundleInfo.SetBundleUid(bundleOption->GetUid());
        bundleInfo.SetEnableNotification(CheckApiCompatibility(bundleOption));
    }

    result = SaveBundleProperty(bundleInfo, bundleOption, type, value);
    preferencesInfo.SetBundleInfo(bundleInfo);

    return result;
}

template <typename T>
ErrCode NotificationPreferences::SaveBundleProperty(NotificationPreferencesInfo::BundleInfo &bundleInfo,
    const sptr<NotificationBundleOption> &bundleOption, const BundleType &type, const T &value)
{
    bool storeDBResult = true;
    switch (type) {
        case BundleType::BUNDLE_IMPORTANCE_TYPE:
            bundleInfo.SetImportance(value);
            storeDBResult = preferncesDB_->PutImportance(bundleInfo, value);
            break;
        case BundleType::BUNDLE_BADGE_TOTAL_NUM_TYPE:
            bundleInfo.SetBadgeTotalNum(value);
            storeDBResult = preferncesDB_->PutTotalBadgeNums(bundleInfo, value);
            break;
        case BundleType::BUNDLE_SHOW_BADGE_TYPE:
            bundleInfo.SetIsShowBadge(value);
            storeDBResult = preferncesDB_->PutShowBadge(bundleInfo, value);
            break;
        case BundleType::BUNDLE_PRIVATE_ALLOWED_TYPE:
            bundleInfo.SetIsPrivateAllowed(value);
            storeDBResult = preferncesDB_->PutPrivateNotificationsAllowed(bundleInfo, value);
            break;
        case BundleType::BUNDLE_ENABLE_NOTIFICATION_TYPE:
            bundleInfo.SetEnableNotification(value);
            storeDBResult = preferncesDB_->PutNotificationsEnabledForBundle(bundleInfo, value);
            break;
        case BundleType::BUNDLE_POPPED_DIALOG_TYPE:
            bundleInfo.SetHasPoppedDialog(value);
            storeDBResult = preferncesDB_->PutHasPoppedDialog(bundleInfo, value);
            break;
        default:
            break;
    }
    return storeDBResult ? ERR_OK : ERR_ANS_PREFERENCES_NOTIFICATION_DB_OPERATION_FAILED;
}

template <typename T>
ErrCode NotificationPreferences::GetBundleProperty(
    const sptr<NotificationBundleOption> &bundleOption, const BundleType &type, T &value) const
{
    ErrCode result = ERR_OK;
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    if (preferencesInfo_.GetBundleInfo(bundleOption, bundleInfo)) {
        switch (type) {
            case BundleType::BUNDLE_IMPORTANCE_TYPE:
                value = bundleInfo.GetImportance();
                break;
            case BundleType::BUNDLE_BADGE_TOTAL_NUM_TYPE:
                value = bundleInfo.GetBadgeTotalNum();
                break;
            case BundleType::BUNDLE_SHOW_BADGE_TYPE:
                value = bundleInfo.GetIsShowBadge();
                break;
            case BundleType::BUNDLE_PRIVATE_ALLOWED_TYPE:
                value = bundleInfo.GetIsPrivateAllowed();
                break;
            case BundleType::BUNDLE_ENABLE_NOTIFICATION_TYPE:
                value = bundleInfo.GetEnableNotification();
                break;
            case BundleType::BUNDLE_POPPED_DIALOG_TYPE:
                value = bundleInfo.GetHasPoppedDialog();
                break;
            default:
                result = ERR_ANS_INVALID_PARAM;
                break;
        }
    } else {
        ANS_LOGW("Notification bundle does not exsit.");
        result = ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST;
    }
    return result;
}

std::string NotificationPreferences::GenerateBundleKey(const sptr<NotificationBundleOption> &bundleOption) const
{
    return bundleOption->GetBundleName().append(std::to_string(bundleOption->GetUid()));
}

void NotificationPreferences::OnDistributedKvStoreDeathRecipient()
{
    if (preferncesDB_ != nullptr) {
        if (preferncesDB_->StoreDeathRecipient()) {
        }
    }
}

ErrCode NotificationPreferences::GetTemplateSupported(const std::string& templateName, bool &support)
{
    if (templateName.length() == 0) {
        ANS_LOGE("template name is null.");
        return ERR_ANS_INVALID_PARAM;
    }

    std::ifstream inFile;
    inFile.open(DEFAULT_TEMPLATE_PATH.c_str(), std::ios::in);
    if (!inFile.is_open()) {
        ANS_LOGE("read template config error.");
        return ERR_ANS_PREFERENCES_NOTIFICATION_READ_TEMPLATE_CONFIG_FAILED;
    }

    nlohmann::json jsonObj;
    inFile >> jsonObj;
    if (jsonObj.is_discarded()) {
        ANS_LOGE("template json discarded error.");
        inFile.close();
        return ERR_ANS_PREFERENCES_NOTIFICATION_READ_TEMPLATE_CONFIG_FAILED;
    }

    if (jsonObj.contains(templateName)) {
        support = true;
    }

    jsonObj.clear();
    inFile.close();
    return ERR_OK;
}

void NotificationPreferences::InitSettingFromDisturbDB()
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    if (preferncesDB_ != nullptr) {
        preferncesDB_->ParseFromDisturbeDB(preferencesInfo_);
    }
}

void NotificationPreferences::RemoveSettings(int32_t userId)
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    preferencesInfo_.RemoveNotificationEnable(userId);
    preferencesInfo_.RemoveDoNotDisturbDate(userId);

    if (preferncesDB_ != nullptr) {
        preferncesDB_->RemoveNotificationEnable(userId);
        preferncesDB_->RemoveDoNotDisturbDate(userId);
    }
}

bool NotificationPreferences::CheckApiCompatibility(const sptr<NotificationBundleOption> &bundleOption) const
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    std::shared_ptr<BundleManagerHelper> bundleManager = BundleManagerHelper::GetInstance();
    if (bundleManager == nullptr) {
        return false;
    }
    return bundleManager->CheckApiCompatibility(bundleOption);
}
}  // namespace Notification
}  // namespace OHOS