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

#include "distributed_notification_manager.h"

#include <vector>

#include "ans_inner_errors.h"
#include "ans_log_wrapper.h"

namespace OHOS {
namespace Notification {
namespace {
const std::string DELIMITER = "|";
}  // namespace

DistributedNotificationManager::DistributedNotificationManager()
{
    runner_ = OHOS::AppExecFwk::EventRunner::Create();
    handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner_);

    DistributedDatabaseCallback::IDatabaseChange databaseCallback = {
        .OnInsert = std::bind(&DistributedNotificationManager::OnDatabaseInsert,
            this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3),
        .OnUpdate = std::bind(&DistributedNotificationManager::OnDatabaseUpdate,
            this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3),
        .OnDelete = std::bind(&DistributedNotificationManager::OnDatabaseDelete,
            this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3),
    };
    databaseCb_ = std::make_shared<DistributedDatabaseCallback>(databaseCallback);

    DistributedDeviceCallback::IDeviceChange deviceCallback = {
        .OnConnected = std::bind(&DistributedNotificationManager::OnDeviceConnected, this, std::placeholders::_1),
        .OnDisconnected = std::bind(&DistributedNotificationManager::OnDeviceDisconnected, this, std::placeholders::_1),
    };
    deviceCb_ = std::make_shared<DistributedDeviceCallback>(deviceCallback);

    database_ = std::make_shared<DistributedDatabase>(databaseCb_, deviceCb_);
    if (database_ == nullptr) {
        ANS_LOGE("database_ is nullptr.");
        return;
    }
    database_->RecreateDistributedDB();
}

DistributedNotificationManager::~DistributedNotificationManager()
{
    handler_->PostSyncTask(std::bind([&]() { callback_ = {}; }), AppExecFwk::EventHandler::Priority::HIGH);
}

void DistributedNotificationManager::GenerateDistributedKey(
    const std::string &deviceId, const std::string &bundleName, const std::string &label, int32_t id, std::string &key)
{
    key = deviceId + DELIMITER + bundleName + DELIMITER + label + DELIMITER + ToString(id);
}

bool DistributedNotificationManager::GenerateLocalDistributedKey(
    const std::string &bundleName, const std::string &label, int32_t id, std::string &key)
{
    std::string deviceId;
    if (!database_->GetLocalDeviceId(deviceId)) {
        return false;
    }

    GenerateDistributedKey(deviceId, bundleName, label, id, key);
    return true;
}

bool DistributedNotificationManager::ResolveDistributedKey(const std::string &key, ResolveKey &resolveKey)
{
    std::size_t deviceIdPosition = 0;
    std::size_t deviceIdEndPosition = key.find(DELIMITER, deviceIdPosition);
    if (deviceIdEndPosition == std::string::npos) {
        return false;
    }
    std::size_t bundleNamePosition = deviceIdEndPosition + DELIMITER.size();
    std::size_t bundleNameEndPosition = key.find(DELIMITER, bundleNamePosition);
    if (bundleNameEndPosition == std::string::npos) {
        return false;
    }
    std::size_t labelPosition = bundleNameEndPosition + DELIMITER.size();
    std::size_t labelEndPosition = key.find_last_of(DELIMITER) - DELIMITER.size() + 1;
    if (labelEndPosition < labelPosition) {
        return false;
    }
    std::size_t idPosition = key.find_last_of(DELIMITER) + DELIMITER.size();

    resolveKey.deviceId = key.substr(deviceIdPosition, deviceIdEndPosition - deviceIdPosition);
    resolveKey.bundleName = key.substr(bundleNamePosition, bundleNameEndPosition - bundleNamePosition);
    resolveKey.label = key.substr(labelPosition, labelEndPosition - labelPosition);
    resolveKey.id = atoi(&key[idPosition]);

    return true;
}

bool DistributedNotificationManager::CheckDeviceId(const std::string &deviceId, const std::string &key)
{
    ResolveKey resolveKey;
    if (!ResolveDistributedKey(key, resolveKey)) {
        ANS_LOGE("key <%{public}s> is invalid.", key.c_str());
        return false;
    }

    return deviceId == resolveKey.deviceId;
}

void DistributedNotificationManager::OnDatabaseInsert(
    const std::string &deviceId, const std::string &key, const std::string &value)
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    handler_->PostTask(std::bind([=]() {
        if (!CheckDeviceId(deviceId, key)) {
            ANS_LOGW("device id are not the same. deviceId:%{public}s key:%{public}s", deviceId.c_str(), key.c_str());
        }

        ResolveKey resolveKey;
        if (!ResolveDistributedKey(key, resolveKey)) {
            ANS_LOGE("key <%{public}s> is invalid.", key.c_str());
            return;
        }

        sptr<NotificationRequest> request =
            NotificationJsonConverter::ConvertFromJosnString<NotificationRequest>(value);
        if (request == nullptr) {
            ANS_LOGE("convert json to request failed. key:%{public}s", key.c_str());
            return;
        }

        PublishCallback(resolveKey.deviceId, resolveKey.bundleName, request);
    }));
}

void DistributedNotificationManager::OnDatabaseUpdate(
    const std::string &deviceId, const std::string &key, const std::string &value)
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    handler_->PostTask(std::bind([=]() {
        if (!CheckDeviceId(deviceId, key)) {
            ANS_LOGW("device id are not the same. deviceId:%{public}s key:%{public}s", deviceId.c_str(), key.c_str());
        }

        ResolveKey resolveKey;
        if (!ResolveDistributedKey(key, resolveKey)) {
            ANS_LOGE("key <%{public}s> is invalid.", key.c_str());
            return;
        }

        sptr<NotificationRequest> request =
            NotificationJsonConverter::ConvertFromJosnString<NotificationRequest>(value);
        if (request == nullptr) {
            ANS_LOGE("convert json to request failed. key:%{public}s", key.c_str());
            return;
        }

        UpdateCallback(resolveKey.deviceId, resolveKey.bundleName, request);
    }));
}

void DistributedNotificationManager::OnDatabaseDelete(
    const std::string &deviceId, const std::string &key, const std::string &value)
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    handler_->PostTask(std::bind([=]() {
        if (!CheckDeviceId(deviceId, key)) {
            ANS_LOGW("device id are not the same. deviceId:%{public}s key:%{public}s", deviceId.c_str(), key.c_str());
        }

        ResolveKey resolveKey;
        if (!ResolveDistributedKey(key, resolveKey)) {
            ANS_LOGE("key <%{public}s> is invalid.", key.c_str());
            return;
        }

        DeleteCallback(resolveKey.deviceId, resolveKey.bundleName, resolveKey.label, resolveKey.id);
    }));
}

void DistributedNotificationManager::OnDeviceConnected(const std::string &deviceId)
{
    ANS_LOGD("%{public}s", __FUNCTION__);
    handler_->PostTask(std::bind([=]() {}));
    return;
}

void DistributedNotificationManager::OnDeviceDisconnected(const std::string &deviceId)
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    handler_->PostTask(std::bind([=]() {
        std::string prefixKey = deviceId + DELIMITER;
        std::vector<DistributedDatabase::Entry> entries;
        if (!database_->GetEntriesFromDistributedDB(prefixKey, entries)) {
            ANS_LOGE("GetEntriesFromDistributedDB failed.");
            return;
        }

        for (auto index : entries) {
            ResolveKey resolveKey;
            if (!ResolveDistributedKey(index.key.ToString(), resolveKey)) {
                ANS_LOGE("key <%{public}s> is invalid.", index.key.ToString().c_str());
                continue;
            }

            DeleteCallback(resolveKey.deviceId, resolveKey.bundleName, resolveKey.label, resolveKey.id);
        }

        database_->ClearDataByDevice(deviceId);

        std::vector<DistributedDatabase::DeviceInfo> deviceList;
        if (database_->GetDeviceInfoList(deviceList) == ERR_OK && deviceList.empty()) {
            database_->RecreateDistributedDB();
        }
    }));
    return;
}

bool DistributedNotificationManager::PublishCallback(
    const std::string &deviceId, const std::string &bundleName, sptr<NotificationRequest> &request)
{
    ANS_LOGI("callback_.OnPublish start.");
    if (callback_.OnPublish) {
        callback_.OnPublish(deviceId, bundleName, request);
    }
    ANS_LOGI("callback_.OnPublish end.");

    return true;
}

bool DistributedNotificationManager::UpdateCallback(
    const std::string &deviceId, const std::string &bundleName, sptr<NotificationRequest> &request)
{
    ANS_LOGI("callback_.OnUpdate start.");
    if (callback_.OnUpdate) {
        callback_.OnUpdate(deviceId, bundleName, request);
    }
    ANS_LOGI("callback_.OnUpdate end.");

    return true;
}

bool DistributedNotificationManager::DeleteCallback(
    const std::string &deviceId, const std::string &bundleName, const std::string &label, int32_t id)
{
    ANS_LOGI("callback_.OnDelete start.");
    if (callback_.OnDelete) {
        callback_.OnDelete(deviceId, bundleName, label, id);
    }
    ANS_LOGI("callback_.OnDelete end.");

    return true;
}

ErrCode DistributedNotificationManager::Publish(
    const std::string &bundleName, const std::string &label, int32_t id, const sptr<NotificationRequest> &request)
{
    ANS_LOGI("%{public}s start", __FUNCTION__);
    std::string key;
    if (!GenerateLocalDistributedKey(bundleName, label, id, key)) {
        ANS_LOGE("Generate distributed key failed.");
        return ERR_ANS_DISTRIBUTED_GET_INFO_FAILED;
    }

    std::string value;
    if (!NotificationJsonConverter::ConvertToJosnString(request, value)) {
        ANS_LOGE("convert request to json failed. key:%{public}s", key.c_str());
        return ERR_ANS_DISTRIBUTED_OPERATION_FAILED;
    }

    if (!database_->PutToDistributedDB(key, value)) {
        ANS_LOGE("put to distributed DB failed. key:%{public}s", key.c_str());
        return ERR_ANS_DISTRIBUTED_OPERATION_FAILED;
    }

    return ERR_OK;
}

ErrCode DistributedNotificationManager::Update(
    const std::string &bundleName, const std::string &label, int32_t id, const sptr<NotificationRequest> &request)
{
    ANS_LOGI("%{public}s start", __FUNCTION__);
    std::string key;
    if (!GenerateLocalDistributedKey(bundleName, label, id, key)) {
        ANS_LOGE("Generate distributed key failed.");
        return ERR_ANS_DISTRIBUTED_GET_INFO_FAILED;
    }

    std::string value;
    if (!NotificationJsonConverter::ConvertToJosnString(request, value)) {
        ANS_LOGE("convert request to json failed. key:%{public}s", key.c_str());
        return ERR_ANS_DISTRIBUTED_OPERATION_FAILED;
    }

    if (!database_->PutToDistributedDB(key, value)) {
        ANS_LOGE("put to distributed DB failed. key:%{public}s", key.c_str());
        return ERR_ANS_DISTRIBUTED_OPERATION_FAILED;
    }
    return ERR_OK;
}

ErrCode DistributedNotificationManager::Delete(const std::string &bundleName, const std::string &label, int32_t id)
{
    ANS_LOGI("%{public}s start", __FUNCTION__);
    std::string key;
    if (!GenerateLocalDistributedKey(bundleName, label, id, key)) {
        ANS_LOGE("Generate distributed key failed.");
        return ERR_ANS_DISTRIBUTED_GET_INFO_FAILED;
    }

    if (!database_->DeleteToDistributedDB(key)) {
        ANS_LOGE("delete to distributed DB failed. key:%{public}s", key.c_str());
        return ERR_ANS_DISTRIBUTED_OPERATION_FAILED;
    }
    return ERR_OK;
}

ErrCode DistributedNotificationManager::DeleteRemoteNotification(
    const std::string &deviceId, const std::string &bundleName, const std::string &label, int32_t id)
{
    ANS_LOGI("%{public}s start", __FUNCTION__);

    std::string key;
    GenerateDistributedKey(deviceId, bundleName, label, id, key);

    if (!database_->DeleteToDistributedDB(key)) {
        ANS_LOGE("delete to distributed DB failed. key:%{public}s", key.c_str());
        return ERR_ANS_DISTRIBUTED_OPERATION_FAILED;
    }
    return ERR_OK;
}

ErrCode DistributedNotificationManager::RegisterCallback(const IDistributedCallback &callback)
{
    ANS_LOGI("%{public}s start", __FUNCTION__);
    handler_->PostSyncTask(std::bind([&]() { callback_ = callback; }), AppExecFwk::EventHandler::Priority::HIGH);
    return ERR_OK;
}

ErrCode DistributedNotificationManager::UngegisterCallback(void)
{
    ANS_LOGI("%{public}s start", __FUNCTION__);
    handler_->PostSyncTask(std::bind([&]() { callback_ = {}; }), AppExecFwk::EventHandler::Priority::HIGH);
    return ERR_OK;
}

ErrCode DistributedNotificationManager::GetCurrentDistributedNotification(
    std::vector<sptr<NotificationRequest>> &requestList)
{
    ANS_LOGI("%{public}s start", __FUNCTION__);
    std::string prefixKey = "";
    std::vector<DistributedDatabase::Entry> entries;
    if (!database_->GetEntriesFromDistributedDB(prefixKey, entries)) {
        ANS_LOGE("GetEntriesFromDistributedDB failed.");
        return ERR_ANS_DISTRIBUTED_OPERATION_FAILED;
    }

    for (auto index : entries) {
        ResolveKey resolveKey;
        if (!ResolveDistributedKey(index.key.ToString(), resolveKey)) {
            ANS_LOGE("key <%{public}s> is invalid.", index.key.ToString().c_str());
            continue;
        }

        sptr<NotificationRequest> request =
            NotificationJsonConverter::ConvertFromJosnString<NotificationRequest>(index.value.ToString());
        if (request == nullptr) {
            ANS_LOGE("convert json to request failed. key:%{public}s", index.key.ToString().c_str());
            continue;
        }

        PublishCallback(resolveKey.deviceId, resolveKey.bundleName, request);
    }

    return ERR_OK;
}

ErrCode DistributedNotificationManager::GetLocalDeviceInfo(DistributedDatabase::DeviceInfo &deviceInfo)
{
    ANS_LOGI("%{public}s start", __FUNCTION__);

    if (!database_->GetLocalDeviceInfo(deviceInfo)) {
        return ERR_ANS_DISTRIBUTED_OPERATION_FAILED;
    }

    return ERR_OK;
}

ErrCode DistributedNotificationManager::OnDistributedKvStoreDeathRecipient()
{
    ANS_LOGI("%{public}s start", __FUNCTION__);

    database_ = std::make_shared<DistributedDatabase>(databaseCb_, deviceCb_);
    if (database_ == nullptr) {
        ANS_LOGE("database_ is nullptr.");
        return ERR_ANS_NO_MEMORY;
    }
    if (!database_->RecreateDistributedDB()) {
        ANS_LOGE("RecreateDistributedDB failed.");
        return ERR_ANS_DISTRIBUTED_OPERATION_FAILED;
    }
    return ERR_OK;
}
}  // namespace Notification
}  // namespace OHOS