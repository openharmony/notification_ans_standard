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

#ifndef BASE_NOTIFICATION_ANS_STANDARD_SERVICES_NOTIFICATION_PREFERENCES_DATABASE_H
#define BASE_NOTIFICATION_ANS_STANDARD_SERVICES_NOTIFICATION_PREFERENCES_DATABASE_H

#include <functional>
#include <memory>

#include "distributed_kv_data_manager.h"
#include "notification_preferences_info.h"

namespace OHOS {
namespace Notification {
class NotificationPreferencesDatabase final {
public:
    NotificationPreferencesDatabase();
    ~NotificationPreferencesDatabase();

    /**
     * @brief Put notification slots into disturbe DB.
     *
     * @param bundleName Indicates bunlde name.
     * @param bundleUid Indicates bunlde uid.
     * @param slots Indicates notification slots.
     * @return Return true on success, false on failure.
     */
    bool PutSlotsToDisturbeDB(
        const std::string &bundleName, const int32_t &bundleUid, const std::vector<sptr<NotificationSlot>> &slots);

    /**
     * @brief Put notification slot groups into disturbe DB.
     *
     * @param bundleName Indicates bunlde name.
     * @param bundleUid Indicates bunlde uid.
     * @param groups Indicates notification groups.
     * @return Return true on success, false on failure.
     */
    bool PutGroupsToDisturbeDB(const std::string &bundleName, const int32_t &bundleUid,
         const std::vector<sptr<NotificationSlotGroup>> &groups);

    /**
     * @brief Put notification bundle into disturbe DB.
     *
     * @param bundleInfo Indicates bunlde info.
     * @return Return true on success, false on failure.
     */
    bool PutBundlePropertyToDisturbeDB(const NotificationPreferencesInfo::BundleInfo &bundleInfo);

    /**
     * @brief Put show badge in the of bundle into disturbe DB.
     *
     * @param bundleInfo Indicates bunlde info.
     * @param enable Indicates to whether show badge.
     * @return Return true on success, false on failure.
     */
    bool PutShowBadge(const NotificationPreferencesInfo::BundleInfo &bundleInfo, const bool &enable);

    /**
     * @brief Put importance in the of bundle into disturbe DB.
     *
     * @param bundleInfo Indicates bunlde info.
     * @param importance Indicates to importance level  which can be LEVEL_NONE,
               LEVEL_MIN, LEVEL_LOW, LEVEL_DEFAULT, LEVEL_HIGH, or LEVEL_UNDEFINED.
     * @return Return true on success, false on failure.
     */
    bool PutImportance(const NotificationPreferencesInfo::BundleInfo &bundleInfo, const int32_t &importance);

    /**
     * @brief Put badge total nums in the of  bundle into disturbe DB.
     *
     * @param bundleInfo Indicates bunlde info.
     * @param totalBadgeNum Indicates to total badge num.
     * @return Return true on success, false on failure.
     */
    bool PutTotalBadgeNums(const NotificationPreferencesInfo::BundleInfo &bundleInfo, const int32_t &totalBadgeNum);

    /**
     * @brief Put private notification allowed in the of  bundle into disturbe DB.
     *
     * @param bundleInfo Indicates bunlde info.
     * @param allow Indicates to whether to allow
     * @return Return true on success, false on failure.
     */
    bool PutPrivateNotificationsAllowed(const NotificationPreferencesInfo::BundleInfo &bundleInfo, const bool &allow);

    /**
     * @brief Put enable notification in the of  bundle into disturbe DB.
     *
     * @param bundleInfo Indicates bunlde info.
     * @param enabled Indicates to whether to enabled
     * @return Return true on success, false on failure.
     */
    bool PutNotificationsEnabledForBundle(
        const NotificationPreferencesInfo::BundleInfo &bundleInfo, const bool &enabled);

    /**
     * @brief Put enable notification into disturbe DB.
     *
     * @param userId Indicates user.
     * @param enabled Indicates to whether to enabled
     * @return Return true on success, false on failure.
     */
    bool PutNotificationsEnabled(const int32_t &userId, const bool &enabled);
    bool PutHasPoppedDialog(const NotificationPreferencesInfo::BundleInfo &bundleInfo, const bool &hasPopped);

    /**
     * @brief Put do not disturbe date into disturbe DB.
     *
     * @param userId Indicates user.
     * @param date Indicates to do not disturbe date.
     * @return Return true on success, false on failure.
     */
    bool PutDoNotDisturbDate(const int32_t &userId, const sptr<NotificationDoNotDisturbDate> &date);

    /**
     * @brief Parse notification info from disturbe DB.
     *
     * @param info Indicates notification info.
     * @return Return true on success, false on failure.
     */
    bool ParseFromDisturbeDB(NotificationPreferencesInfo &info);

    /**
     * @brief Delete all data from disturbe DB.
     *
     * @return Return true on success, false on failure.
     */
    bool RemoveAllDataFromDisturbeDB();

    /**
     * @brief Delete bundle data from disturbe DB.
     *
     * @param bundleKey Indicates the bundle key.
     * @return Return true on success, false on failure.
     */
    bool RemoveBundleFromDisturbeDB(const std::string &bundleKey);

    /**
     * @brief Delete slot from disturbe DB.
     *
     * @param bundleKey Indicates to which a bundle.
     * @param type Indicates to slot type.
     * @return Return true on success, false on failure.
     */
    bool RemoveSlotFromDisturbeDB(const std::string &bundleKey, const NotificationConstant::SlotType &type);

    /**
     * @brief Delete all slots in the of bundle from disturbe DB.
     *
     * @param bundleKey Indicates to which a bundle.
     * @return Return true on success, false on failure.
     */
    bool RemoveAllSlotsFromDisturbeDB(const std::string &bundleKey);

    /**
     * @brief Delete some groups in the of bundle from disturbe DB.
     *
     * @param bundleKey Indicates to which a bundle.
     * @param groupId Indicates to group id in the of bundle.
     * @return Return true on success, false on failure.
     */
    bool RemoveGroupsFromDisturbeDB(const std::string &bundleKey, const std::vector<std::string> &groupId);

    /**
     * @brief Deal death recipient.
     *
     * @return Return true on success, false on failure.
     */
    bool StoreDeathRecipient();

    bool RemoveNotificationEnable(const int32_t userId);
    bool RemoveDoNotDisturbDate(const int32_t userId);

private:
    void TryTwice(const std::function<DistributedKv::Status()> &func) const;
    DistributedKv::Status GetKvStore();
    void CloseKvStore();
    bool CheckKvStore();

    bool CheckBundle(const std::string &bundleName, const int32_t &bundleUid);
    bool PutBundlePropertyValueToDisturbeDB(const NotificationPreferencesInfo::BundleInfo &bundleInfo);
    template <typename T>
    DistributedKv::Status PutBundlePropertyToDisturbeDB(
        const std::string &bundleKey, const BundleType &type, const T &t);
    bool PutBundleToDisturbeDB(
        const std::string &bundleKey, const NotificationPreferencesInfo::BundleInfo &bundleInfo);

    void GetValueFromDisturbeDB(const std::string &key, std::function<void(DistributedKv::Value &)> function);
    void GetValueFromDisturbeDB(const std::string &key,
        std::function<void(DistributedKv::Status &, DistributedKv::Value &)> function);
    bool GetRemoveGroupKeysFromDisturbeDB(
        const std::string &bundleKey, const std::string &groupId, std::vector<DistributedKv::Key> &keys);

    bool SlotToEntry(const std::string &bundleName, const int32_t &bundleUid, const sptr<NotificationSlot> &slot,
        std::vector<DistributedKv::Entry> &entries);
    bool GroupToEntry(const std::string &bundleName, const int32_t &bundleUid, const sptr<NotificationSlotGroup> &group,
        std::vector<DistributedKv::Entry> &entries);
    void GenerateGroupEntry(const std::string &bundleKey, const sptr<NotificationSlotGroup> &group,
        std::vector<DistributedKv::Entry> &entries) const;
    void GenerateSlotEntry(const std::string &bundleKey, const sptr<NotificationSlot> &slot,
        std::vector<DistributedKv::Entry> &entries) const;
    void GenerateEntry(
        const std::string &key, const std::string &value, std::vector<DistributedKv::Entry> &entry) const;

    std::string FindLastString(const std::string &findString, const std::string &inputString) const;
    std::string SubUniqueIdentifyFromString(const std::string &findString, const std::string &keyStr) const;
    std::string VectorToString(const std::vector<int64_t> &data) const;
    void StringToVector(const std::string &str, std::vector<int64_t> &data) const;
    int32_t StringToInt(const std::string &str) const;
    int64_t StringToInt64(const std::string &str) const;
    bool IsSlotKey(const std::string &bundleKey, const std::string &key) const;
    bool IsGroupKey(const std::string &bundleKey, const std::string &key) const;
    std::string GenerateSlotKey(
        const std::string &bundleKey, const std::string &type = "", const std::string &subType = "") const;
    std::string GenerateGroupKey(const std::string &bundleKey, const std::string &subType = "") const;
    std::string GenerateBundleKey(const std::string &bundleKey, const std::string &type = "") const;

    void ParseBundleFromDistureDB(
        NotificationPreferencesInfo &info, const std::vector<DistributedKv::Entry> &entries);
    void ParseSlotFromDisturbeDB(NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &bundleKey,
        const DistributedKv::Entry &entry);
    void ParseGroupFromDisturbeDB(NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &bundleKey,
        const DistributedKv::Entry &entry);
    void ParseBundlePropertyFromDisturbeDB(NotificationPreferencesInfo::BundleInfo &bundleInfo,
        const std::string &bundleKey, const DistributedKv::Entry &entry);
    void ParseDoNotDisturbType(NotificationPreferencesInfo &info);
    void ParseDoNotDisturbBeginDate(NotificationPreferencesInfo &info);
    void ParseDoNotDisturbEndDate(NotificationPreferencesInfo &info);
    void ParseEnableAllNotification(NotificationPreferencesInfo &info);
    void ParseGroupDescription(
        const std::string &bundleKey, sptr<NotificationSlotGroup> &group, const DistributedKv::Entry &entry);
    void ParseBundleName(NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &value) const;
    void ParseBundleImportance(NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &value) const;
    void ParseBundleShowBadge(NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &value) const;
    void ParseBundleBadgeNum(NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &value) const;
    void ParseBundlePrivateAllowed(
        NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &value) const;
    void ParseBundleEnableNotification(
        NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &value) const;
    void ParseBundlePoppedDialog(
        NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &value) const;
    void ParseBundleUid(NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &value) const;
    void ParseSlot(
        const std::string &findString, sptr<NotificationSlot> &slot, const DistributedKv::Entry &entry);
    void ParseSlotGroupId(sptr<NotificationSlot> &slot, const std::string &value) const;
    void ParseSlotDescription(sptr<NotificationSlot> &slot, const std::string &value) const;
    void ParseSlotLevel(sptr<NotificationSlot> &slot, const std::string &value) const;
    void ParseSlotShowBadge(sptr<NotificationSlot> &slot, const std::string &value) const;
    void ParseSlotEnableLight(sptr<NotificationSlot> &slot, const std::string &value) const;
    void ParseSlotEnableVrbration(sptr<NotificationSlot> &slot, const std::string &value) const;
    void ParseSlotLedLightColor(sptr<NotificationSlot> &slot, const std::string &value) const;
    void ParseSlotLockscreenVisibleness(sptr<NotificationSlot> &slot, const std::string &value) const;
    void ParseSlotSound(sptr<NotificationSlot> &slot, const std::string &value) const;
    void ParseSlotVibrationSytle(sptr<NotificationSlot> &slot, const std::string &value) const;
    void ParseSlotEnableBypassDnd(sptr<NotificationSlot> &slot, const std::string &value) const;
    void ParseSlotEnabled(sptr<NotificationSlot> &slot, const std::string &value) const;

    std::string GenerateBundleLablel(const NotificationPreferencesInfo::BundleInfo &bundleInfo) const;
    void GetDoNotDisturbType(NotificationPreferencesInfo &info, int32_t userId);
    void GetDoNotDisturbBeginDate(NotificationPreferencesInfo &info, int32_t userId);
    void GetDoNotDisturbEndDate(NotificationPreferencesInfo &info, int32_t userId);
    void GetEnableAllNotification(NotificationPreferencesInfo &info, int32_t userId);

    static const std::map<std::string,
        std::function<void(NotificationPreferencesDatabase *, sptr<NotificationSlot> &, std::string &)>>
        slotMap_;
    static const std::map<std::string, std::function<void(NotificationPreferencesDatabase *,
                                           NotificationPreferencesInfo::BundleInfo &, std::string &)>>
        bundleMap_;

    const DistributedKv::AppId appId_ {APP_ID};
    const DistributedKv::StoreId storeId_ {STORE_ID};
    std::shared_ptr<DistributedKv::SingleKvStore> kvStorePtr_ = nullptr;
    DistributedKv::DistributedKvDataManager dataManager_;
};
}  // namespace Notification
}  // namespace OHOS
#endif  // BASE_NOTIFICATION_ANS_STANDARD_SERVICES_NOTIFICATION_PREFERENCES_DATABASE_H
