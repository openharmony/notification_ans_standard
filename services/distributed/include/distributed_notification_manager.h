/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_DISTRIBUTED_INCLUDE_NOTIFICATION_MANAGER_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_DISTRIBUTED_INCLUDE_NOTIFICATION_MANAGER_H

#include <string>

#include "singleton.h"

#include "event_handler.h"
#include "event_runner.h"

#include "distributed_database.h"
#include "distributed_database_callback.h"
#include "distributed_device_callback.h"
#include "notification_request.h"

namespace OHOS {
namespace Notification {
class DistributedNotificationManager : public DelayedSingleton<DistributedNotificationManager> {
public:
    /**
     * @brief Distributed notification callback function for remote device.
     */
    struct IDistributedCallback {
        std::function<void(
            const std::string &deviceId, const std::string &bundleName, sptr<NotificationRequest> &request)>
            OnPublish;
        std::function<void(
            const std::string &deviceId, const std::string &bundleName, sptr<NotificationRequest> &request)>
            OnUpdate;
        std::function<void(
            const std::string &deviceId, const std::string &bundleName, const std::string &label, int32_t id)>
            OnDelete;
    };

    /**
     * @brief Publishes a local notification to remote device.
     *
     * @param bundleName Indicates the bundle name of the application whose notifications are to be publish.
     * @param label Indicates the label of the notifications.
     * @param id Indicates the bundle uid of the application whose notifications are to be publish.
     * @param request Indicates the NotificationRequest object for setting the notification content.
     * @return ErrCode Returns the publish result.
     */
    ErrCode Publish(
        const std::string &bundleName, const std::string &label, int32_t id, const sptr<NotificationRequest> &request);

    /**
     * @brief Updates infomation of local notification to remote device.
     *
     * @param bundleName Indicates the bundle name of the application whose notifications are to be update.
     * @param label Indicates the label of the notifications.
     * @param id Indicates the bundle uid of the application whose notifications are to be update.
     * @param request Indicates the NotificationRequest object for setting the notification content.
     * @return ErrCode Returns the update result.
     */
    ErrCode Update(
        const std::string &bundleName, const std::string &label, int32_t id, const sptr<NotificationRequest> &request);

    /**
     * @brief Removes a local notification.
     *
     * @param bundleName Indicates the bundle name of the application whose notifications are to be remove.
     * @param label Indicates the label of the notifications.
     * @param id Indicates the bundle uid of the application whose notifications are to be remove.
     * @return ErrCode Returns the remove result.
     */
    ErrCode Delete(const std::string &bundleName, const std::string &label, int32_t id);

    /**
     * @brief Removes a remote notification.
     *
     * @param deviceId Indicates the ID of the device.
     * @param bundleName Indicates the bundle name of the application whose notifications are to be remove.
     * @param label Indicates the label of the notifications.
     * @param id Indicates the bundle uid of the application whose notifications are to be remove.
     * @return ErrCode Returns the remove result.
     */
    ErrCode DeleteRemoteNotification(
        const std::string &deviceId, const std::string &bundleName, const std::string &label, int32_t id);

    /**
     * @brief Register callback of distributed notification changed.
     *
     * @param callback Indicates the callback structure
     * @return ErrCode Returns the register result.
     */
    ErrCode RegisterCallback(const IDistributedCallback &callback);

    /**
     * @brief Unregister Callback of Distributed notification changed.
     *
     * @return ErrCode Returns the unregister result.
     */
    ErrCode UngegisterCallback(void);

    /**
     * @brief Get all distributed notification in database.
     *
     * @param requestList Indicates the list of NotificationRequest object for setting the notification content.
     * @return ErrCode Returns Get all distributed notification result.
     */
    ErrCode GetCurrentDistributedNotification(std::vector<sptr<NotificationRequest>> &requestList);

    /**
     * @brief Get local device info.
     *
     * @param deviceInfo Indicates the infomation of local device.
     * @return ErrCode Returns get device infomation result.
     */
    ErrCode GetLocalDeviceInfo(DistributedDatabase::DeviceInfo &deviceInfo);

    /**
     * @brief Obtains the death event of the Distributed KvStore service.
     */
    ErrCode OnDistributedKvStoreDeathRecipient();

private:
    void OnDatabaseInsert(const std::string &deviceId, const std::string &key, const std::string &value);
    void OnDatabaseUpdate(const std::string &deviceId, const std::string &key, const std::string &value);
    void OnDatabaseDelete(const std::string &deviceId, const std::string &key, const std::string &value);
    void OnDeviceConnected(const std::string &deviceId);
    void OnDeviceDisconnected(const std::string &deviceId);

private:
    struct ResolveKey {
        std::string deviceId;
        std::string bundleName;
        std::string label;
        int32_t id = 0;
    };

    void GenerateDistributedKey(const std::string &deviceId, const std::string &bundleName, const std::string &label,
        int32_t id, std::string &key);
    bool GenerateLocalDistributedKey(
        const std::string &bundleName, const std::string &label, int32_t id, std::string &key);
    bool ResolveDistributedKey(const std::string &key, ResolveKey &resolveKey);
    bool GetDeviceIdFromKey(const std::string &key, std::string &deviceId);
    bool CheckDeviceId(const std::string &deviceId, const std::string &key);

    bool PublishCallback(
        const std::string &deviceId, const std::string &bundleName, sptr<NotificationRequest> &request);
    bool UpdateCallback(const std::string &deviceId, const std::string &bundleName, sptr<NotificationRequest> &request);
    bool DeleteCallback(
        const std::string &deviceId, const std::string &bundleName, const std::string &label, int32_t id);

private:
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler_ = nullptr;
    std::shared_ptr<DistributedDatabase> database_ = nullptr;

    std::shared_ptr<DistributedDatabaseCallback> databaseCb_;
    std::shared_ptr<DistributedDeviceCallback> deviceCb_;
    IDistributedCallback callback_ = {0};

    DECLARE_DELAYED_SINGLETON(DistributedNotificationManager);
    DISALLOW_COPY_AND_MOVE(DistributedNotificationManager);
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_DISTRIBUTED_INCLUDE_NOTIFICATION_MANAGER_H