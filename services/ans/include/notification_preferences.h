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

#ifndef BASE_NOTIFICATION_ANS_STANDARD_SERVICES_NOTIFICATION_PREFERENCES_H
#define BASE_NOTIFICATION_ANS_STANDARD_SERVICES_NOTIFICATION_PREFERENCES_H

#include "refbase.h"
#include "singleton.h"

#include "notification_preferences_database.h"

namespace OHOS {
namespace Notification {
class NotificationPreferences final {
public:
    DISALLOW_COPY_AND_MOVE(NotificationPreferences);
    static NotificationPreferences &GetInstance();
    ErrCode AddNotificationSlots(const std::string &bundleName, const std::vector<sptr<NotificationSlot>> &slots);
    ErrCode AddNotificationSlotGroups(
        const std::string &bundleName, const std::vector<sptr<NotificationSlotGroup>> &groups);
    ErrCode RemoveNotificationSlot(const std::string &bundleName, const NotificationConstant::SlotType &slotType);
    ErrCode RemoveNotificationSlotGroups(const std::string &bundleName, const std::vector<std::string> &groupIds);
    ErrCode RemoveNotificationForBundle(const std::string &bundleName);
    ErrCode UpdateNotificationSlots(const std::string &bundleName, const std::vector<sptr<NotificationSlot>> &slot);
    ErrCode UpdateNotificationSlotGroups(
        const std::string &bundleName, const std::vector<sptr<NotificationSlotGroup>> &groups);
    ErrCode GetNotificationSlot(
        const std::string &bundleName, const NotificationConstant::SlotType &type, sptr<NotificationSlot> &slot);
    ErrCode GetNotificationAllSlots(const std::string &bundleName, std::vector<sptr<NotificationSlot>> &slots);
    ErrCode GetNotificationSlotGroup(
        const std::string &bundleName, const std::string &groupId, sptr<NotificationSlotGroup> &group);
    ErrCode GetNotificationAllSlotGroups(
        const std::string &bundleName, std::vector<sptr<NotificationSlotGroup>> &groups);
    ErrCode GetNotificationAllSlotInSlotGroup(
        const std::string &bundleName, const std::string &groupId, std::vector<sptr<NotificationSlot>> &slots);
    ErrCode IsShowBadge(const std::string &bundleName, bool &enable);
    ErrCode SetShowBadge(const std::string &bundleName, const bool enable);
    ErrCode GetImportance(const std::string &bundleName, int &importance);
    ErrCode SetImportance(const std::string &bundleName, const int &importance);
    ErrCode GetTotalBadgeNums(const std::string &bundleName, int &totalBadgeNum);
    ErrCode SetTotalBadgeNums(const std::string &bundleName, const int num);
    ErrCode GetPrivateNotificationsAllowed(const std::string &bundleName, bool &allow);
    ErrCode SetPrivateNotificationsAllowed(const std::string &bundleName, const bool allow);
    ErrCode GetNotificationsEnabledForBundle(const std::string &bundleName, bool &enabled);
    ErrCode SetNotificationsEnabledForBundle(const std::string &bundleName, const bool enabled);
    ErrCode GetNotificationsEnabled(bool &enabled);
    ErrCode SetNotificationsEnabled(const bool &enabled);
    ErrCode GetDisturbMode(NotificationConstant::DisturbMode &mode);
    ErrCode SetDisturbMode(const NotificationConstant::DisturbMode &mode);
    ErrCode ClearNotificationInRestoreFactorySettings();

    void OnDistributedKvStoreDeathRecipient();

private:
    // enum class BundlePropertyType;

    ErrCode CheckSlotForCreateSlot(const std::string &bundleName, const sptr<NotificationSlot> &slot,
        NotificationPreferencesInfo &preferencesInfo) const;
    ErrCode CheckGroupForCreateSlotGroup(const std::string &bundleName, const sptr<NotificationSlotGroup> &group,
        NotificationPreferencesInfo &preferencesInfo) const;
    ErrCode CheckSlotForRemoveSlot(const std::string &bundleName, const NotificationConstant::SlotType &slotType,
        NotificationPreferencesInfo &preferencesInfo) const;
    ErrCode CheckGroupForRemoveSlotGroup(
        const std::string &bundleName, const std::string &groupId, NotificationPreferencesInfo &preferencesInfo) const;
    ErrCode CheckSlotForUpdateSlot(const std::string &bundleName, const sptr<NotificationSlot> &slot,
        NotificationPreferencesInfo &preferencesInfo) const;
    ErrCode CheckGroupForUpdateSlotGroup(const std::string &bundleName, const sptr<NotificationSlotGroup> &group,
        NotificationPreferencesInfo &preferencesInfo) const;
    template <typename T>
    ErrCode SetBundleProperty(NotificationPreferencesInfo &preferencesInfo, const std::string &bundleName,
        const BundleType &type, const T &value);
    template <typename T>
    ErrCode SaveBundleProperty(NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &bundleName,
        const BundleType &type, const T &value);
    template <typename T>
    ErrCode GetBundleProperty(const std::string &bundleName, const BundleType &type, T &value) const;

private:
    NotificationPreferencesInfo preferencesInfo_ {};
    std::unique_ptr<NotificationPreferencesDatabase> preferncesDB_ = nullptr;
    DECLARE_DELAYED_REF_SINGLETON(NotificationPreferences);
};

}  // namespace Notification
}  // namespace OHOS

#endif  // NOTIFICATION_PREFERENCES_H
