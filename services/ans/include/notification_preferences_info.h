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

#ifndef BASE_NOTIFICATION_ANS_STANDARD_SERVICES_NOTIFICATION_PREFERENCES_INFO_H
#define BASE_NOTIFICATION_ANS_STANDARD_SERVICES_NOTIFICATION_PREFERENCES_INFO_H

#include <map>
#include <string>
#include <vector>

#include "notification_bundle_option.h"
#include "notification_do_not_disturb_date.h"
#include "notification_slot.h"
#include "notification_slot_group.h"
#include "preferences_constant.h"

namespace OHOS {
namespace Notification {
class NotificationPreferencesInfo final {
public:
    class BundleInfo final {
    public:
        BundleInfo();
        ~BundleInfo();
        /**
         * @brief Set bundle name.
         *
         * @param name Indicates the bundle name.
         */
        void SetBundleName(const std::string &name);

        /**
         * @brief Get bundle name.
         *
         * @return Return bundle name.
         */
        std::string GetBundleName() const;

        /**
         * @brief Set bundle importance.
         *
         * @param name Indicates the bundle importance.
         */
        void SetImportance(const int32_t &level);

        /**
         * @brief Get bundle importance.
         *
         * @return Return importance.
         */
        int32_t GetImportance() const;

        /**
         * @brief Set bundle Whether to show badge.
         *
         * @param name Indicates the set bundle Whether to show badge.
         */
        void SetIsShowBadge(const bool &isShowBadge);

        /**
         * @brief Get bundle Whether to show badge.
         *
         * @return Return true on success, false on failure.
         */
        bool GetIsShowBadge() const;

        /**
         * @brief Set bundle total badge num.
         *
         * @param name Indicates the set bundle total badge num.
         */
        void SetBadgeTotalNum(const int32_t &num);

        /**
         * @brief Get bundle total badge num.
         *
         * @return Return badge total num.
         */
        int32_t GetBadgeTotalNum() const;

        /**
         * @brief Set bundle Whether to private allowed.
         *
         * @param name Indicates the set Whether to private allowed.
         */
        void SetIsPrivateAllowed(const bool &isPrivateAllowed);

        /**
         * @brief Get bundle Whether to private allowed.
         *
         * @return Return true on success, false on failure.
         */
        bool GetIsPrivateAllowed() const;

        /**
         * @brief Set bundle enable notification.
         *
         * @param enable Indicates the set enable notification.
         */
        void SetEnableNotification(const bool &enable);

        /**
         * @brief Set bundle enable notification.
         *
         * @return Return true on success, false on failure.
         */
        bool GetEnableNotification() const;

        void SetHasPoppedDialog(const bool &hasPopped);
        bool GetHasPoppedDialog() const;

        /**
         * @brief Set bundle slot.
         *
         * @param slot Indicates the set slot.
         */
        void SetSlot(const sptr<NotificationSlot> &slot);

        /**
         * @brief Get bundle slot by type.
         *
         * @param type Indicates the slot type.
         * @param slot Indicates the slot object.
         * @return Return true on success, false on failure.
         */
        bool GetSlot(const NotificationConstant::SlotType &type, sptr<NotificationSlot> &slot) const;

        /**
         * @brief Get slots from bundle.
         *
         * @param slots Indicates the get slots.
         * @return Return true on success, false on failure.
         */
        bool GetAllSlots(std::vector<sptr<NotificationSlot>> &slots);

        /**
         * @brief Get slot num from bundle.
         *
         * @return Return true on success, false on failure.
         */
        uint32_t GetAllSlotsSize();

        /**
         * @brief Get all slot from group in bundle.
         *
         * @param groupId Indicates a groupId from bundle.
         * @param slots Indicates get slots from group.
         * @return Return true on success, false on failure.
         */
        bool GetAllSlotsInGroup(const std::string &groupId, std::vector<sptr<NotificationSlot>> &slots);

        /**
         * @brief Get all slot from group in bundle.
         *
         * @param groupId Indicates a groupId from bundle.
         * @param slots Indicates get slots from group.
         * @return Return true on success, false on failure.
         */
        bool GetAllSlotsInGroup(const std::string &groupId, std::vector<NotificationSlot> &slots);

        /**
         * @brief Set bundle group.
         *
         * @param group Indicates the set group.
         */
        void SetGroup(const sptr<NotificationSlotGroup> &group);

        /**
         * @brief Get group from bundle.
         *
         * @param groupId Indicates the get group id in the of bundle.
         * @param group Indicates the get group.
         * @return Return true on success, false on failure.
         */
        bool GetGroup(const std::string &groupId, sptr<NotificationSlotGroup> &group);

        /**
         * @brief Get all group from bundle.
         *
         * @param group Indicates the get groups.
         * @return Return true on success, false on failure.
         */
        bool GetAllGroups(std::vector<sptr<NotificationSlotGroup>> &group);

        /**
         * @brief Get slot group num from bundle.
         *
         * @return Return num is group size.
         */
        size_t GetGroupSize() const;

        /**
         * @brief Check whether to exsist slot in the of bundle.
         *
         * @param type Indicates the slot type.
         * @return Return true on success, false on failure.
         */
        bool IsExsitSlot(const NotificationConstant::SlotType &type) const;

        /**
         * @brief Check whether to exsist slot group in the of bundle.
         *
         * @param groupId Indicates the slot group id.
         * @return Return true on success, false on failure.
         */
        bool IsExsitSlotGroup(const std::string &groupId) const;

        /**
         * @brief Rremove a slot from bundle.
         *
         * @param type Indicates the slot type.
         * @return Return true on success, false on failure.
         */
        bool RemoveSlot(const NotificationConstant::SlotType &type);

        /**
         * @brief Remove all slots from bundle.
         *
         * @return Return true on success, false on failure.
         */
        void RemoveAllSlots();

        /**
         * @brief remove slot group from bundle.
         *
         * @param groupId Indicates the slot group id.
         * @return Return true on success, false on failure.
         */
        bool RemoveSlotGroup(const std::string &groupId);
        void SetBundleUid(const int32_t &uid);
        int32_t GetBundleUid() const;
        void SetSlotEnabled(NotificationConstant::SlotType slotType, bool enabled);
        bool GetSlotEnabled(NotificationConstant::SlotType slotType, bool &enabled) const;

    private:
        std::string bundleName_;
        int32_t uid_ = 0;
        int32_t importance_ = BUNDLE_IMPORTANCE;
        bool isShowBadge_ = BUNDLE_SHOW_BADGE;
        int32_t badgeTotalNum_ = BUNDLE_BADGE_TOTAL_NUM;
        bool isPrivateAllowed_ = BUNDLE_PRIVATE_ALLOWED;
        bool isEnabledNotification_ = BUNDLE_ENABLE_NOTIFICATION;
        bool hasPoppedDialog_ = BUNDLE_POPPED_DIALOG;
        std::map<NotificationConstant::SlotType, sptr<NotificationSlot>> slots_;
        std::map<std::string, sptr<NotificationSlotGroup>> groups_;
    };

    /*
     * @brief Constructor used to create an NotificationPreferencesInfo object.
     */
    NotificationPreferencesInfo()
    {}
    /**
     * @brief Default destructor.
     */
    ~NotificationPreferencesInfo()
    {}

    /**
     * set bundle info into preferences info.
     * @param info Indicates the bundle.
     */
    void SetBundleInfo(const BundleInfo &info);

    /**
     * get bundle info from preferences info.
     * @param bundleOption Indicates the bundle info label.
     * @param info Indicates the bundle info.
     * @return Whether to get bundle info success.
     */
    bool GetBundleInfo(const sptr<NotificationBundleOption> &bundleOption, BundleInfo &info) const;

    /**
     * remove bundle info from preferences info.
     * @param bundleOption Indicates the bundle info label.
     * @return Whether to remove bundle info success.
     */
    bool RemoveBundleInfo(const sptr<NotificationBundleOption> &bundleOption);

    /**
     * whether to exsist bundle info in the of preferences info.
     * @param bundleOption Indicates the bundle info label.
     * @return Whether to exsist bundle info.
     */
    bool IsExsitBundleInfo(const sptr<NotificationBundleOption> &bundleOption) const;

    /**
     * clear bundle info in the of preferences info.
     */
    void ClearBundleInfo();

    /**
     * set do not disturb date into preferences info.
     * @param userId Indicates userId.
     * @param doNotDisturbDate Indicates do not disturb date.
     * @return Whether to set do not disturb success.
     */
    void SetDoNotDisturbDate(const int32_t &userId,
        const sptr<NotificationDoNotDisturbDate> &doNotDisturbDate);

    /**
     * get do not disturb date from preferences info.
     * @param userId Indicates userId.
     * @param doNotDisturbDate Indicates do not disturb date.
     * @return Whether to get do not disturb success.
     */
    bool GetDoNotDisturbDate(const int32_t &userId,
        sptr<NotificationDoNotDisturbDate> &doNotDisturbDate) const;

    /**
     * set enable all notification into preferences info.
     * @param userId Indicates userId.
     * @param enable Indicates whether to enable all notification.
     */
    void SetEnabledAllNotification(const int32_t &userId, const bool &enable);

    /**
     * get enable all notification from preferences info.
     * @param userId Indicates userId.
     * @param enable Indicates whether to enable all notification.
     * @return Whether to enable all notification success.
     */
    bool GetEnabledAllNotification(const int32_t &userId, bool &enable) const;
    void RemoveNotificationEnable(const int32_t userId);
    void RemoveDoNotDisturbDate(const int32_t userId);

private:
    std::map<int32_t, bool> isEnabledAllNotification_;
    std::map<int32_t, sptr<NotificationDoNotDisturbDate>> doNotDisturbDate_;
    std::map<std::string, BundleInfo> infos_;
};
}  // namespace Notification
}  // namespace OHOS
#endif  // BASE_NOTIFICATION_ANS_STANDARD_SERVICES_NOTIFICATION_PREFERENCES_INFO_H