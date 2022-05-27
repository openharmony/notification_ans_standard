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

#include "ans_manager_proxy.h"
#include "ans_const_define.h"
#include "ans_inner_errors.h"
#include "ans_log_wrapper.h"
#include "message_option.h"
#include "message_parcel.h"
#include "parcel.h"
#include "reminder_request_alarm.h"
#include "reminder_request_calendar.h"
#include "reminder_request_timer.h"

namespace OHOS {
namespace Notification {
AnsManagerProxy::AnsManagerProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<AnsManagerInterface>(impl)
{}

AnsManagerProxy::~AnsManagerProxy()
{}

ErrCode AnsManagerProxy::Publish(const std::string &label, const sptr<NotificationRequest> &notification)
{
    if (notification == nullptr) {
        ANS_LOGE("[Publish] fail: notification is null ptr.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[Publish] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteString(label)) {
        ANS_LOGE("[Publish] fail: write label failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteParcelable(notification)) {
        ANS_LOGE("[Publish] fail: write notification parcelable failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(PUBLISH_NOTIFICATION, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[Publish] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[Publish] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::PublishToDevice(const sptr<NotificationRequest> &notification, const std::string &deviceId)
{
    if (notification == nullptr) {
        ANS_LOGE("[PublishToDevice] fail: notification is null ptr.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[PublishToDevice] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteParcelable(notification)) {
        ANS_LOGE("[PublishToDevice] fail: write notification parcelable failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteString(deviceId)) {
        ANS_LOGE("[PublishToDevice] fail: write deviceId failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(PUBLISH_NOTIFICATION_TO_DEVICE, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[PublishToDevice] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[PublishToDevice] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::Cancel(int32_t notificationId, const std::string &label)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[Cancel] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteInt32(notificationId)) {
        ANS_LOGE("[Cancel] fail: write notificationId failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteString(label)) {
        ANS_LOGE("[Cancel] fail: write label failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(CANCEL_NOTIFICATION, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[Cancel] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[Cancel] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::CancelAll()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[CancelAll] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(CANCEL_ALL_NOTIFICATIONS, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[CancelAll] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[CancelAll] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::CancelAsBundle(
    int32_t notificationId, const std::string &representativeBundle, int32_t userId)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[CancelAsBundle] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteInt32(notificationId)) {
        ANS_LOGE("[CancelAsBundle] fail: write notificationId failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteString(representativeBundle)) {
        ANS_LOGE("[CancelAsBundle] fail: write representativeBundle failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteInt32(userId)) {
        ANS_LOGE("[CancelAsBundle] fail: write userId failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(CANCEL_AS_BUNDLE, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[CancelAsBundle] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[CancelAsBundle] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::AddSlotByType(NotificationConstant::SlotType slotType)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[AddSlotByType] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteInt32(slotType)) {
        ANS_LOGE("[AddSlotByType] fail:: write slotIds failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(ADD_SLOT_BY_TYPE, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[AddSlotByType] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[AddSlotByType] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::AddSlots(const std::vector<sptr<NotificationSlot>> &slots)
{
    if (slots.empty()) {
        ANS_LOGE("[AddSlots] fail: slots is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    size_t slotsSize = slots.size();
    if (slotsSize > MAX_SLOT_NUM) {
        ANS_LOGE("[AddSlots] fail: slotsSize over max size.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[AddSlots] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!WriteParcelableVector(slots, data)) {
        ANS_LOGE("[AddSlots] fail: write slots failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(ADD_SLOTS, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[AddSlots] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[AddSlots] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::RemoveSlotByType(const NotificationConstant::SlotType &slotType)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[RemoveSlotByType] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteInt32(slotType)) {
        ANS_LOGE("[RemoveSlotByType] fail:: write slotIds failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(REMOVE_SLOT_BY_TYPE, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[RemoveSlotByType] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[RemoveSlotByType] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::RemoveAllSlots()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[RemoveAllSlots] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(REMOVE_ALL_SLOTS, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[RemoveAllSlots] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[RemoveAllSlots] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::AddSlotGroups(std::vector<sptr<NotificationSlotGroup>> groups)
{
    if (groups.empty()) {
        ANS_LOGE("[AddSlotGroups] fail: groups is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    size_t groupsSize = groups.size();
    if (groupsSize > MAX_SLOT_GROUP_NUM) {
        ANS_LOGE("[AddSlotGroups] fail: groupsSize over max size.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[AddSlotGroups] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!WriteParcelableVector(groups, data)) {
        ANS_LOGE("[AddSlotGroups] fail: write groups failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(ADD_SLOT_GROUPS, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[AddSlotGroups] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[AddSlotGroups] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::GetSlotByType(const NotificationConstant::SlotType &slotType, sptr<NotificationSlot> &slot)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[GetSlotByType] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteInt32(slotType)) {
        ANS_LOGE("[GetSlotByType] fail:: write slotId failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(GET_SLOT_BY_TYPE, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[GetSlotByType] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[GetSlotByType] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (result == ERR_OK) {
        slot = reply.ReadParcelable<NotificationSlot>();
        if (slot == nullptr) {
            ANS_LOGE("[GetSlotByType] fail: read slot failed");
            return ERR_ANS_PARCELABLE_FAILED;
        }
    }

    return result;
}

ErrCode AnsManagerProxy::GetSlots(std::vector<sptr<NotificationSlot>> &slots)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[GetSlots] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(GET_SLOTS, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[GetSlots] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!ReadParcelableVector(slots, reply, result)) {
        ANS_LOGE("[GetSlots] fail: read slots failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::GetSlotGroup(const std::string &groupId, sptr<NotificationSlotGroup> &group)
{
    if (groupId.empty()) {
        ANS_LOGE("[GetSlotGroup] fail: groupId is null.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[GetSlotGroup] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteString(groupId)) {
        ANS_LOGE("[GetSlotGroup] fail:: write groupId failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(GET_SLOT_GROUP, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[GetSlotGroup] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[GetSlotGroup] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (result == ERR_OK) {
        group = reply.ReadParcelable<NotificationSlotGroup>();
        if (group == nullptr) {
            ANS_LOGE("[GetSlotGroup] fail: read group failed");
            return ERR_ANS_PARCELABLE_FAILED;
        }
    }

    return result;
}

ErrCode AnsManagerProxy::GetSlotGroups(std::vector<sptr<NotificationSlotGroup>> &groups)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[GetSlotGroups] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(GET_SLOT_GROUPS, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[GetSlotGroups] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!ReadParcelableVector(groups, reply, result)) {
        ANS_LOGE("[GetSlotGroups] fail: read groups failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::GetSlotNumAsBundle(const sptr<NotificationBundleOption> &bundleOption, uint64_t &num)
{
    if (bundleOption == nullptr) {
        ANS_LOGE("[GetSlotNumAsBundle] fail: bundle is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[GetSlotNumAsBundle] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteStrongParcelable(bundleOption)) {
        ANS_LOGE("[GetSlotNumAsBundle] fail:: write bundle failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(GET_SLOT_NUM_AS_BUNDLE, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[GetShowBadgeEnabledForBundle] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[GetShowBadgeEnabledForBundle] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!reply.ReadUint64(num)) {
        ANS_LOGE("[GetShowBadgeEnabledForBundle] fail: read enabled failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::RemoveSlotGroups(const std::vector<std::string> &groupIds)
{
    if (groupIds.empty()) {
        ANS_LOGE("[RemoveSlotGroups] fail: groupIds is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[RemoveSlotGroups] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteStringVector(groupIds)) {
        ANS_LOGE("[RemoveSlotGroups] fail:: write groupIds failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(REMOVE_SLOT_GROUPS, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[RemoveSlotGroups] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[RemoveSlotGroups] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::GetActiveNotifications(std::vector<sptr<NotificationRequest>> &notifications)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[GetActiveNotifications] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(GET_ACTIVE_NOTIFICATIONS, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[GetActiveNotifications] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!ReadParcelableVector(notifications, reply, result)) {
        ANS_LOGE("[GetActiveNotifications] fail: read notifications failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::GetActiveNotificationNums(uint64_t &num)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[GetActiveNotificationNums] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(GET_ACTIVE_NOTIFICATION_NUMS, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[GetActiveNotificationNums] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[GetActiveNotificationNums] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!reply.ReadUint64(num)) {
        ANS_LOGE("[GetActiveNotificationNums] fail: read notification num failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::GetAllActiveNotifications(std::vector<sptr<Notification>> &notifications)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[GetAllActiveNotifications] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(GET_ALL_ACTIVE_NOTIFICATIONS, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[GetAllActiveNotifications] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!ReadParcelableVector(notifications, reply, result)) {
        ANS_LOGE("[GetAllActiveNotifications] fail: read notifications failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::GetSpecialActiveNotifications(
    const std::vector<std::string> &key, std::vector<sptr<Notification>> &notifications)
{
    if (key.empty()) {
        ANS_LOGE("[GetSpecialActiveNotifications] fail: key is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[GetSpecialActiveNotifications] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteStringVector(key)) {
        ANS_LOGE("[GetSpecialActiveNotifications] fail:: write key failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(GET_SPECIAL_ACTIVE_NOTIFICATIONS, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[GetSpecialActiveNotifications] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!ReadParcelableVector(notifications, reply, result)) {
        ANS_LOGE("[GetSpecialActiveNotifications] fail: read notifications failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::SetNotificationAgent(const std::string &agent)
{
    if (agent.empty()) {
        ANS_LOGE("[SetNotificationAgent] fail: agent is null.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[SetNotificationAgent] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteString(agent)) {
        ANS_LOGE("[SetNotificationAgent] fail:: write agent failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(SET_NOTIFICATION_AGENT, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[SetNotificationAgent] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[SetNotificationAgent] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::GetNotificationAgent(std::string &agent)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[GetNotificationAgent] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(GET_NOTIFICATION_AGENT, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[GetNotificationAgent] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[GetNotificationAgent] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!reply.ReadString(agent)) {
        ANS_LOGE("[GetNotificationAgent] fail: read agent failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::CanPublishAsBundle(const std::string &representativeBundle, bool &canPublish)
{
    if (representativeBundle.empty()) {
        ANS_LOGE("[CanPublishAsBundle] fail: representativeBundle is null.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[CanPublishAsBundle] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteString(representativeBundle)) {
        ANS_LOGE("[CanPublishAsBundle] fail: write representativeBundle failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(CAN_PUBLISH_AS_BUNDLE, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[CanPublishAsBundle] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[CanPublishAsBundle] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!reply.ReadBool(canPublish)) {
        ANS_LOGE("[CanPublishAsBundle] fail: read canPublish failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::PublishAsBundle(
    const sptr<NotificationRequest> notification, const std::string &representativeBundle)
{
    if (notification == nullptr) {
        ANS_LOGE("[PublishAsBundle] fail: notification is null ptr.");
        return ERR_ANS_INVALID_PARAM;
    }

    if (representativeBundle.empty()) {
        ANS_LOGE("[PublishAsBundle] fail: representativeBundle is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[PublishAsBundle] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteParcelable(notification)) {
        ANS_LOGE("[PublishAsBundle] fail: write notification failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteString(representativeBundle)) {
        ANS_LOGE("[PublishAsBundle] fail: write representativeBundle failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(PUBLISH_AS_BUNDLE, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[PublishAsBundle] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[PublishAsBundle] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::SetNotificationBadgeNum(int32_t num)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[SetNotificationBadgeNum] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteInt32(num)) {
        ANS_LOGE("[SetNotificationBadgeNum] fail: write num failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(SET_NOTIFICATION_BADGE_NUM, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[SetNotificationBadgeNum] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[SetNotificationBadgeNum] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::GetBundleImportance(int32_t &importance)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[GetBundleImportance] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(GET_BUNDLE_IMPORTANCE, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[GetBundleImportance] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[GetBundleImportance] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!reply.ReadInt32(importance)) {
        ANS_LOGE("[GetBundleImportance] fail: read importance failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::HasNotificationPolicyAccessPermission(bool &granted)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[HasNotificationPolicyAccessPermission] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(IS_NOTIFICATION_POLICY_ACCESS_GRANTED, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[HasNotificationPolicyAccessPermission] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[HasNotificationPolicyAccessPermission] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!reply.ReadBool(granted)) {
        ANS_LOGE("[HasNotificationPolicyAccessPermission] fail: read granted failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::SetPrivateNotificationsAllowed(bool allow)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[SetPrivateNotificationsAllowed] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteBool(allow)) {
        ANS_LOGE("[SetPrivateNotificationsAllowed] fail: write allow failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(SET_PRIVATIVE_NOTIFICATIONS_ALLOWED, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[SetPrivateNotificationsAllowed] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[SetPrivateNotificationsAllowed] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::GetPrivateNotificationsAllowed(bool &allow)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[GetPrivateNotificationsAllowed] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(GET_PRIVATIVE_NOTIFICATIONS_ALLOWED, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[GetPrivateNotificationsAllowed] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[GetPrivateNotificationsAllowed] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!reply.ReadBool(allow)) {
        ANS_LOGE("[GetPrivateNotificationsAllowed] fail: read allow failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::RemoveNotification(
    const sptr<NotificationBundleOption> &bundleOption, int32_t notificationId, const std::string &label)
{
    if (bundleOption == nullptr) {
        ANS_LOGE("[RemoveNotification] fail: bundle is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[RemoveNotification] fail:, write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteStrongParcelable(bundleOption)) {
        ANS_LOGE("[RemoveNotification] fail:: write bundle failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteInt32(notificationId)) {
        ANS_LOGE("[RemoveNotification] fail: write notificationId failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteString(label)) {
        ANS_LOGE("[RemoveNotification] fail: write label failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(REMOVE_NOTIFICATION, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[RemoveNotification] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[RemoveNotification] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::RemoveAllNotifications(const sptr<NotificationBundleOption> &bundleOption)
{
    if (bundleOption == nullptr) {
        ANS_LOGE("[RemoveAllNotifications] fail: bundle is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[RemoveAllNotifications] fail:, write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteStrongParcelable(bundleOption)) {
        ANS_LOGE("[RemoveAllNotifications] fail:: write bundle failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(REMOVE_ALL_NOTIFICATIONS, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[RemoveNotification] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[RemoveNotification] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::Delete(const std::string &key)
{
    if (key.empty()) {
        ANS_LOGE("[Delete] fail: key is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[Delete] fail:, write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteString(key)) {
        ANS_LOGE("[Delete] fail:: write key failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(DELETE_NOTIFICATION, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[Delete] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[Delete] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::DeleteByBundle(const sptr<NotificationBundleOption> &bundleOption)
{
    if (bundleOption == nullptr) {
        ANS_LOGE("[DeleteByBundle] fail: bundle is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[DeleteByBundle] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteStrongParcelable(bundleOption)) {
        ANS_LOGE("[DeleteByBundle] fail: write bundle failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(DELETE_NOTIFICATION_BY_BUNDLE, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[DeleteByBundle] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[DeleteByBundle] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::DeleteAll()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[DeleteAll] fail:, write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(DELETE_ALL_NOTIFICATIONS, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[DeleteAll] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[DeleteAll] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::GetSlotsByBundle(
    const sptr<NotificationBundleOption> &bundleOption, std::vector<sptr<NotificationSlot>> &slots)
{
    if (bundleOption == nullptr) {
        ANS_LOGE("[GetSlotsByBundle] fail: bundleOption is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[GetSlotsByBundle] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteParcelable(bundleOption)) {
        ANS_LOGE("[GetSlotsByBundle] fail:: write bundle failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(GET_SLOTS_BY_BUNDLE, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[GetSlotsByBundle] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!ReadParcelableVector(slots, reply, result)) {
        ANS_LOGE("[GetSlotsByBundle] fail: read slots failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::UpdateSlots(
    const sptr<NotificationBundleOption> &bundleOption, const std::vector<sptr<NotificationSlot>> &slots)
{
    if (bundleOption == nullptr) {
        ANS_LOGE("[UpdateSlots] fail: bundleOption is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    if (slots.empty()) {
        ANS_LOGE("[UpdateSlots] fail: slots is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    size_t slotSize = slots.size();
    if (slotSize > MAX_SLOT_NUM) {
        ANS_LOGE("[UpdateSlots] fail: slotSize over max size.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[UpdateSlots] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteParcelable(bundleOption)) {
        ANS_LOGE("[UpdateSlots] fail:: write bundleoption failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!WriteParcelableVector(slots, data)) {
        ANS_LOGE("[UpdateSlots] fail: write slots failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(UPDATE_SLOTS, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[UpdateSlots] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[UpdateSlots] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::UpdateSlotGroups(
    const sptr<NotificationBundleOption> &bundleOption, const std::vector<sptr<NotificationSlotGroup>> &groups)
{
    if (bundleOption == nullptr) {
        ANS_LOGE("[UpdateSlotGroups] fail: bundleOption is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    if (groups.empty()) {
        ANS_LOGE("[UpdateSlotGroups] fail: groups is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    size_t groupSize = groups.size();
    if (groupSize > MAX_SLOT_GROUP_NUM) {
        ANS_LOGE("[UpdateSlotGroups] fail: groupSize over max size.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[UpdateSlotGroups] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteParcelable(bundleOption)) {
        ANS_LOGE("[UpdateSlotGroups] fail:: write bundleOption failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!WriteParcelableVector(groups, data)) {
        ANS_LOGE("[UpdateSlotGroups] fail: write groups failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(UPDATE_SLOT_GROUPS, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[UpdateSlotGroups] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[UpdateSlotGroups] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::RequestEnableNotification(const std::string &deviceId)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[RequestEnableNotification] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteString(deviceId)) {
        ANS_LOGE("[RequestEnableNotification] fail: write deviceId failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(REQUEST_ENABLE_NOTIFICATION, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[RequestEnableNotification] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[RequestEnableNotification] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::SetNotificationsEnabledForBundle(const std::string &deviceId, bool enabled)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[SetNotificationsEnabledForBundle] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteString(deviceId)) {
        ANS_LOGE("[SetNotificationsEnabledForBundle] fail: write deviceId failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteBool(enabled)) {
        ANS_LOGE("[SetNotificationsEnabledForBundle] fail: write enabled failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(SET_NOTIFICATION_ENABLED_FOR_BUNDLE, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[SetNotificationsEnabledForBundle] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[SetNotificationsEnabledForBundle] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::SetNotificationsEnabledForAllBundles(const std::string &deviceId, bool enabled)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[SetNotificationsEnabledForAllBundles] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteString(deviceId)) {
        ANS_LOGE("[SetNotificationsEnabledForAllBundles] fail: write deviceId failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteBool(enabled)) {
        ANS_LOGE("[SetNotificationsEnabledForAllBundles] fail: write enabled failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(SET_NOTIFICATION_ENABLED_FOR_ALL_BUNDLE, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[SetNotificationsEnabledForAllBundles] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[SetNotificationsEnabledForAllBundles] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::SetNotificationsEnabledForSpecialBundle(
    const std::string &deviceId, const sptr<NotificationBundleOption> &bundleOption, bool enabled)
{
    if (bundleOption == nullptr) {
        ANS_LOGE("[SetNotificationsEnabledForSpecialBundle] fail: bundleOption is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[SetNotificationsEnabledForSpecialBundle] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteString(deviceId)) {
        ANS_LOGE("[SetNotificationsEnabledForSpecialBundle] fail: write deviceId failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteParcelable(bundleOption)) {
        ANS_LOGE("[SetNotificationsEnabledForSpecialBundle] fail: write bundleOption failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteBool(enabled)) {
        ANS_LOGE("[SetNotificationsEnabledForSpecialBundle] fail: write enabled failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(SET_NOTIFICATION_ENABLED_FOR_SPECIAL_BUNDLE, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[SetNotificationsEnabledForSpecialBundle] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[SetNotificationsEnabledForSpecialBundle] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::SetShowBadgeEnabledForBundle(const sptr<NotificationBundleOption> &bundleOption, bool enabled)
{
    if (bundleOption == nullptr) {
        ANS_LOGE("[SetShowBadgeEnabledForBundle] fail: bundle is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[SetShowBadgeEnabledForBundle] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteParcelable(bundleOption)) {
        ANS_LOGE("[SetShowBadgeEnabledForBundle] fail:: write bundle failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteBool(enabled)) {
        ANS_LOGE("[SetShowBadgeEnabledForBundle] fail:: write enabled failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(SET_SHOW_BADGE_ENABLED_FOR_BUNDLE, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[SetShowBadgeEnabledForBundle] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[SetShowBadgeEnabledForBundle] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::GetShowBadgeEnabledForBundle(const sptr<NotificationBundleOption> &bundleOption, bool &enabled)
{
    if (bundleOption == nullptr) {
        ANS_LOGE("[GetShowBadgeEnabledForBundle] fail: bundle is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[GetShowBadgeEnabledForBundle] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteParcelable(bundleOption)) {
        ANS_LOGE("[GetShowBadgeEnabledForBundle] fail:: write bundle failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(GET_SHOW_BADGE_ENABLED_FOR_BUNDLE, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[GetShowBadgeEnabledForBundle] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[GetShowBadgeEnabledForBundle] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!reply.ReadBool(enabled)) {
        ANS_LOGE("[GetShowBadgeEnabledForBundle] fail: read enabled failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::GetShowBadgeEnabled(bool &enabled)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[GetShowBadgeEnabled] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(GET_SHOW_BADGE_ENABLED, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[GetShowBadgeEnabled] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[GetShowBadgeEnabled] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!reply.ReadBool(enabled)) {
        ANS_LOGE("[GetShowBadgeEnabled] fail: read enabled failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::Subscribe(const sptr<AnsSubscriberInterface> &subscriber,
    const sptr<NotificationSubscribeInfo> &info)
{
    if (subscriber == nullptr) {
        ANS_LOGE("[Subscribe] fail: subscriber is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[Subscribe] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    bool ret = data.WriteRemoteObject(subscriber->AsObject());
    if (!ret) {
        ANS_LOGE("[Subscribe] fail: write subscriber failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteBool(info != nullptr)) {
        ANS_LOGE("[Subscribe] fail: write isSubcribeInfo failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (info != nullptr) {
        if (!data.WriteParcelable(info)) {
            ANS_LOGE("[Subscribe] fail: write subcribeInfo failed");
            return ERR_ANS_PARCELABLE_FAILED;
        }
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(SUBSCRIBE_NOTIFICATION, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[Subscribe] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[Subscribe] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::Unsubscribe(
    const sptr<AnsSubscriberInterface> &subscriber, const sptr<NotificationSubscribeInfo> &info)
{
    if (subscriber == nullptr) {
        ANS_LOGE("[Unsubscribe] fail: subscriber is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[Unsubscribe] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    bool ret = data.WriteRemoteObject(subscriber->AsObject());
    if (!ret) {
        ANS_LOGE("[Unsubscribe] fail: write subscriber failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteBool(info != nullptr)) {
        ANS_LOGE("[Unsubscribe] fail: write isSubcribeInfo failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (info != nullptr) {
        if (!data.WriteParcelable(info)) {
            ANS_LOGE("[Unsubscribe] fail: write subcribeInfo failed");
            return ERR_ANS_PARCELABLE_FAILED;
        }
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(UNSUBSCRIBE_NOTIFICATION, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[Unsubscribe] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[Unsubscribe] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::AreNotificationsSuspended(bool &suspended)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[AreNotificationsSuspended] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(ARE_NOTIFICATION_SUSPENDED, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[AreNotificationsSuspended] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[AreNotificationsSuspended] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!reply.ReadBool(suspended)) {
        ANS_LOGE("[AreNotificationsSuspended] fail: read suspended failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::GetCurrentAppSorting(sptr<NotificationSortingMap> &sortingMap)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[GetCurrentAppSorting] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(GET_CURRENT_APP_SORTING, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[GetCurrentAppSorting] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[GetCurrentAppSorting] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    sortingMap = reply.ReadParcelable<NotificationSortingMap>();
    if (sortingMap == nullptr) {
        ANS_LOGE("[GetCurrentAppSorting] fail: read sortingMap failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::IsAllowedNotify(bool &allowed)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[IsAllowedNotify] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(IS_ALLOWED_NOTIFY, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[IsAllowedNotify] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[IsAllowedNotify] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!reply.ReadBool(allowed)) {
        ANS_LOGE("[IsAllowedNotify] fail: read allowed failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::IsAllowedNotifySelf(bool &allowed)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[IsAllowedNotifySelf] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(IS_ALLOWED_NOTIFY_SELF, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[IsAllowedNotifySelf] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[IsAllowedNotifySelf] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!reply.ReadBool(allowed)) {
        ANS_LOGE("[IsAllowedNotifySelf] fail: read allowed failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::IsSpecialBundleAllowedNotify(const sptr<NotificationBundleOption> &bundleOption, bool &allowed)
{
    if (bundleOption == nullptr) {
        ANS_LOGE("[IsSpecialBundleAllowedNotify] fail: bundle is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[IsSpecialBundleAllowedNotify] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteParcelable(bundleOption)) {
        ANS_LOGE("[IsSpecialBundleAllowedNotify] fail: write bundle failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(IS_SPECIAL_BUNDLE_ALLOWED_NOTIFY, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[IsSpecialBundleAllowedNotify] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[IsSpecialBundleAllowedNotify] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!reply.ReadBool(allowed)) {
        ANS_LOGE("[IsSpecialBundleAllowedNotify] fail: read allowed failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::CancelGroup(const std::string &groupName)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[CancelGroup] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteString(groupName)) {
        ANS_LOGE("[CancelGroup] fail: write groupName failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(CANCEL_GROUP, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[CancelGroup] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[CancelGroup] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::RemoveGroupByBundle(
    const sptr<NotificationBundleOption> &bundleOption, const std::string &groupName)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[RemoveGroupByBundle] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteParcelable(bundleOption)) {
        ANS_LOGE("[RemoveGroupByBundle] fail:: write bundleOption failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteString(groupName)) {
        ANS_LOGE("[RemoveGroupByBundle] fail: write groupName failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(REMOVE_GROUP_BY_BUNDLE, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[RemoveGroupByBundle] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[RemoveGroupByBundle] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::SetDoNotDisturbDate(const sptr<NotificationDoNotDisturbDate> &date)
{
    if (date == nullptr) {
        ANS_LOGE("[SetDoNotDisturbDate] fail: date is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[SetDoNotDisturbDate] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteParcelable(date)) {
        ANS_LOGE("[SetDoNotDisturbDate] fail: write date failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(SET_DO_NOT_DISTURB_DATE, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[SetDoNotDisturbDate] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[SetDoNotDisturbDate] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::GetDoNotDisturbDate(sptr<NotificationDoNotDisturbDate> &date)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[GetDoNotDisturbDate] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(GET_DO_NOT_DISTURB_DATE, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[GetDoNotDisturbDate] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[GetDoNotDisturbDate] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (result == ERR_OK) {
        date = reply.ReadParcelable<NotificationDoNotDisturbDate>();
        if (date == nullptr) {
            ANS_LOGE("[GetDoNotDisturbDate] fail: read date failed.");
            return ERR_ANS_PARCELABLE_FAILED;
        }
    }

    return result;
}

ErrCode AnsManagerProxy::DoesSupportDoNotDisturbMode(bool &doesSupport)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[DoesSupportDoNotDisturbMode] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(DOES_SUPPORT_DO_NOT_DISTURB_MODE, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[DoesSupportDoNotDisturbMode] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[DoesSupportDoNotDisturbMode] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!reply.ReadBool(doesSupport)) {
        ANS_LOGE("[DoesSupportDoNotDisturbMode] fail: read doesSupport failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::IsDistributedEnabled(bool &enabled)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[IsDistributedEnabled] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(IS_DISTRIBUTED_ENABLED, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[IsDistributedEnabled] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[IsDistributedEnabled] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!reply.ReadBool(enabled)) {
        ANS_LOGE("[IsDistributedEnabled] fail: read enabled failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::EnableDistributed(bool enabled)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[EnableDistributed] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteBool(enabled)) {
        ANS_LOGE("[EnableDistributed] fail: write enabled failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(ENABLE_DISTRIBUTED, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[EnableDistributed] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[EnableDistributed] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::EnableDistributedByBundle(const sptr<NotificationBundleOption> &bundleOption, bool enabled)
{
    if (bundleOption == nullptr) {
        ANS_LOGE("[EnableDistributedByBundle] fail: bundle is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[EnableDistributedByBundle] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteParcelable(bundleOption)) {
        ANS_LOGE("[EnableDistributedByBundle] fail:: write bundle failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteBool(enabled)) {
        ANS_LOGE("[EnableDistributedByBundle] fail:: write enabled failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(ENABLE_DISTRIBUTED_BY_BUNDLE, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[EnableDistributedByBundle] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[EnableDistributedByBundle] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::EnableDistributedSelf(bool enabled)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[EnableDistributedSelf] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteBool(enabled)) {
        ANS_LOGE("[EnableDistributedSelf] fail: write enabled failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(ENABLE_DISTRIBUTED_SELF, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[EnableDistributedSelf] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[EnableDistributedSelf] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::IsDistributedEnableByBundle(const sptr<NotificationBundleOption> &bundleOption, bool &enabled)
{
    if (bundleOption == nullptr) {
        ANS_LOGE("[IsDistributedEnableByBundle] fail: bundle is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[IsDistributedEnableByBundle] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteParcelable(bundleOption)) {
        ANS_LOGE("[IsDistributedEnableByBundle] fail: write bundle failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(IS_DISTRIBUTED_ENABLED_BY_BUNDLE, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[IsDistributedEnableByBundle] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[IsDistributedEnableByBundle] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!reply.ReadBool(enabled)) {
        ANS_LOGE("[IsDistributedEnableByBundle] fail: read enabled failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::GetDeviceRemindType(NotificationConstant::RemindType &remindType)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[GetDeviceRemindType] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(GET_DEVICE_REMIND_TYPE, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[GetDeviceRemindType] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[GetDeviceRemindType] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (result == ERR_OK) {
        int32_t rType {-1};
        if (!reply.ReadInt32(rType)) {
            ANS_LOGE("[GetDeviceRemindType] fail: read remind type failed.");
            return ERR_ANS_PARCELABLE_FAILED;
        }

        remindType = static_cast<NotificationConstant::RemindType>(rType);
    }

    return result;
}

ErrCode AnsManagerProxy::ShellDump(const std::string &dumpOption, std::vector<std::string> &dumpInfo)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[ShellDump] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteString(dumpOption)) {
        ANS_LOGE("[ShellDump] fail: write option failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(SHELL_DUMP, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[ShellDump] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[ShellDump] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!reply.ReadStringVector(&dumpInfo)) {
        ANS_LOGE("[ShellDump] fail: read dumpInfo failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::PublishContinuousTaskNotification(const sptr<NotificationRequest> &request)
{
    if (request == nullptr) {
        ANS_LOGE("[PublishContinuousTaskNotification] fail: notification request is null ptr.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[PublishContinuousTaskNotification] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteParcelable(request)) {
        ANS_LOGE("[PublishContinuousTaskNotification] fail: write request failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(PUBLISH_CONTINUOUS_TASK_NOTIFICATION, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[PublishContinuousTaskNotification] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[PublishContinuousTaskNotification] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::CancelContinuousTaskNotification(const std::string &label, int32_t notificationId)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[CancelContinuousTaskNotification] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteString(label)) {
        ANS_LOGE("[CancelContinuousTaskNotification] fail: write label failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteInt32(notificationId)) {
        ANS_LOGE("[CancelContinuousTaskNotification] fail: write notificationId failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(CANCEL_CONTINUOUS_TASK_NOTIFICATION, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[CancelContinuousTaskNotification] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[CancelContinuousTaskNotification] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::PublishReminder(sptr<ReminderRequest> &reminder)
{
    ANSR_LOGI("PublishReminder");
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANSR_LOGE("[PublishReminder] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }
    if (reminder == nullptr) {
        ANSR_LOGW("[PublishReminder] fail: reminder is null ptr.");
        return ERR_ANS_INVALID_PARAM;
    }
    if (!data.WriteUint8(static_cast<uint8_t>(reminder->GetReminderType()))) {
        ANSR_LOGE("[PublishReminder] fail: write reminder type failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }
    if (!data.WriteParcelable(reminder)) {
        ANSR_LOGE("[Publish] fail: write reminder parcelable failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(PUBLISH_REMINDER, option, data, reply);
    if (result != ERR_OK) {
        ANSR_LOGE("[PublishReminder] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }
    int32_t reminderId = -1;
    if (!reply.ReadInt32(reminderId)) {
        ANSR_LOGE("[PublishReminder] fail: read reminder id failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }
    reminder->SetReminderId(reminderId);
    ANSR_LOGD("ReminderId=%{public}d", reminder->GetReminderId());
    return result;
}

ErrCode AnsManagerProxy::CancelReminder(const int32_t reminderId)
{
    ANSR_LOGI("[CancelReminder]");
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANSR_LOGE("[CancelReminder] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }
    if (!data.WriteInt32(reminderId)) {
        ANSR_LOGE("[CancelReminder] fail: write reminder id failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(CANCEL_REMINDER, option, data, reply);
    if (result != ERR_OK) {
        ANSR_LOGE("[CancelReminder] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }
    return result;
}

ErrCode AnsManagerProxy::CancelAllReminders()
{
    ANSR_LOGI("[CancelAllReminders]");
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANSR_LOGE("[CancelAllReminders] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(CANCEL_ALL_REMINDERS, option, data, reply);
    if (result != ERR_OK) {
        ANSR_LOGE("[CancelAllReminders] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }
    return result;
}

ErrCode AnsManagerProxy::GetValidReminders(std::vector<sptr<ReminderRequest>> &reminders)
{
    ANSR_LOGI("[GetValidReminders]");
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANSR_LOGE("[GetValidReminders] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(GET_ALL_VALID_REMINDERS, option, data, reply);
    if (result != ERR_OK) {
        ANSR_LOGE("[GetValidReminders] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }
    uint8_t count = 0;
    if (!reply.ReadUint8(count)) {
        ANSR_LOGE("[GetValidReminders] fail: read reminder count failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }
    ANSR_LOGD("[GetValidReminders] count=%{public}hhu", count);
    reminders.clear();
    result = ReadReminders(count, reply, reminders);
    if (result != ERR_OK) {
        ANSR_LOGE("[GetValidReminders] fail: ReadReminders ErrCode=%{public}d", result);
    } else {
        ANSR_LOGD("[GetValidReminders], size=%{public}zu", reminders.size());
    }
    return result;
}

ErrCode AnsManagerProxy::ReadReminders(
    uint8_t &count, MessageParcel &reply, std::vector<sptr<ReminderRequest>> &reminders)
{
    for (uint8_t i = 0; i < count; i++) {
        uint8_t typeInfo = static_cast<uint8_t>(ReminderRequest::ReminderType::INVALID);
        if (!reply.ReadUint8(typeInfo)) {
            ANSR_LOGE("Failed to read reminder type");
            return ERR_ANS_PARCELABLE_FAILED;
        }
        auto reminderType = static_cast<ReminderRequest::ReminderType>(typeInfo);
        sptr<ReminderRequest> reminder;
        if (ReminderRequest::ReminderType::ALARM == reminderType) {
            ANSR_LOGD("[GetValidReminders] alarm");
            reminder = reply.ReadParcelable<ReminderRequestAlarm>();
        } else if (ReminderRequest::ReminderType::TIMER == reminderType) {
            ANSR_LOGD("[GetValidReminders] timer");
            reminder = reply.ReadParcelable<ReminderRequestTimer>();
        } else if (ReminderRequest::ReminderType::CALENDAR == reminderType) {
            ANSR_LOGD("[GetValidReminders] calendar");
            reminder = reply.ReadParcelable<ReminderRequestCalendar>();
        } else {
            ANSR_LOGW("[GetValidReminders] type=%{public}hhu", typeInfo);
            return ERR_ANS_INVALID_PARAM;
        }
        if (!reminder) {
            ANSR_LOGE("[GetValidReminders] fail: Reminder ReadParcelable failed");
            return ERR_ANS_PARCELABLE_FAILED;
        }
        reminders.push_back(reminder);
    }
    return ERR_OK;
}

ErrCode AnsManagerProxy::InnerTransact(uint32_t code, MessageOption &flags, MessageParcel &data, MessageParcel &reply)
{
    auto remote = Remote();
    if (remote == nullptr) {
        ANS_LOGE("[InnerTransact] fail: get Remote fail code %{public}u", code);
        return ERR_DEAD_OBJECT;
    }
    int32_t err = remote->SendRequest(code, data, reply, flags);
    switch (err) {
        case NO_ERROR: {
            return ERR_OK;
        }
        case DEAD_OBJECT: {
            ANS_LOGE("[InnerTransact] fail: ipcErr=%{public}d code %{public}d", err, code);
            return ERR_DEAD_OBJECT;
        }
        default: {
            ANS_LOGE("[InnerTransact] fail: ipcErr=%{public}d code %{public}d", err, code);
            return ERR_ANS_TRANSACT_FAILED;
        }
    }
}

template<typename T>
bool AnsManagerProxy::WriteParcelableVector(const std::vector<sptr<T>> &parcelableVector, MessageParcel &data)
{
    if (!data.WriteInt32(parcelableVector.size())) {
        ANS_LOGE("write ParcelableVector size failed");
        return false;
    }

    for (auto &parcelable : parcelableVector) {
        if (!data.WriteStrongParcelable(parcelable)) {
            ANS_LOGE("write ParcelableVector failed");
            return false;
        }
    }
    return true;
}

template<typename T>
bool AnsManagerProxy::ReadParcelableVector(std::vector<sptr<T>> &parcelableInfos, MessageParcel &reply, ErrCode &result)
{
    if (!reply.ReadInt32(result)) {
        ANS_LOGE("read result failed.");
        return false;
    }

    int32_t infoSize = 0;
    if (!reply.ReadInt32(infoSize)) {
        ANS_LOGE("read Parcelable size failed.");
        return false;
    }

    parcelableInfos.clear();
    for (int32_t index = 0; index < infoSize; index++) {
        sptr<T> info = reply.ReadStrongParcelable<T>();
        if (info == nullptr) {
            ANS_LOGE("read Parcelable infos failed.");
            return false;
        }
        parcelableInfos.emplace_back(info);
    }

    return true;
}

ErrCode AnsManagerProxy::IsSupportTemplate(const std::string &templateName, bool &support)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[IsSupportTemplate] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteString(templateName)) {
        ANS_LOGE("[IsSupportTemplate] fail: write template name failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(IS_SUPPORT_TEMPLATE, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[IsSupportTemplate] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[IsSupportTemplate] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!reply.ReadBool(support)) {
        ANS_LOGE("[IsSupportTemplate] fail: read support failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::IsSpecialUserAllowedNotify(const int32_t &userId, bool &allowed)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[IsSpecialUserAllowedNotify] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteInt32(userId)) {
        ANS_LOGE("[IsSpecialUserAllowedNotify] fail: write userId failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(IS_SPECIAL_USER_ALLOWED_NOTIFY, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[IsSpecialBundleAllowedNotify] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[IsSpecialBundleAllowedNotify] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!reply.ReadBool(allowed)) {
        ANS_LOGE("[IsSpecialBundleAllowedNotify] fail: read allowed failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::SetNotificationsEnabledByUser(const int32_t &userId, bool enabled)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[SetNotificationsEnabledByUser] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteInt32(userId)) {
        ANS_LOGE("[SetNotificationsEnabledByUser] fail: write userId failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteBool(enabled)) {
        ANS_LOGE("[SetNotificationsEnabledByUser] fail: write enabled failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(SET_NOTIFICATION_ENABLED_BY_USER, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[SetNotificationsEnabledByUser] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[SetNotificationsEnabledByUser] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::DeleteAllByUser(const int32_t &userId)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[DeleteAllByUser] fail:, write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteInt32(userId)) {
        ANS_LOGE("[DeleteAllByUser] fail: write userId failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(DELETE_ALL_NOTIFICATIONS_BY_USER, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[DeleteAllByUser] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[DeleteAllByUser] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::SetDoNotDisturbDate(const int32_t &userId, const sptr<NotificationDoNotDisturbDate> &date)
{
    if (date == nullptr) {
        ANS_LOGE("[SetDoNotDisturbDate] fail: date is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[SetDoNotDisturbDate] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteInt32(userId)) {
        ANS_LOGE("[SetDoNotDisturbDate] fail: write userId failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteParcelable(date)) {
        ANS_LOGE("[SetDoNotDisturbDate] fail: write date failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(SET_DO_NOT_DISTURB_DATE_BY_USER, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[SetDoNotDisturbDate] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[SetDoNotDisturbDate] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::GetDoNotDisturbDate(const int32_t &userId, sptr<NotificationDoNotDisturbDate> &date)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[GetDoNotDisturbDate] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteInt32(userId)) {
        ANS_LOGE("[GetDoNotDisturbDate] fail: write userId failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(GET_DO_NOT_DISTURB_DATE_BY_USER, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[GetDoNotDisturbDate] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[GetDoNotDisturbDate] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (result == ERR_OK) {
        date = reply.ReadParcelable<NotificationDoNotDisturbDate>();
        if (date == nullptr) {
            ANS_LOGE("[GetDoNotDisturbDate] fail: read date failed.");
            return ERR_ANS_PARCELABLE_FAILED;
        }
    }

    return result;
}

ErrCode AnsManagerProxy::SetEnabledForBundleSlot(
    const sptr<NotificationBundleOption> &bundleOption, const NotificationConstant::SlotType &slotType, bool enabled)
{
    if (bundleOption == nullptr) {
        ANS_LOGE("[SetEnabledForBundleSlot] fail: bundle is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[SetEnabledForBundleSlot] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteStrongParcelable(bundleOption)) {
        ANS_LOGE("[SetEnabledForBundleSlot] fail:: write bundle failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteInt32(slotType)) {
        ANS_LOGE("[SetEnabledForBundleSlot] fail:: write slotType failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteBool(enabled)) {
        ANS_LOGE("[SetEnabledForBundleSlot] fail: write enabled failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(SET_ENABLED_FOR_BUNDLE_SLOT, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[SetEnabledForBundleSlot] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[SetEnabledForBundleSlot] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}

ErrCode AnsManagerProxy::GetEnabledForBundleSlot(
    const sptr<NotificationBundleOption> &bundleOption, const NotificationConstant::SlotType &slotType, bool &enabled)
{
    if (bundleOption == nullptr) {
        ANS_LOGE("[GetEnabledForBundleSlot] fail: bundle is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsManagerProxy::GetDescriptor())) {
        ANS_LOGE("[GetEnabledForBundleSlot] fail: write interface token failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteStrongParcelable(bundleOption)) {
        ANS_LOGE("[GetEnabledForBundleSlot] fail:: write bundle failed");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!data.WriteInt32(slotType)) {
        ANS_LOGE("[GetEnabledForBundleSlot] fail:: write slotType failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(GET_ENABLED_FOR_BUNDLE_SLOT, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[GetEnabledForBundleSlot] fail: transact ErrCode=%{public}d", result);
        return ERR_ANS_TRANSACT_FAILED;
    }

    if (!reply.ReadInt32(result)) {
        ANS_LOGE("[GetEnabledForBundleSlot] fail: read result failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    if (!reply.ReadBool(enabled)) {
        ANS_LOGE("[GetEnabledForBundleSlot] fail: read canPublish failed.");
        return ERR_ANS_PARCELABLE_FAILED;
    }

    return result;
}
}  // namespace Notification
}  // namespace OHOS
