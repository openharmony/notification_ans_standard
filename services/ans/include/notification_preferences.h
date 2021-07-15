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

#ifndef NOTIFICATION_PREFERENCES_H
#define NOTIFICATION_PREFERENCES_H

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"
#include "refbase.h"
#include "singleton.h"

#include "ans_inner_errors.h"
#include "notification_slot.h"
#include "notification_slot_group.h"

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
    ErrCode SetImportance(const std::string &bundleName, const int importance);
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

private:
    struct BundleInfo {
        std::string bundleName {};
        int importance = NotificationSlot::NotificationLevel::LEVEL_DEFAULT;
        bool isShowBadge = false;
        int badgeTotalNum = 0;
        bool isPrivateAllowed = false;
        bool isEnabledNotification = true;
        std::map<std::string, sptr<NotificationSlotGroup>> groups {};
        std::map<NotificationConstant::SlotType, sptr<NotificationSlot>> slots {};
    };
    struct PreferencesInfo {
        bool isEnabledAllNotification = true;
        NotificationConstant::DisturbMode disturbMode = NotificationConstant::DisturbMode::ALLOW_ALL;
        std::map<std::string, BundleInfo> infos {};
    };
    enum class BundlePropertyType;

    ErrCode CreateSlot(
        const std::string &bundleName, const NotificationConstant::SlotType &type, sptr<NotificationSlot> &slot);
    ErrCode CheckSlotForCreateSlot(const std::string &bundleName, const sptr<NotificationSlot> &slot,
        PreferencesInfo &preferencesInfo, nlohmann::json &paraseJson) const;
    ErrCode CheckGroupForCreateSlotGroup(const std::string &bundleName, const sptr<NotificationSlotGroup> &group,
        PreferencesInfo &preferencesInfo, nlohmann::json &paraseJson) const;
    ErrCode CheckSlotForRemoveSlot(const std::string &bundleName, const NotificationConstant::SlotType &slotType,
        PreferencesInfo &preferencesInfo, nlohmann::json &paraseJson) const;
    ErrCode CheckGroupForRemoveSlotGroup(const std::string &bundleName, const std::string &groupId,
        PreferencesInfo &preferencesInfo, nlohmann::json &paraseJson) const;
    ErrCode CheckSlotForUpdateSlot(const std::string &bundleName, const sptr<NotificationSlot> &slot,
        PreferencesInfo &preferencesInfo, nlohmann::json &paraseJson) const;
    ErrCode CheckGroupForUpdateSlotGroup(const std::string &bundleName, const sptr<NotificationSlotGroup> &group,
        PreferencesInfo &preferencesInfo, nlohmann::json &paraseJson) const;
    template <typename T>
    void SetBundleProperty(PreferencesInfo &preferencesInfo, nlohmann::json &paraseJson, const std::string &bundleName,
        const BundlePropertyType &type, const T &value);
    template <typename T>
    void CreateBundleProperty(PreferencesInfo &preferencesInfo, nlohmann::json &paraseJson,
        const std::string &bundleName, const BundlePropertyType &type, const T &value);
    template <typename T>
    void UpdateBundleProperty(BundleInfo &bundleInfo, nlohmann::json &paraseJson, const std::string &bundleName,
        const BundlePropertyType &type, const T &value);
    template <typename T>
    ErrCode GetBundleProperty(const std::string &bundleName, const BundlePropertyType &type, T &value);

    void AddSlotToJson(nlohmann::json &json, const std::string &bundleName, const sptr<NotificationSlot> &slot) const;
    void AddSlotGroupToJson(
        nlohmann::json &json, const std::string &bundleName, const sptr<NotificationSlotGroup> &group) const;
    void UpdateExsitSlotTypeInJson(
        nlohmann::json &json, const std::string &bundleName, const sptr<NotificationSlot> &slot) const;
    void RemoveSlotInJson(
        nlohmann::json &json, const std::string &bundleName, const NotificationConstant::SlotType &type) const;
    void UpdateSlotInJson(
        nlohmann::json &json, const std::string &bundleName, const sptr<NotificationSlot> &slot) const;
    void RemoveSlotGroupInJson(nlohmann::json &json, const std::string &bundleName, const std::string &groupId) const;
    void UpdateSlotGroupInJson(
        nlohmann::json &json, const std::string &bundleName, const sptr<NotificationSlotGroup> &slot) const;
    void RemoveBundleInJson(nlohmann::json &json, const std::string &bundleName);
    void AddJson(nlohmann::json &json, const BundleInfo &info, const NotificationConstant::DisturbMode &mode,
        const bool &enable) const;
    void ParseFromJson();
    ErrCode ParseNotification();
    ErrCode StoreNotification(const nlohmann::json &json) const;
    ErrCode CommitNotification(const nlohmann::json &paraseJson, const PreferencesInfo &preferencesInfo);

    template <typename T>
    void CheckInvalidPath(T &t) const;
    void InitDir() const;
    void InitFile() const;
    bool IsEmptyInputFile(std::ifstream &inputFile) const;
    bool IsExistDir(const std::string &path) const;
    bool IsExistFile(const std::string &dirPath, const std::string &filePath) const;
    bool CreateDirectory(const std::string &path) const;
    bool CreateAndChangeFileAttr(const std::string &filePath) const;

    nlohmann::json BundlePropertyToJson(const BundleInfo &bundleInfo) const;
    nlohmann::json GroupToJson(const sptr<NotificationSlotGroup> &group) const;
    nlohmann::json SlotToJson(const sptr<NotificationSlot> &slot) const;
    void JsonToBundleProperty(nlohmann::json &json, BundleInfo &bundleInfo) const;
    void JsonToGroup(nlohmann::json &json, sptr<NotificationSlotGroup> &group) const;
    void JsonToSlot(nlohmann::json &json, sptr<NotificationSlot> &slot) const;

private:
    PreferencesInfo preferencesInfo_ {};
    nlohmann::json paraseJson_ {};

    DECLARE_DELAYED_REF_SINGLETON(NotificationPreferences);
};

}  // namespace Notification
}  // namespace OHOS

#endif  // NOTIFICATION_PREFERENCES_H
