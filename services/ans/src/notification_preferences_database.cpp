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
#include "notification_preferences_database.h"

#include <iterator>
#include <sstream>
#include <unistd.h>

#include "ans_log_wrapper.h"

#include "uri.h"
namespace OHOS {
namespace Notification {

const std::map<std::string,
    std::function<void(NotificationPreferencesDatabase *, sptr<NotificationSlot> &, std::string &)>>
    NotificationPreferencesDatabase::slotMap_ = {
        {kSlotGroupId,
            std::bind(&NotificationPreferencesDatabase::ParseSlotGroupId, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {kSlotDescription,
            std::bind(&NotificationPreferencesDatabase::ParseSlotDescription, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
        {kSlotLevel,
            std::bind(&NotificationPreferencesDatabase::ParseSlotLevel, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {kSlotShowBadge,
            std::bind(&NotificationPreferencesDatabase::ParseSlotShowBadge, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
        {kSlotEnableLight,
            std::bind(&NotificationPreferencesDatabase::ParseSlotEnableLight, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
        {kSlotEnableVrbration,
            std::bind(&NotificationPreferencesDatabase::ParseSlotEnableVrbration, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
        {kSlotLedLightColor,
            std::bind(&NotificationPreferencesDatabase::ParseSlotLedLightColor, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
        {kSlotLockscreenVisibleness,
            std::bind(&NotificationPreferencesDatabase::ParseSlotLockscreenVisibleness, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
        {kSlotSound,
            std::bind(&NotificationPreferencesDatabase::ParseSlotSound, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {kSlotVibrationSytle,
            std::bind(&NotificationPreferencesDatabase::ParseSlotVibrationSytle, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
        {kSlotEnableBypassDnd,
            std::bind(&NotificationPreferencesDatabase::ParseSlotEnableBypassDnd, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)}};

const std::map<std::string,
    std::function<void(NotificationPreferencesDatabase *, NotificationPreferencesInfo::BundleInfo &, std::string &)>>
    NotificationPreferencesDatabase::bundleMap_ = {
        {kBundleName,
            std::bind(&NotificationPreferencesDatabase::ParseBundleName, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {kBundleImportance,
            std::bind(&NotificationPreferencesDatabase::ParseBundleImportance, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
        {kBundleShowBadge,
            std::bind(&NotificationPreferencesDatabase::ParseBundleShowBadge, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
        {kBundleBadgeTotalNum,
            std::bind(&NotificationPreferencesDatabase::ParseBundleBadgeNum, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
        {kBundlePrivateAllowed,
            std::bind(&NotificationPreferencesDatabase::ParseBundlePrivateAllowed, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
        {kBundleEnableNotification,
            std::bind(&NotificationPreferencesDatabase::ParseBundleEnableNotification, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)}};

NotificationPreferencesDatabase::NotificationPreferencesDatabase()
{
    TryTwice([this] { return GetKvStore(); });
}

NotificationPreferencesDatabase::~NotificationPreferencesDatabase()
{
    CloseKvStore();
}

void NotificationPreferencesDatabase::TryTwice(const std::function<OHOS::DistributedKv::Status()> &func) const
{
    OHOS::DistributedKv::Status status = func();
    if (status != OHOS::DistributedKv::Status::SUCCESS) {
        status = func();
        ANS_LOGW("distribute database ipc error and try to call again, result = %{public}d", status);
    }
}

OHOS::DistributedKv::Status NotificationPreferencesDatabase::GetKvStore()
{
    OHOS::DistributedKv::Status status;
    OHOS::DistributedKv::Options options = {.createIfMissing = true,
        .encrypt = false,
        .autoSync = true,
        .kvStoreType = OHOS::DistributedKv::KvStoreType::SINGLE_VERSION};
    dataManager_.GetSingleKvStore(options,
        appId_,
        storeId_,
        [this, &status](OHOS::DistributedKv::Status paramStatus,
            std::unique_ptr<OHOS::DistributedKv::SingleKvStore>
                singleKvStore) {
            status = paramStatus;
            if (status != OHOS::DistributedKv::Status::SUCCESS) {
                ANS_LOGE("return error: %{public}d", status);
                return;
            }
            {
                kvStorePtr_ = std::move(singleKvStore);
            }
            ANS_LOGD("get kvStore success");
        });
    return status;
}

void NotificationPreferencesDatabase::CloseKvStore()
{
    dataManager_.CloseKvStore(appId_, std::move(kvStorePtr_));
}

bool NotificationPreferencesDatabase::CheckKvStore()
{
    if (kvStorePtr_ != nullptr) {
        return true;
    }
    TryTwice([this] { return GetKvStore(); });
    return kvStorePtr_ != nullptr;
}

bool NotificationPreferencesDatabase::ResetStore()
{
    kvStorePtr_ = nullptr;
    int32_t tryTimes = MAX_TIMES;
    while (tryTimes > 0) {
        OHOS::DistributedKv::Status status = GetKvStore();
        if (status == OHOS::DistributedKv::Status::SUCCESS && kvStorePtr_ != nullptr) {
            return true;
        }
        ANS_LOGW("CheckKvStore, Times: %{public}d", tryTimes);
        usleep(SLEEP_INTERVAL);
        tryTimes--;
    }
    return false;
}

bool NotificationPreferencesDatabase::PutSlotsToDisturbeDB(
    const std::string &bundleName, const std::vector<sptr<NotificationSlot>> &slots)
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    if (bundleName.empty()) {
        ANS_LOGE("Bundle Name is Null");
        return false;
    }

    if (slots.empty()) {
        ANS_LOGE("Slot is empty");
        return false;
    }

    bool result = true;
    std::vector<OHOS::DistributedKv::Entry> entries;
    for (auto iter : slots) {
        result = SlotToEntry(bundleName, iter, entries);
        if (!result) {
            return result;
        }
    }

    if (!CheckKvStore()) {
        ANS_LOGE("KvStore is nullptr");
        return false;
    }
    OHOS::DistributedKv::Status status = kvStorePtr_->PutBatch(entries);
    return (status == OHOS::DistributedKv::Status::SUCCESS);
}

bool NotificationPreferencesDatabase::PutGroupsToDisturbeDB(
    const std::string &bundleName, const std::vector<sptr<NotificationSlotGroup>> &groups)
{
    if (bundleName.empty()) {
        ANS_LOGE("Bundle Name is Null");
        return false;
    }

    if (groups.empty()) {
        ANS_LOGE("Slot is empty");
        return false;
    }

    bool result = true;
    std::vector<OHOS::DistributedKv::Entry> entries;
    for (auto iter : groups) {
        result = GroupToEntry(bundleName, iter, entries);
        if (!result) {
            return result;
        }
    }

    if (!CheckKvStore()) {
        ANS_LOGE("KvStore is nullptr");
        return false;
    }
    OHOS::DistributedKv::Status status = kvStorePtr_->PutBatch(entries);
    return (status == OHOS::DistributedKv::Status::SUCCESS);
}

bool NotificationPreferencesDatabase::PutShowBadge(const std::string &bundleName, const bool &enable)
{
    if (bundleName.empty()) {
        ANS_LOGE("Bundle Name is Null");
        return false;
    }

    if (!CheckBundle(bundleName)) {
        return false;
    }
    OHOS::DistributedKv::Status status =
        PutBundlePropertyToDisturbeDB(bundleName, BundleType::kBundleShowBadgeType, enable);
    return (status == OHOS::DistributedKv::Status::SUCCESS);
}

bool NotificationPreferencesDatabase::PutImportance(const std::string &bundleName, const int &importance)
{
    if (bundleName.empty()) {
        ANS_LOGE("Bundle Name is Null");
        return false;
    }

    if (!CheckBundle(bundleName)) {
        return false;
    }
    OHOS::DistributedKv::Status status =
        PutBundlePropertyToDisturbeDB(bundleName, BundleType::kBundleImportanceType, importance);
    return (status == OHOS::DistributedKv::Status::SUCCESS);
}

bool NotificationPreferencesDatabase::PutTotalBadgeNums(const std::string &bundleName, const int &totalBadgeNum)
{
    if (bundleName.empty()) {
        ANS_LOGE("Bundle Name is Null");
        return false;
    }

    if (!CheckBundle(bundleName)) {
        return false;
    }

    OHOS::DistributedKv::Status status =
        PutBundlePropertyToDisturbeDB(bundleName, BundleType::kBundleBadgeTotalNumType, totalBadgeNum);
    return (status == OHOS::DistributedKv::Status::SUCCESS);
}

bool NotificationPreferencesDatabase::PutPrivateNotificationsAllowed(const std::string &bundleName, const bool &allow)
{
    if (bundleName.empty()) {
        ANS_LOGE("Bundle Name is Null");
        return false;
    }

    if (!CheckBundle(bundleName)) {
        return false;
    }
    OHOS::DistributedKv::Status status =
        PutBundlePropertyToDisturbeDB(bundleName, BundleType::kBundlePrivateAllowedType, allow);

    return (status == OHOS::DistributedKv::Status::SUCCESS);
}

bool NotificationPreferencesDatabase::PutNotificationsEnabledForBundle(
    const std::string &bundleName, const bool &enabled)
{
    if (bundleName.empty()) {
        ANS_LOGE("Bundle Name is Null");
        return false;
    }

    if (!CheckBundle(bundleName)) {
        return false;
    }

    OHOS::DistributedKv::Status status =
        PutBundlePropertyToDisturbeDB(bundleName, BundleType::kBundleEnableNotificationType, enabled);
    return (status == OHOS::DistributedKv::Status::SUCCESS);
}

bool NotificationPreferencesDatabase::PutNotificationsEnabled(const bool &enabled)
{
    if (!CheckKvStore()) {
        ANS_LOGE("KvStore is nullptr");
        return false;
    }

    OHOS::DistributedKv::Key enableKey(kEnable);
    OHOS::DistributedKv::Value enableValue(std::to_string(enabled));
    OHOS::DistributedKv::Status status;
    status = kvStorePtr_->Put(enableKey, enableValue);
    if (status != OHOS::DistributedKv::Status::SUCCESS) {
        ANS_LOGE("Store enable notification failed. %{public}d", status);
        return false;
    }
    return true;
}

bool NotificationPreferencesDatabase::PutDisturbMode(const NotificationConstant::DisturbMode &mode)
{
    if (!CheckKvStore()) {
        ANS_LOGE("KvStore is nullptr");
        return false;
    }

    OHOS::DistributedKv::Key disturbModeKey(kDisturbMode);
    OHOS::DistributedKv::Value disturbModeValue(std::to_string(mode));
    OHOS::DistributedKv::Status status;
    status = kvStorePtr_->Put(disturbModeKey, disturbModeValue);
    if (status != OHOS::DistributedKv::Status::SUCCESS) {
        ANS_LOGE("Store disturbe modeFailed. %{public}d", status);
        return false;
    }
    return true;
}

void NotificationPreferencesDatabase::GetValueFromDisturbeDB(
    const std::string &key, std::function<void(OHOS::DistributedKv::Value &)> funcion)
{
    if (!CheckKvStore()) {
        ANS_LOGE("KvStore is nullptr");
        return;
    }

    OHOS::DistributedKv::Status status;
    OHOS::DistributedKv::Value value;
    OHOS::DistributedKv::Key getKey(key);
    status = kvStorePtr_->Get(getKey, value);
    if (status != OHOS::DistributedKv::Status::SUCCESS) {
        ANS_LOGE("Get value failed, use defalut value. error code is %{public}d", status);
        return;
    }

    if (value.Empty()) {
        ANS_LOGE("Get value is empty, use defalut value. error code is %{public}d", value.Empty());
        return;
    }
    funcion(value);
}

void NotificationPreferencesDatabase::GetValueFromDisturbeDB(
    const std::string &key, std::function<void(OHOS::DistributedKv::Status &, OHOS::DistributedKv::Value &)> funcion)
{
    if (!CheckKvStore()) {
        ANS_LOGE("KvStore is nullptr");
        return;
    }

    OHOS::DistributedKv::Status status;
    OHOS::DistributedKv::Value value;
    OHOS::DistributedKv::Key getKey(key);
    status = kvStorePtr_->Get(getKey, value);
    funcion(status, value);
}

bool NotificationPreferencesDatabase::CheckBundle(const std::string &bundleName)
{
    std::string bundleKeyStr = kBundleLabel + bundleName;
    bool result = true;
    GetValueFromDisturbeDB(bundleKeyStr, [&](OHOS::DistributedKv::Status &status, OHOS::DistributedKv::Value &value) {
        switch (status) {
            case OHOS::DistributedKv::Status::KEY_NOT_FOUND: {
                result = PutBundleToDisturbeDB(bundleName);
                break;
            }
            case OHOS::DistributedKv::Status::SUCCESS: {
                result = true;
                break;
            }
            default:
                result = false;
                break;
        }
    });
    return result;
}

bool NotificationPreferencesDatabase::InitBundlePropertyValueToDisturbeDB(const std::string &bundleName)
{
    std::vector<OHOS::DistributedKv::Entry> entries;
    GenerateEntry(GenerateBundleKey(bundleName, kBundleName), bundleName, entries);
    GenerateEntry(GenerateBundleKey(bundleName, kBundleBadgeTotalNum), std::to_string(BUNDLE_BADGE_TOTAL_NUM), entries);
    GenerateEntry(GenerateBundleKey(bundleName, kBundleImportance), std::to_string(BUNDLE_IMPORTANCE), entries);
    GenerateEntry(GenerateBundleKey(bundleName, kBundleShowBadge), std::to_string(BUNDLE_SHOW_BADGE), entries);
    GenerateEntry(
        GenerateBundleKey(bundleName, kBundlePrivateAllowed), std::to_string(BUNDLE_PRIVATE_ALLOWED), entries);
    GenerateEntry(
        GenerateBundleKey(bundleName, kBundleEnableNotification), std::to_string(BUNDLE_ENABLE_NOTIFICATION), entries);
    if (!CheckKvStore()) {
        ANS_LOGE("KvStore is nullptr");
        return false;
    }
    OHOS::DistributedKv::Status status = kvStorePtr_->PutBatch(entries);
    if (status != OHOS::DistributedKv::Status::SUCCESS) {
        ANS_LOGE("Store bundle failed. %{public}d", status);
        return false;
    }
    return true;
}

bool NotificationPreferencesDatabase::ParseFromDisturbeDB(NotificationPreferencesInfo &info)
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    ParseDisturbeMode(info);
    ParseEnableAllNotification(info);

    if (!CheckKvStore()) {
        ANS_LOGE("KvStore is nullptr");
        return false;
    }
    OHOS::DistributedKv::Status status;
    std::vector<OHOS::DistributedKv::Entry> entries;
    status = kvStorePtr_->GetEntries({kBundleLabel}, entries);
    if (status != OHOS::DistributedKv::Status::SUCCESS) {
        ANS_LOGE("Get Bundle Info failed.");
        return false;
    }
    ParseBundleFromDistureDB(info, entries);
    return true;
}

bool NotificationPreferencesDatabase::RemoveAllBundleFromDisturbeDB()
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    if (!CheckKvStore()) {
        ANS_LOGE("KvStore is nullptr");
        return false;
    }
    OHOS::DistributedKv::Status status = dataManager_.DeleteKvStore(appId_, storeId_);
    return (status == OHOS::DistributedKv::Status::SUCCESS);
}

bool NotificationPreferencesDatabase::RemoveBundleFromDisturbeDB(const std::string &bundleName)
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    if (!CheckKvStore()) {
        ANS_LOGE("KvStore is nullptr");
        return false;
    }

    OHOS::DistributedKv::Status status;
    std::vector<OHOS::DistributedKv::Entry> entries;
    status = kvStorePtr_->GetEntries({kAnsBundle + kUnderLine + bundleName + kUnderLine}, entries);

    if (status != OHOS::DistributedKv::Status::SUCCESS) {
        ANS_LOGE("Get Bundle Info failed.");
        return false;
    }

    std::vector<OHOS::DistributedKv::Key> keys;
    for (auto iter : entries) {
        keys.push_back(iter.key);
    }

    OHOS::DistributedKv::Key bundleKey({kBundleLabel + kBundleName + kUnderLine + bundleName});
    keys.push_back(bundleKey);
    status = kvStorePtr_->DeleteBatch(keys);
    if (status != OHOS::DistributedKv::Status::SUCCESS) {
        ANS_LOGE("delete bundle Info failed.");
        return false;
    }
    return true;
}

bool NotificationPreferencesDatabase::RemoveSlotFromDisturbeDB(
    const std::string &bundleName, const NotificationConstant::SlotType &type)
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    if (bundleName.empty()) {
        ANS_LOGE("Bundle Name is Null");
        return false;
    }

    if (!CheckKvStore()) {
        ANS_LOGE("KvStore is nullptr");
        return false;
    }

    OHOS::DistributedKv::Status status;
    std::vector<OHOS::DistributedKv::Entry> slotentries;
    std::string slotType = kSlotType + std::to_string(type);
    status = kvStorePtr_->GetEntries({GenerateSlotKey(bundleName, slotType) + kUnderLine}, slotentries);
    if (status != DistributedKv::Status::SUCCESS) {
        return false;
    }
    std::vector<OHOS::DistributedKv::Key> keys;
    for (auto iter : slotentries) {
        keys.push_back(iter.key);
    }

    std::string slotTypeKey = GenerateSlotKey(bundleName, kSlotType, std::to_string(type));
    OHOS::DistributedKv::Key slotIdKey({slotTypeKey});
    keys.push_back(slotIdKey);
    status = kvStorePtr_->DeleteBatch(keys);
    if (status != OHOS::DistributedKv::Status::SUCCESS) {
        ANS_LOGE("delete bundle Info failed.");
        return false;
    }

    ANS_LOGD("%{public}s remove status %{public}d", __FUNCTION__, status);

    return true;
}

bool NotificationPreferencesDatabase::RemoveGroupsFromDisturbeDB(
    const std::string &bundleName, const std::vector<std::string> &groupIds)
{
    if (bundleName.empty()) {
        ANS_LOGE("Bundle Name is Null");
        return false;
    }

    if (!CheckKvStore()) {
        ANS_LOGE("KvStore is nullptr");
        return false;
    }

    std::vector<OHOS::DistributedKv::Key> keys;
    bool result = true;
    for (auto iter : groupIds) {
        result = GetRemoveGroupKeysFromDisturbeDB(bundleName, iter, keys);
        if (!result) {
            return result;
        }
    }

    OHOS::DistributedKv::Status status = kvStorePtr_->DeleteBatch(keys);
    if (status != OHOS::DistributedKv::Status::SUCCESS) {
        ANS_LOGE("delete bundle Info failed.");
        return false;
    }
    return result;
}

bool NotificationPreferencesDatabase::GetRemoveGroupKeysFromDisturbeDB(
    const std::string &bundleName, const std::string &groupId, std::vector<OHOS::DistributedKv::Key> &keys)
{
    if (!CheckKvStore()) {
        ANS_LOGE("KvStore is nullptr");
        return false;
    }

    OHOS::DistributedKv::Status status;
    std::vector<OHOS::DistributedKv::Entry> groupentries;
    std::string slotKeyStr = GenerateGroupKey(bundleName, groupId);
    status = kvStorePtr_->GetEntries({slotKeyStr + kUnderLine}, groupentries);
    if (status != DistributedKv::Status::SUCCESS) {
        return false;
    }
    for (auto iter : groupentries) {
        keys.push_back(iter.key);
    }

    OHOS::DistributedKv::Key groupIdKey({GenerateGroupKey(bundleName, kGroupId + kUnderLine + groupId)});
    keys.push_back(groupIdKey);

    return true;
}

void NotificationPreferencesDatabase::SubscribeDeathRecipient(
    const std::shared_ptr<DistributedKv::KvStoreDeathRecipient> &callback)
{
    dataManager_.RegisterKvStoreServiceDeathRecipient(callback);
}

void NotificationPreferencesDatabase::UnsubscribeDeathRecipient(
    const std::shared_ptr<DistributedKv::KvStoreDeathRecipient> &callback)
{
    dataManager_.UnRegisterKvStoreServiceDeathRecipient(callback);
}

bool NotificationPreferencesDatabase::StoreDeathRecipient(NotificationPreferencesInfo &info)
{
    ANS_LOGW("distribute remote died");
    bool result = ResetStore();
    if (result) {
        ParseFromDisturbeDB(info);
    } else {
        ANS_LOGE("db try connect failed");
    }
    return result;
}

template <typename T>
OHOS::DistributedKv::Status NotificationPreferencesDatabase::PutBundlePropertyToDisturbeDB(
    const std::string &bundleName, const BundleType &type, const T &t)
{
    std::string keyStr;
    switch (type) {
        case BundleType::kBundleBadgeTotalNumType:
            keyStr = GenerateBundleKey(bundleName, kBundleBadgeTotalNum);
            break;
        case BundleType::kBundleImportanceType:
            keyStr = GenerateBundleKey(bundleName, kBundleImportance);
            break;
        case BundleType::kBundleShowBadgeType:
            keyStr = GenerateBundleKey(bundleName, kBundleShowBadge);
            break;
        case BundleType::kBundlePrivateAllowedType:
            keyStr = GenerateBundleKey(bundleName, kBundlePrivateAllowed);
            break;
        case BundleType::kBundleEnableNotificationType:
            keyStr = GenerateBundleKey(bundleName, kBundleEnableNotification);
            break;
        default:
            break;
    }
    OHOS::DistributedKv::Status status;
    OHOS::DistributedKv::Key key(keyStr);
    OHOS::DistributedKv::Value value(std::to_string(t));
    if (!CheckKvStore()) {
        ANS_LOGE("KvStore is nullptr");
        status = OHOS::DistributedKv::Status::ERROR;
        return status;
    }
    status = kvStorePtr_->Put(key, value);
    return status;
}

bool NotificationPreferencesDatabase::PutBundleToDisturbeDB(const std::string &bundleName)
{
    if (!CheckKvStore()) {
        ANS_LOGE("KvStore is nullptr");
        return false;
    }

    ANS_LOGD("Key not fund, so create a bundle. bundle key is %{public}s", bundleName.c_str());
    OHOS::DistributedKv::Key bundleKey(kBundleLabel + bundleName);
    OHOS::DistributedKv::Value bundleValue(bundleName);
    if (kvStorePtr_->Put(bundleKey, bundleValue) != OHOS::DistributedKv::Status::SUCCESS) {
        ANS_LOGE("Store bundle name to db is failed.");
        return false;
    }

    if (!InitBundlePropertyValueToDisturbeDB(bundleName)) {
        return false;
    }

    return true;
}

void NotificationPreferencesDatabase::GenerateEntry(
    const std::string &key, const std::string &value, std::vector<OHOS::DistributedKv::Entry> &entries)
{
    OHOS::DistributedKv::Entry entry;
    OHOS::DistributedKv::Key dbKey(key);
    OHOS::DistributedKv::Value dbValue(value);
    entry.key = dbKey;
    entry.value = dbValue;
    entries.push_back(entry);
}

bool NotificationPreferencesDatabase::SlotToEntry(
    const std::string &bundleName, const sptr<NotificationSlot> &slot, std::vector<OHOS::DistributedKv::Entry> &entries)
{
    if (slot == nullptr) {
        ANS_LOGE("Notification group is nullptr");
        return false;
    }

    if (!CheckBundle(bundleName)) {
        return false;
    }

    GenerateSlotEntry(bundleName, slot, entries);
    return true;
}

void NotificationPreferencesDatabase::GenerateSlotEntry(
    const std::string &bundleName, const sptr<NotificationSlot> &slot, std::vector<OHOS::DistributedKv::Entry> &entries)
{
    std::string slotType = kSlotType + kUnderLine + std::to_string(slot->GetType());
    GenerateEntry(GenerateSlotKey(bundleName, slotType, kSlotType), std::to_string(slot->GetType()), entries);
    GenerateEntry(GenerateSlotKey(bundleName, slotType, kSlotId), slot->GetId(), entries);
    GenerateEntry(GenerateSlotKey(bundleName, slotType, kSlotGroupId), slot->GetSlotGroup(), entries);
    GenerateEntry(GenerateSlotKey(bundleName, slotType, kSlotName), slot->GetName(), entries);
    GenerateEntry(GenerateSlotKey(bundleName, slotType, kSlotDescription), slot->GetDescription(), entries);
    GenerateEntry(GenerateSlotKey(bundleName, slotType, kSlotLevel), std::to_string(slot->GetLevel()), entries);
    GenerateEntry(GenerateSlotKey(bundleName, slotType, kSlotShowBadge), std::to_string(slot->IsShowBadge()), entries);
    GenerateEntry(
        GenerateSlotKey(bundleName, slotType, kSlotEnableLight), std::to_string(slot->CanEnableLight()), entries);
    GenerateEntry(
        GenerateSlotKey(bundleName, slotType, kSlotEnableVrbration), std::to_string(slot->CanVibrate()), entries);
    GenerateEntry(
        GenerateSlotKey(bundleName, slotType, kSlotLedLightColor), std::to_string(slot->GetLedLightColor()), entries);
    GenerateEntry(GenerateSlotKey(bundleName, slotType, kSlotLockscreenVisibleness),
        std::to_string(static_cast<int>(slot->GetLockScreenVisibleness())),
        entries);
    GenerateEntry(GenerateSlotKey(bundleName, slotType, kSlotSound), slot->GetSound().ToString(), entries);
    GenerateEntry(GenerateSlotKey(bundleName, slotType, kSlotEnableBypassDnd),
        std::to_string(slot->IsEnableBypassDnd()),
        entries);
    GenerateEntry(
        GenerateSlotKey(bundleName, slotType, kSlotVibrationSytle), VectorToString(slot->GetVibrationStyle()), entries);
}

bool NotificationPreferencesDatabase::GroupToEntry(const std::string &bundleName,
    const sptr<NotificationSlotGroup> &group, std::vector<OHOS::DistributedKv::Entry> &entries)
{

    if (group == nullptr) {
        ANS_LOGE("Notification group is nullptr");
        return false;
    }

    if (!CheckBundle(bundleName)) {
        return false;
    }

    GenerateGroupEntry(bundleName, group, entries);
    return true;
}

void NotificationPreferencesDatabase::GenerateGroupEntry(const std::string &bundleName,
    const sptr<NotificationSlotGroup> &group, std::vector<OHOS::DistributedKv::Entry> &entries)
{
    std::string groupId = kGroupId + kUnderLine + group->GetId();
    std::string groupLebal = group->GetId().append(kUnderLine);
    GenerateEntry(GenerateGroupKey(bundleName, groupId), group->GetId(), entries);
    GenerateEntry(GenerateGroupKey(bundleName, groupLebal + kGroupName), group->GetName(), entries);
    GenerateEntry(GenerateGroupKey(bundleName, groupLebal + kGroupDescription), group->GetDescription(), entries);
    GenerateEntry(
        GenerateGroupKey(bundleName, groupLebal + kGroupDisable), std::to_string(group->IsDisabled()), entries);
}

void NotificationPreferencesDatabase::ParseBundleFromDistureDB(
    NotificationPreferencesInfo &info, const std::vector<OHOS::DistributedKv::Entry> &entries)
{
    if (!CheckKvStore()) {
        ANS_LOGE("KvStore is nullptr");
        return;
    }
    for (auto item : entries) {
        std::string bundleName = item.value.ToString();
        ANS_LOGD("bundle name is %{public}s", bundleName.c_str());
        std::vector<OHOS::DistributedKv::Entry> bundleEntries;
        kvStorePtr_->GetEntries({GenerateBundleKey(bundleName)}, bundleEntries);
        ANS_LOGD("bundle key is %{public}s", GenerateBundleKey(bundleName).c_str());
        NotificationPreferencesInfo::BundleInfo bunldeInfo;
        for (auto bundleEntry : bundleEntries) {
            if (bundleEntry.key.ToString().find(kSlot) != std::string::npos) {
                ParseSlotFromDisturbeDB(bunldeInfo, bundleName, bundleEntry);
            } else if (bundleEntry.key.ToString().find(kGroup) != std::string::npos) {
                ParseGroupFromDisturbeDB(bunldeInfo, bundleName, bundleEntry);
            } else {
                ParseBundlePropertyFromDisturbeDB(bunldeInfo, bundleEntry.key, bundleEntry.value);
            }
        }

        info.SetBundleInfo(bunldeInfo);
    }
}

void NotificationPreferencesDatabase::ParseSlotFromDisturbeDB(NotificationPreferencesInfo::BundleInfo &bundleInfo,
    const std::string &bundleName, const OHOS::DistributedKv::Entry &entry)
{
    std::string slotKey = entry.key.ToString();

    NotificationConstant::SlotType slotType =
        static_cast<NotificationConstant::SlotType>(stoi(SubUniqueIdentifyFromString(slotKey), nullptr));
    sptr<NotificationSlot> slot = nullptr;
    if (!bundleInfo.GetSlot(slotType, slot)) {
        slot = new NotificationSlot(slotType);
    }
    ParseSlot(slot, entry);
    bundleInfo.SetSlot(slot);
}

void NotificationPreferencesDatabase::ParseGroupFromDisturbeDB(NotificationPreferencesInfo::BundleInfo &bundleInfo,
    const std::string &bundleName, const OHOS::DistributedKv::Entry &entry)
{
    if (!CheckKvStore()) {
        ANS_LOGE("KvStore is nullptr");
        return;
    }
    std::string groupKey = entry.key.ToString();
    std::string groupId = SubUniqueIdentifyFromString(groupKey);
    sptr<NotificationSlotGroup> group;
    if (!bundleInfo.GetGroup(groupId, group)) {
        std::string groupName;
        std::string groupNameKey = GenerateGroupKey(bundleName, groupId + kUnderLine + kGroupName);
        GetValueFromDisturbeDB(
            groupNameKey, [&groupName](OHOS::DistributedKv::Value &value) { groupName = value.ToString(); });
        if (groupName.empty()) {
            ANS_LOGE("Group name does not exsited.");
            return;
        }
        group = new NotificationSlotGroup(groupId, groupName);
    }

    ParseGroupDescription(group, entry);
    bundleInfo.SetGroup(group);
}

void NotificationPreferencesDatabase::ParseBundlePropertyFromDisturbeDB(
    NotificationPreferencesInfo::BundleInfo &bundleInfo, const OHOS::DistributedKv::Key &key,
    const OHOS::DistributedKv::Value &value)
{
    std::string typeStr = FindLastString(key.ToString());
    std::string valueStr = value.ToString();
    ANS_LOGD("db key = %{public}s and type str %{public}s", key.ToString().c_str(), typeStr.c_str());

    auto iter = bundleMap_.find(typeStr);
    if (iter != bundleMap_.end()) {
        auto func = iter->second;
        func(this, bundleInfo, valueStr);
    }
}

void NotificationPreferencesDatabase::ParseSlot(sptr<NotificationSlot> &slot, const OHOS::DistributedKv::Entry &entry)
{
    std::string typeStr = FindLastString(entry.key.ToString());
    std::string valueStr = entry.value.ToString();
    ANS_LOGD("db key = %{public}s , type str %{public}s ,  value is %{public}s ",
        entry.key.ToString().c_str(),
        typeStr.c_str(),
        entry.value.ToString().c_str());

    auto iter = slotMap_.find(typeStr);
    if (iter != slotMap_.end()) {
        auto func = iter->second;
        func(this, slot, valueStr);
    }
}

std::string NotificationPreferencesDatabase::FindLastString(const std::string &inputString) const
{
    std::string keyStr;
    size_t pos = inputString.find_last_of(kUnderLine);
    if (pos != std::string::npos) {
        keyStr = inputString.substr(pos + 1);
    }
    return keyStr;
}

std::string NotificationPreferencesDatabase::VectorToString(const std::vector<int64_t> &data) const
{
    std::stringstream streamStr;
    std::copy(data.begin(), data.end(), std::ostream_iterator<int>(streamStr, kUnderLine.c_str()));
    return streamStr.str();
}

void NotificationPreferencesDatabase::StringToVector(const std::string &str, std::vector<int64_t> &data) const
{
    if (str.empty()) {
        return;
    }

    if (str.find_first_of(kUnderLine) != std::string::npos) {
        std::string str1 = str.substr(0, str.find_first_of(kUnderLine));
        std::string afterStr = str.substr(str.find_first_of(kUnderLine) + 1);
        data.push_back(stoi(str1, nullptr));
        StringToVector(afterStr, data);
    }
}

std::string NotificationPreferencesDatabase::GenerateSlotKey(
    const std::string &bundleKey, const std::string &type, const std::string &subType) const
{
    /*slot key
     *
     * ans_bundle_bundlename_slot_type_0_id
     * ans_bundle_bundlename_slot_type_0_des
     * ans_bundle_bundlename_slot_type_1_id
     * ans_bundle_bundlename_slot_type_1_des
     *
     */
    std::string key = GenerateBundleKey(bundleKey).append(kSlot);
    if (!type.empty()) {
        key.append(kUnderLine).append(type);
    }
    if (!subType.empty()) {
        key.append(kUnderLine).append(subType);
    }
    ANS_LOGD("Slot key is : %{public}s", key.c_str());
    return key;
}

std::string NotificationPreferencesDatabase::GenerateGroupKey(
    const std::string &bundleKey, const std::string &type) const
{
    /*group key
     *
     * ans_bundle_bundlename_group_group_id0_name
     * ans_bundle_bundlename_group_group_id1_name
     *
     */
    std::string key = GenerateBundleKey(bundleKey).append(kGroup);
    if (!type.empty()) {
        key.append(kUnderLine).append(type);
    }
    ANS_LOGD("Group key is %{public}s", key.c_str());
    return key;
}

std::string NotificationPreferencesDatabase::GenerateBundleKey(
    const std::string &bundleName, const std::string &type) const
{
    /*bundle key
     *
     * label_ans_bundle_name = ""
     * ans_bundle_bundlename_
     * ans_bundle_bundlename_
     * ans_bundle_bundlename_
     * ans_bundle_bundlename_
     *
     */
    std::string key = std::string().append(kAnsBundle).append(kUnderLine).append(bundleName).append(kUnderLine);
    if (!type.empty()) {
        key.append(type);
    }
    ANS_LOGD("Bundle key : %{public}s", key.c_str());
    return key;
}

std::string NotificationPreferencesDatabase::SubUniqueIdentifyFromString(const std::string &keyStr) const
{
    std::string slotType;
    size_t pos = keyStr.find_last_of(kUnderLine);
    if (pos != std::string::npos) {
        slotType = FindLastString(keyStr.substr(0, pos));
    }

    return slotType;
}

void NotificationPreferencesDatabase::ParseDisturbeMode(NotificationPreferencesInfo &info)
{
    GetValueFromDisturbeDB(kDisturbMode, [&](OHOS::DistributedKv::Status &status, OHOS::DistributedKv::Value &value) {
        if (status == OHOS::DistributedKv::Status::KEY_NOT_FOUND) {
            PutDisturbMode(info.GetDisturbMode());
        } else if (status == OHOS::DistributedKv::Status::SUCCESS) {
            if (!value.ToString().empty()) {
                info.SetDisturbMode(static_cast<NotificationConstant::DisturbMode>(stoi(value.ToString(), nullptr)));
            }
        } else {
            ANS_LOGD("Parse disturbe mode failed, use defalut value.");
        }
    });
}

void NotificationPreferencesDatabase::ParseEnableAllNotification(NotificationPreferencesInfo &info)
{
    GetValueFromDisturbeDB(kEnable, [&](OHOS::DistributedKv::Status &status, OHOS::DistributedKv::Value &value) {
        if (status == OHOS::DistributedKv::Status::KEY_NOT_FOUND) {
            PutNotificationsEnabled(info.GetEnabledAllNotification());
        } else if (status == OHOS::DistributedKv::Status::SUCCESS) {
            if (!value.ToString().empty()) {
                info.SetEnabledAllNotification(static_cast<bool>(stoi(value.ToString(), nullptr)));
            }
        } else {
            ANS_LOGD("Parse enable all notification failed, use defalut value.");
        }
    });
}

void NotificationPreferencesDatabase::ParseGroupDescription(
    sptr<NotificationSlotGroup> &group, const OHOS::DistributedKv::Entry &entry)
{
    std::string typeStr = FindLastString(entry.key.ToString());
    std::string valueStr = entry.value.ToString();
    if (!typeStr.compare(kGroupDescription)) {
        ANS_LOGD("SetGroupDescription is %{public}s", valueStr.c_str());
        group->SetDescription(valueStr);
    }
}

void NotificationPreferencesDatabase::ParseBundleName(
    NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &value) const
{
    ANS_LOGD("SetBundleName bundle name is %{public}s", value.c_str());
    bundleInfo.SetBundleName(value);
}

void NotificationPreferencesDatabase::ParseBundleImportance(
    NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &value) const
{
    ANS_LOGD("SetBundleImportance bundle importance is %{public}s", value.c_str());
    bundleInfo.SetImportance(static_cast<NotificationSlot::NotificationLevel>(stoi(value, nullptr)));
}

void NotificationPreferencesDatabase::ParseBundleShowBadge(
    NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &value) const
{
    ANS_LOGD("SetBundleShowBadge bundle show badge is %{public}s", value.c_str());
    bundleInfo.SetIsShowBadge(static_cast<bool>(stoi(value, nullptr)));
}

void NotificationPreferencesDatabase::ParseBundleBadgeNum(
    NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &value) const
{
    ANS_LOGD("SetBundleBadgeNum bundle badge num is %{public}s", value.c_str());
    bundleInfo.SetBadgeTotalNum(stoi(value, nullptr));
}

void NotificationPreferencesDatabase::ParseBundlePrivateAllowed(
    NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &value) const
{
    ANS_LOGD("SetBundlePrivateAllowed bundle private allowed is %{public}s", value.c_str());
    bundleInfo.SetIsPrivateAllowed(static_cast<bool>(stoi(value, nullptr)));
}

void NotificationPreferencesDatabase::ParseBundleEnableNotification(
    NotificationPreferencesInfo::BundleInfo &bundleInfo, const std::string &value) const
{
    ANS_LOGD("SetBundleEnableNotification bundle enable is %{public}s", value.c_str());
    bundleInfo.SetEnableNotification(static_cast<bool>(stoi(value, nullptr)));
}

void NotificationPreferencesDatabase::ParseSlotGroupId(sptr<NotificationSlot> &slot, const std::string &value) const
{
    ANS_LOGD("slot group id is %{public}s", value.c_str());
    std::string slotGroupId = value;
    slot->SetSlotGroup(slotGroupId);
}

void NotificationPreferencesDatabase::ParseSlotDescription(sptr<NotificationSlot> &slot, const std::string &value) const
{
    ANS_LOGD("slot des is %{public}s", value.c_str());
    std::string slotDescription = value;
    slot->SetDescription(slotDescription);
}

void NotificationPreferencesDatabase::ParseSlotLevel(sptr<NotificationSlot> &slot, const std::string &value) const
{
    ANS_LOGD("slot level is %{public}s", value.c_str());
    NotificationSlot::NotificationLevel level = static_cast<NotificationSlot::NotificationLevel>(stoi(value, nullptr));
    slot->SetLevel(level);
}

void NotificationPreferencesDatabase::ParseSlotShowBadge(sptr<NotificationSlot> &slot, const std::string &value) const
{
    ANS_LOGD("slot show badge is %{public}s", value.c_str());
    bool showBadge = static_cast<bool>(stoi(value, nullptr));
    slot->EnableBadge(showBadge);
}

void NotificationPreferencesDatabase::ParseSlotEnableLight(sptr<NotificationSlot> &slot, const std::string &value) const
{
    ANS_LOGD("slot enable light is %{public}s", value.c_str());
    bool enableLight = static_cast<bool>(stoi(value, nullptr));
    slot->SetEnableLight(enableLight);
}

void NotificationPreferencesDatabase::ParseSlotEnableVrbration(
    sptr<NotificationSlot> &slot, const std::string &value) const
{
    ANS_LOGD("slot enable vir is %{public}s", value.c_str());
    bool enableVrbration = static_cast<bool>(stoi(value, nullptr));
    slot->SetEnableVibration(enableVrbration);
}

void NotificationPreferencesDatabase::ParseSlotLedLightColor(
    sptr<NotificationSlot> &slot, const std::string &value) const
{
    ANS_LOGD("slot led is %{public}s", value.c_str());
    int32_t ledLightColor = static_cast<int32_t>(stoi(value, nullptr));
    slot->SetLedLightColor(ledLightColor);
}

void NotificationPreferencesDatabase::ParseSlotLockscreenVisibleness(
    sptr<NotificationSlot> &slot, const std::string &value) const
{

    ANS_LOGD("slot visible is %{public}s", value.c_str());
    NotificationConstant::VisiblenessType visible =
        static_cast<NotificationConstant::VisiblenessType>(stoi(value, nullptr));
    slot->SetLockscreenVisibleness(visible);
}

void NotificationPreferencesDatabase::ParseSlotSound(sptr<NotificationSlot> &slot, const std::string &value) const
{
    std::string slotUri = value;
    Uri uri(slotUri);
    slot->SetSound(uri);
}

void NotificationPreferencesDatabase::ParseSlotVibrationSytle(
    sptr<NotificationSlot> &slot, const std::string &value) const
{
    std::vector<int64_t> vibrationStyle;
    StringToVector(value, vibrationStyle);
    slot->SetVibrationStyle(vibrationStyle);
}

void NotificationPreferencesDatabase::ParseSlotEnableBypassDnd(
    sptr<NotificationSlot> &slot, const std::string &value) const
{
    ANS_LOGD("slot by pass dnd is %{public}s", value.c_str());
    bool enable = static_cast<bool>(stoi(value, nullptr));
    slot->EnableBypassDnd(enable);
}
}  // namespace Notification
}  // namespace OHOS