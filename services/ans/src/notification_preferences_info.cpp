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
#include "notification_preferences_info.h"

namespace OHOS {
namespace Notification {

NotificationPreferencesInfo::BundleInfo::BundleInfo()
{}
NotificationPreferencesInfo::BundleInfo::~BundleInfo()
{}

void NotificationPreferencesInfo::BundleInfo::SetBundleName(const std::string &name)
{
    bundleName_ = name;
}

std::string NotificationPreferencesInfo::BundleInfo::GetBundleName() const
{
    return bundleName_;
}

void NotificationPreferencesInfo::BundleInfo::SetImportance(const int &level)
{
    importance_ = level;
}

int NotificationPreferencesInfo::BundleInfo::GetImportance() const
{
    return importance_;
}

void NotificationPreferencesInfo::BundleInfo::SetIsShowBadge(const bool &isShowBadge)
{
    isShowBadge_ = isShowBadge;
}

bool NotificationPreferencesInfo::BundleInfo::GetIsShowBadge() const
{
    return isShowBadge_;
}

void NotificationPreferencesInfo::BundleInfo::SetBadgeTotalNum(const int &num)
{
    badgeTotalNum_ = num;
}

int NotificationPreferencesInfo::BundleInfo::GetBadgeTotalNum() const
{
    return badgeTotalNum_;
}

void NotificationPreferencesInfo::BundleInfo::SetIsPrivateAllowed(const bool &isPrivateAllowed)
{
    isPrivateAllowed_ = isPrivateAllowed;
}

bool NotificationPreferencesInfo::BundleInfo::GetIsPrivateAllowed() const
{
    return isPrivateAllowed_;
}

void NotificationPreferencesInfo::BundleInfo::SetEnableNotification(const bool &enable)
{
    isEnabledNotification_ = enable;
}

bool NotificationPreferencesInfo::BundleInfo::GetEnableNotification() const
{
    return isEnabledNotification_;
}

void NotificationPreferencesInfo::BundleInfo::SetSlot(const sptr<NotificationSlot> &slot)
{
    slots_.insert_or_assign(slot->GetType(), slot);
}

bool NotificationPreferencesInfo::BundleInfo::GetSlot(
    const NotificationConstant::SlotType &type, sptr<NotificationSlot> &slot) const
{
    auto iter = slots_.find(type);
    if (iter != slots_.end()) {
        slot = iter->second;
        return true;
    }
    return false;
}

bool NotificationPreferencesInfo::BundleInfo::GetAllSlots(std::vector<sptr<NotificationSlot>> &slots)
{
    std::for_each(slots_.begin(),
        slots_.end(),
        [&slots](std::map<NotificationConstant::SlotType, sptr<NotificationSlot>>::reference iter) {
            slots.emplace_back(iter.second);
        });
    return true;
}

bool NotificationPreferencesInfo::BundleInfo::GetAllSlotsInGroup(
    const std::string groupId, std::vector<sptr<NotificationSlot>> &slots)
{
    std::for_each(slots_.begin(),
        slots_.end(),
        [&slots, &groupId](std::map<NotificationConstant::SlotType, sptr<NotificationSlot>>::reference &iter) {
            if (!iter.second->GetSlotGroup().compare(groupId)) {
                slots.emplace_back(iter.second);
            }
        });
    return true;
}

void NotificationPreferencesInfo::BundleInfo::SetGroup(const sptr<NotificationSlotGroup> &group)
{
    groups_.insert_or_assign(group->GetId(), group);
}

bool NotificationPreferencesInfo::BundleInfo::GetGroup(
    const std::string &groupId, sptr<NotificationSlotGroup> &group) const
{
    auto iter = groups_.find(groupId);
    if (iter != groups_.end()) {
        group = iter->second;
        return true;
    }
    return false;
}

bool NotificationPreferencesInfo::BundleInfo::GetAllGroups(std::vector<sptr<NotificationSlotGroup>> &group)
{
    std::for_each(
        groups_.begin(), groups_.end(), [&group](std::map<std::string, sptr<NotificationSlotGroup>>::reference &iter) {
            group.emplace_back(iter.second);
        });
    return true;
}

uint32_t NotificationPreferencesInfo::BundleInfo::GetGroupSize() const
{
    return groups_.size();
}

bool NotificationPreferencesInfo::BundleInfo::IsExsitSlot(const NotificationConstant::SlotType &type) const
{
    auto iter = slots_.find(type);
    if (iter != slots_.end()) {
        return true;
    }
    return false;
}

bool NotificationPreferencesInfo::BundleInfo::IsExsitSlotGroup(const std::string &groupId) const
{
    auto iter = groups_.find(groupId);
    if (iter != groups_.end()) {
        return true;
    }
    return false;
}

bool NotificationPreferencesInfo::BundleInfo::RemoveSlot(const NotificationConstant::SlotType &type)
{
    auto iter = slots_.find(type);
    if (iter != slots_.end()) {
        slots_.erase(iter);
        return true;
    }
    return false;
}

bool NotificationPreferencesInfo::BundleInfo::RemoveSlotGroup(const std::string &groupId)
{
    auto iter = groups_.find(groupId);
    if (iter != groups_.end()) {
        groups_.erase(groupId);
        return true;
    }
    return false;
}

void NotificationPreferencesInfo::SetEnabledAllNotification(const bool &value)
{
    isEnabledAllNotification_ = value;
}

bool NotificationPreferencesInfo::GetEnabledAllNotification() const
{
    return isEnabledAllNotification_;
}

void NotificationPreferencesInfo::SetDisturbMode(const NotificationConstant::DisturbMode &mode)
{
    disturbMode_ = mode;
}

NotificationConstant::DisturbMode NotificationPreferencesInfo::GetDisturbMode() const
{
    return disturbMode_;
}

void NotificationPreferencesInfo::SetBundleInfo(const BundleInfo &info)
{
    infos_.insert_or_assign(info.GetBundleName(), info);
}

bool NotificationPreferencesInfo::GetBundleInfo(const std::string &bundleName, BundleInfo &info) const
{
    auto iter = infos_.find(bundleName);
    if (iter != infos_.end()) {
        info = iter->second;
        return true;
    }
    return false;
}

bool NotificationPreferencesInfo::RemoveBundleInfo(const std::string &bundleName)
{
    auto iter = infos_.find(bundleName);
    if (iter != infos_.end()) {
        infos_.erase(iter);
        return true;
    }
    return false;
}

bool NotificationPreferencesInfo::IsExsitBundleInfo(const std::string &bundleName) const
{
    auto iter = infos_.find(bundleName);
    if (iter != infos_.end()) {
        return true;
    }
    return false;
}

void NotificationPreferencesInfo::ClearBundleInfo()
{
    infos_.clear();
}

}  // namespace Notification
}  // namespace OHOS