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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_REMINDER_STORE_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_REMINDER_STORE_H

#include <vector>

#include "notification_bundle_option.h"
#include "reminder_request.h"
#include "rdb_errno.h"
#include "rdb_helper.h"
#include "rdb_open_callback.h"
#include "rdb_store_config.h"

namespace OHOS {
namespace Notification {
class ReminderStore {
public:
    ReminderStore() {};
    virtual ~ReminderStore() {};
    int32_t Init();
    int32_t Delete(int32_t reminderId);
    int32_t Delete(const std::string &pkg, int32_t userId);
    int32_t DeleteUser(int32_t userId);
    std::vector<sptr<ReminderRequest>> GetAllValidReminders();
    bool GetBundleOption(const int32_t &reminderId, sptr<NotificationBundleOption> &bundleOption) const;
    int32_t GetMaxId();
    int64_t UpdateOrInsert(const sptr<ReminderRequest> &reminder, const sptr<NotificationBundleOption> &bundleOption);
    static uint8_t GetColumnIndex(const std::string& name);

    static const int32_t STATE_OK;

private:
    /**
     * @brief Inits the data in database when system boot on or proxy process reboot on.
     *
     * 1. Deletes all the reminders which IS_EXPIRED is true.
     * 2. Sets all the value of STATE to ReminderRequest::REMINDER_STATUS_INACTIVE
     * 3. Sets all the value of IS_ACTIVE to false.
     * 4. Sets all the value of HAS_SCHEDULED_TIMEOUT to false.
     *
     * @return int32_t result code.
     */
    int32_t InitData();
    sptr<ReminderRequest> BuildReminder(const std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet);
    int32_t Delete(const std::string &deleteCondition);
    void GetInt32Val(std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet,
        const std::string &name, int32_t &value) const;
    void GetStringVal(std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet,
        const std::string &name, std::string &value) const;
    std::vector<sptr<ReminderRequest>> GetReminders(const std::string &queryCondition);
    void GenerateData(const sptr<ReminderRequest> &reminder,
        const sptr<NotificationBundleOption> &bundleOption, NativeRdb::ValuesBucket &values) const;
    bool IsReminderExist(const sptr<ReminderRequest> &reminder);
    int64_t Insert(const sptr<ReminderRequest> &reminder, const sptr<NotificationBundleOption> &bundleOption);
    std::shared_ptr<NativeRdb::AbsSharedResultSet> Query(const std::string &queryCondition) const;
    int64_t Update(const sptr<ReminderRequest> &reminder, const sptr<NotificationBundleOption> &bundleOption);

class ReminderStoreDataCallBack : public NativeRdb::RdbOpenCallback {
public:
    int32_t OnCreate(NativeRdb::RdbStore &rdbStore) override;
    int32_t OnUpgrade(NativeRdb::RdbStore &rdbStore, int32_t oldVersion, int32_t newVersion) override;
};

private:
    std::shared_ptr<NativeRdb::RdbStore> rdbStore_ = nullptr;
};
}  // namespace Notification
}  // namespace OHOS
#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_REMINDER_STORE_H