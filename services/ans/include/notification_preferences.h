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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_NOTIFICATION_PREFERENCES_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_NOTIFICATION_PREFERENCES_H

#include "refbase.h"
#include "singleton.h"

#include "notification_do_not_disturb_date.h"
#include "notification_preferences_database.h"

namespace OHOS {
namespace Notification {
class NotificationPreferences final {
public:
    DISALLOW_COPY_AND_MOVE(NotificationPreferences);

    /**
     * @brief Get NotificationPreferences instance object.
     */
    static NotificationPreferences &GetInstance();

    /**
     * @brief Add notification slots into DB.
     *
     * @param bundleOption Indicates bunlde info label.
     * @param slots Indicates add notification slots.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode AddNotificationSlots(
        const sptr<NotificationBundleOption> &bundleOption, const std::vector<sptr<NotificationSlot>> &slots);

    /**
     * @brief Add notification slot groups into DB.
     *
     * @param bundleOption Indicates bunlde info label.
     * @param groups Indicates add notification slot groups.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode AddNotificationSlotGroups(
        const sptr<NotificationBundleOption> &bundleOption, const std::vector<sptr<NotificationSlotGroup>> &groups);

    /**
     * @brief Add notification bunle info into DB.
     *
     * @param bundleOption Indicates bunlde info.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode AddNotificationBundleProperty(const sptr<NotificationBundleOption> &bundleOption);

    /**
     * @brief Remove notification a slot in the of bundle from DB.
     *
     * @param bundleOption Indicates bunlde info label.
     * @param slotType Indicates slot type.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode RemoveNotificationSlot(
        const sptr<NotificationBundleOption> &bundleOption, const NotificationConstant::SlotType &slotType);

    /**
     * @brief Remove notification all slot in the of bundle from DB.
     *
     * @param bundleOption Indicates bunlde info label.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode RemoveNotificationAllSlots(const sptr<NotificationBundleOption> &bundleOption);

    /**
     * @brief Remove notification all slot in the of bundle from DB.
     *
     * @param bundleOption Indicates bunlde info label.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode RemoveNotificationSlotGroups(
        const sptr<NotificationBundleOption> &bundleOption, const std::vector<std::string> &groupIds);

    /**
     * @brief Remove notification bundle from DB.
     *
     * @param bundleOption Indicates bunlde info label.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode RemoveNotificationForBundle(const sptr<NotificationBundleOption> &bundleOption);

    /**
     * @brief Update notification slot into DB.
     *
     * @param bundleOption Indicates bunlde info label.
     * @param slot Indicates need to upadte slot.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode UpdateNotificationSlots(
        const sptr<NotificationBundleOption> &bundleOption, const std::vector<sptr<NotificationSlot>> &slot);

    /**
     * @brief Update notification slot group into DB.
     *
     * @param bundleOption Indicates bunlde info label.
     * @param slot Indicates need to upadte slot group.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode UpdateNotificationSlotGroups(
        const sptr<NotificationBundleOption> &bundleOption, const std::vector<sptr<NotificationSlotGroup>> &groups);

    /**
     * @brief Get notification slot from DB.
     *
     * @param bundleOption Indicates bunlde info label.
     * @param type Indicates to get slot type.
     * @param slot Indicates to get slot.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode GetNotificationSlot(const sptr<NotificationBundleOption> &bundleOption,
        const NotificationConstant::SlotType &type, sptr<NotificationSlot> &slot);

    /**
     * @brief Get notification all slots in a bundle from DB.
     *
     * @param bundleOption Indicates bunlde info label.
     * @param slots Indicates to get slots.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode GetNotificationAllSlots(
        const sptr<NotificationBundleOption> &bundleOption, std::vector<sptr<NotificationSlot>> &slots);

    /**
     * @brief Get notification slot num in a bundle from DB.
     *
     * @param bundleOption Indicates bunlde info label.
     * @param num Indicates to get slot num.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode GetNotificationSlotsNumForBundle(const sptr<NotificationBundleOption> &bundleOption, uint64_t &num);

    /**
     * @brief Get notification group in a bundle from DB.
     *
     * @param bundleOption Indicates bunlde info label.
     * @param groupId Indicates to get group id.
     * @param group Indicates to get slot group.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode GetNotificationSlotGroup(const sptr<NotificationBundleOption> &bundleOption, const std::string &groupId,
        sptr<NotificationSlotGroup> &group);

    /**
     * @brief Get notification all group in a bundle from DB.
     *
     * @param bundleOption Indicates bunlde info label.
     * @param groups Indicates to get slot groups.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode GetNotificationAllSlotGroups(
        const sptr<NotificationBundleOption> &bundleOption, std::vector<sptr<NotificationSlotGroup>> &groups);

    /**
     * @brief Get notification all slot in a group in the of bunlde from DB.
     *
     * @param bundleOption Indicates bunlde info label.
     * @param groupId  Indicates to get group id.
     * @param slots Indicates to get slots.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode GetNotificationAllSlotInSlotGroup(const sptr<NotificationBundleOption> &bundleOption,
        const std::string &groupId, std::vector<sptr<NotificationSlot>> &slots);

    /**
     * @brief Get show badge in the of bunlde from DB.
     *
     * @param bundleOption Indicates bunlde info label.
     * @param enable Indicates to whether to show badge
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode IsShowBadge(const sptr<NotificationBundleOption> &bundleOption, bool &enable);

    /**
     * @brief Set show badge in the of bunlde from DB.
     *
     * @param bundleOption Indicates bunlde info label.
     * @param enable Indicates to set show badge
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode SetShowBadge(const sptr<NotificationBundleOption> &bundleOption, const bool enable);

    /**
    * @brief Get importance in the of bunlde from DB.
    *
    * @param bundleOption Indicates bunlde info label.
    * @param importance Indicates to importance label which can be LEVEL_NONE,
               LEVEL_MIN, LEVEL_LOW, LEVEL_DEFAULT, LEVEL_HIGH, or LEVEL_UNDEFINED.
    * @return Return ERR_OK on success, others on failure.
    */
    ErrCode GetImportance(const sptr<NotificationBundleOption> &bundleOption, int32_t &importance);

    /**
    * @brief Set importance in the of bunlde from DB.
    *
    * @param bundleOption Indicates bunlde info label.
    * @param importance Indicates to set a importance label which can be LEVEL_NONE,
               LEVEL_MIN, LEVEL_LOW, LEVEL_DEFAULT, LEVEL_HIGH, or LEVEL_UNDEFINED.
    * @return Return ERR_OK on success, others on failure.
    */
    ErrCode SetImportance(const sptr<NotificationBundleOption> &bundleOption, const int32_t &importance);

    /**
     * @brief Get total badge nums in the of bunlde from DB.
     *
     * @param bundleOption Indicates bunlde info label.
     * @param totalBadgeNum Indicates to get badge num.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode GetTotalBadgeNums(const sptr<NotificationBundleOption> &bundleOption, int32_t &totalBadgeNum);

    /**
     * @brief Set total badge nums in the of bunlde from DB.
     *
     * @param bundleOption Indicates bunlde info label.
     * @param totalBadgeNum Indicates to set badge num.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode SetTotalBadgeNums(const sptr<NotificationBundleOption> &bundleOption, const int32_t num);

    /**
     * @brief Get private notification allowed in the of bunlde from DB.
     *
     * @param bundleOption Indicates bunlde info label.
     * @param allow Indicates to whether to allow.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode GetPrivateNotificationsAllowed(const sptr<NotificationBundleOption> &bundleOption, bool &allow);

    /**
     * @brief Set private notification allowed in the of bunlde from DB.
     *
     * @param bundleOption Indicates bunlde info label.
     * @param allow Indicates to set allow.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode SetPrivateNotificationsAllowed(const sptr<NotificationBundleOption> &bundleOption, const bool allow);

    /**
     * @brief Get private notification enable in the of bunlde from DB.
     *
     * @param bundleOption Indicates bunlde info label.
     * @param enabled Indicates to whether to enable.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode GetNotificationsEnabledForBundle(const sptr<NotificationBundleOption> &bundleOption, bool &enabled);

    /**
     * @brief Set private notification enable in the of bunlde from DB.
     *
     * @param bundleOption Indicates bunlde info label.
     * @param enabled Indicates to set enable.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode SetNotificationsEnabledForBundle(const sptr<NotificationBundleOption> &bundleOption, const bool enabled);

    /**
     * @brief Get notification enable from DB.
     *
     * @param userId Indicates user.
     * @param enabled Indicates to whether to enable.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode GetNotificationsEnabled(const int32_t &userId, bool &enabled);

    /**
     * @brief Set notification enable from DB.
     *
     * @param userId Indicates user.
     * @param enabled Indicates to set enable.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode SetNotificationsEnabled(const int32_t &userId, const bool &enabled);
    ErrCode GetHasPoppedDialog(const sptr<NotificationBundleOption> &bundleOption, bool &hasPopped);
    ErrCode SetHasPoppedDialog(const sptr<NotificationBundleOption> &bundleOption, bool hasPopped);

    /**
     * @brief Get do not disturb date from DB.
     *
     * @param userId Indicates user.
     * @param date Indicates to get do not disturb date.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode GetDoNotDisturbDate(const int32_t &userId, sptr<NotificationDoNotDisturbDate> &date);

    /**
     * @brief Set do not disturb date from DB.
     *
     * @param userId Indicates user.
     * @param date Indicates to set do not disturb date.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode SetDoNotDisturbDate(const int32_t &userId, const sptr<NotificationDoNotDisturbDate> date);
    ErrCode GetTemplateSupported(const std::string &templateName, bool &support);

    /**
     * @brief Remove all proferences info from DB.
     *
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode ClearNotificationInRestoreFactorySettings();

    /**
     * @brief Death recipient.
     */
    void OnDistributedKvStoreDeathRecipient();
    void InitSettingFromDisturbDB();
    void RemoveSettings(int32_t userId);

private:
    ErrCode CheckSlotForCreateSlot(const sptr<NotificationBundleOption> &bundleOption,
        const sptr<NotificationSlot> &slot, NotificationPreferencesInfo &preferencesInfo) const;
    ErrCode CheckGroupForCreateSlotGroup(const sptr<NotificationBundleOption> &bundleOption,
        const sptr<NotificationSlotGroup> &group, NotificationPreferencesInfo &preferencesInfo) const;
    ErrCode CheckSlotForRemoveSlot(const sptr<NotificationBundleOption> &bundleOption,
        const NotificationConstant::SlotType &slotType, NotificationPreferencesInfo &preferencesInfo) const;
    ErrCode CheckGroupForRemoveSlotGroup(const sptr<NotificationBundleOption> &bundleOption, const std::string &groupId,
        NotificationPreferencesInfo &preferencesInfo) const;
    ErrCode CheckSlotForUpdateSlot(const sptr<NotificationBundleOption> &bundleOption,
        const sptr<NotificationSlot> &slot, NotificationPreferencesInfo &preferencesInfo) const;
    ErrCode CheckGroupForUpdateSlotGroup(const sptr<NotificationBundleOption> &bundleOption,
        const sptr<NotificationSlotGroup> &group, NotificationPreferencesInfo &preferencesInfo) const;
    template <typename T>
    ErrCode SetBundleProperty(NotificationPreferencesInfo &preferencesInfo,
        const sptr<NotificationBundleOption> &bundleOption, const BundleType &type, const T &value);
    template <typename T>
    ErrCode SaveBundleProperty(NotificationPreferencesInfo::BundleInfo &bundleInfo,
        const sptr<NotificationBundleOption> &bundleOption, const BundleType &type, const T &value);
    template <typename T>
    ErrCode GetBundleProperty(
        const sptr<NotificationBundleOption> &bundleOption, const BundleType &type, T &value) const;
    std::string GenerateBundleKey(const sptr<NotificationBundleOption> &bundleOption) const;
    bool CheckApiCompatibility(const sptr<NotificationBundleOption> &bundleOption) const;

private:
    NotificationPreferencesInfo preferencesInfo_ {};
    std::unique_ptr<NotificationPreferencesDatabase> preferncesDB_ = nullptr;
    DECLARE_DELAYED_REF_SINGLETON(NotificationPreferences);
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_NOTIFICATION_PREFERENCES_H
