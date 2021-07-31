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

#ifndef BASE_NOTIFICATION_ANS_STANDARD_SERVICES_NOTIFICATION_PREFERENCES_DATABASE_H
#define BASE_NOTIFICATION_ANS_STANDARD_SERVICES_NOTIFICATION_PREFERENCES_DATABASE_H

#include <functional>
#include <memory>

#include "distributed_kv_data_manager.h"
#include "notification_preferences_info.h"

namespace OHOS {
namespace Notification {

class NotificationPreferencesDatabase {
public:
    NotificationPreferencesDatabase();
    ~NotificationPreferencesDatabase();
    bool PutSlotsToDisturbeDB(const std::string &bundleName, const std::vector<sptr<NotificationSlot>> &slots);
    bool PutGroupsToDisturbeDB(const std::string &bundleName, const std::vector<sptr<NotificationSlotGroup>> &groups);

    bool PutShowBadge(const std::string &bundleName, const bool &enable);
    bool PutImportance(const std::string &bundleName, const int &importance);
    bool PutTotalBadgeNums(const std::string &bundleName, const int &totalBadgeNum);
    bool PutPrivateNotificationsAllowed(const std::string &bundleName, const bool &allow);
    bool PutNotificationsEnabledForBundle(const std::string &bundleName, const bool &enabled);
    bool PutNotificationsEnabled(const bool &enabled);
    bool PutDisturbMode(const NotificationConstant::DisturbMode &mode);

    bool ParseFromDisturbeDB(NotificationPreferencesInfo &info);

    bool RemoveAllBundleFromDisturbeDB();
    bool RemoveBundleFromDisturbeDB(const std::string &bundleName);
    bool RemoveSlotFromDisturbeDB(const std::string &bundleName, const NotificationConstant::SlotType &type);
    bool RemoveGroupsFromDisturbeDB(const std::string &bundleName, const std::vector<std::string> &groupId);

    void SubscribeDeathRecipient(const std::shared_ptr<DistributedKv::KvStoreDeathRecipient> &callback);
    void UnsubscribeDeathRecipient(const std::shared_ptr<DistributedKv::KvStoreDeathRecipient> &callback);
    bool StoreDeathRecipient(NotificationPreferencesInfo &info);

private:
    void TryTwice(const std::function<OHOS::DistributedKv::Status()> &func) const;
    OHOS::DistributedKv::Status GetKvStore();
    void CloseKvStore();
    bool CheckKvStore();
    bool ResetStore();

    bool CheckBundle(const std::string &bundleName);
    bool InitBundlePropertyValueToDisturbeDB(const std::string &bundleName);
    template <typename T>
    OHOS::DistributedKv::Status PutBundlePropertyToDisturbeDB(
        const std::string &bundleName, const BundleType &type, const T &t);
    bool PutBundleToDisturbeDB(const std::string &bundleName);

    void GetValueFromDisturbeDB(const std::string &key, std::function<void(OHOS::DistributedKv::Value &)> funcion);
    void GetValueFromDisturbeDB(const std::string &key,
        std::function<void(OHOS::DistributedKv::Status &, OHOS::DistributedKv::Value &)> funcion);
    bool GetRemoveGroupKeysFromDisturbeDB(
        const std::string &bundleName, const std::string &groupId, std::vector<OHOS::DistributedKv::Key> &keys);

    bool SlotToEntry(const std::string &bundleName, const sptr<NotificationSlot> &slot,
        std::vector<OHOS::DistributedKv::Entry> &entries);
    bool GroupToEntry(const std::string &bundleName, const sptr<NotificationSlotGroup> &group,
        std::vector<OHOS::DistributedKv::Entry> &entries);
    void GenerateGroupEntry(const std::string &bundleName, const sptr<NotificationSlotGroup> &group,
        std::vector<OHOS::DistributedKv::Entry> &entries);
    void GenerateSlotEntry(const std::string &bundleName, const sptr<NotificationSlot> &slot,
        std::vector<OHOS::DistributedKv::Entry> &entries);
    void GenerateEntry(
        const std::string &key, const std::string &value, std::vector<OHOS::DistributedKv::Entry> &entry);

    std::string FindLastString(const std::string &inputString) const;
    std::string SubUniqueIdentifyFromString(const std::string &keyStr) const;
    std::string VectorToString(const std::vector<int64_t> &data) const;
    void StringToVector(const std::string &str, std::vector<int64_t> &data) const;
    std::string GenerateSlotKey(
        const std::string &bundleKey, const std::string &type = "", const std::string &subType = "") const;
    std::string GenerateGroupKey(const std::string &bundleKey, const std::string &subType = "") const;
    std::string GenerateBundleKey(const std::string &bundleName, const std::string &type = "") const;

    void ParseBundleFromDistureDB(
        NotificationPreferencesInfo &info, const std::vector<OHOS::DistributedKv::Entry> &entries);
    void ParseSlotFromDisturbeDB(NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &bundleName,
        const OHOS::DistributedKv::Entry &entry);
    void ParseGroupFromDisturbeDB(NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &bundleName,
        const OHOS::DistributedKv::Entry &entry);
    void ParseBundlePropertyFromDisturbeDB(NotificationPreferencesInfo::BundleInfo &bundleInfo,
        const OHOS::DistributedKv::Key &key, const OHOS::DistributedKv::Value &value);
    void ParseDisturbeMode(NotificationPreferencesInfo &info);
    void ParseEnableAllNotification(NotificationPreferencesInfo &info);
    void ParseGroupDescription(sptr<NotificationSlotGroup> &group, const OHOS::DistributedKv::Entry &entry);
    void ParseBundleName(NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &value) const;
    void ParseBundleImportance(NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &value) const;
    void ParseBundleShowBadge(NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &value) const;
    void ParseBundleBadgeNum(NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &value) const;
    void ParseBundlePrivateAllowed(NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &value) const;
    void ParseBundleEnableNotification(
        NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &value) const;
    void ParseSlot(sptr<NotificationSlot> &slot, const OHOS::DistributedKv::Entry &entry);
    void ParseSlotGroupId(sptr<NotificationSlot> &slot, const std::string &value) const;
    void ParseSlotDescription(sptr<NotificationSlot> &slot, const std::string &value) const;
    void ParseSlotLevel(sptr<NotificationSlot> &slot, const std::string &value) const;
    void ParseSlotShowBadge(sptr<NotificationSlot> &slot, const std::string &value) const;
    void ParseSlotEnableLight(sptr<NotificationSlot> &slot, const std::string &value) const;
    void ParseSlotEnableVrbration(sptr<NotificationSlot> &slot, const std::string &value) const;
    void ParseSlotLedLightColor(sptr<NotificationSlot> &slot, const std::string &kevaluey) const;
    void ParseSlotLockscreenVisibleness(sptr<NotificationSlot> &slot, const std::string &value) const;
    void ParseSlotSound(sptr<NotificationSlot> &slot, const std::string &value) const;
    void ParseSlotVibrationSytle(sptr<NotificationSlot> &slot, const std::string &value) const;
    void ParseSlotEnableBypassDnd(sptr<NotificationSlot> &slot, const std::string &value) const;

    static const std::map<std::string,
        std::function<void(NotificationPreferencesDatabase *, sptr<NotificationSlot> &, std::string &)>>
        slotMap_;
    static const std::map<std::string, std::function<void(NotificationPreferencesDatabase *,
                                           NotificationPreferencesInfo::BundleInfo &, std::string &)>>
        bundleMap_;

    const DistributedKv::AppId appId_ {APP_ID};
    const DistributedKv::StoreId storeId_ {STORE_ID};
    std::unique_ptr<DistributedKv::SingleKvStore> kvStorePtr_ = nullptr;
    DistributedKv::DistributedKvDataManager dataManager_;
};

}  // namespace Notification
}  // namespace OHOS
#endif  // BASE_NOTIFICATION_ANS_STANDARD_SERVICES_NOTIFICATION_PREFERENCES_DATABASE_H