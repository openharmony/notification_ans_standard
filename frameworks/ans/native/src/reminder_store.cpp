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

#include "reminder_store.h"

#include <filesystem>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

#include "ability_context.h"
#include "ans_log_wrapper.h"
#include "reminder_request_alarm.h"
#include "reminder_request_calendar.h"
#include "reminder_request_timer.h"

namespace OHOS {
namespace Notification {
namespace {
const std::string REMINDER_DB_DIR = "/data/system_ce/ans_standard/";
const std::string REMINDER_DB_NAME = "reminder.db";
const std::string REMINDER_DB_TABLE = "reminder";
const uint32_t REMINDER_RDB_VERSION = 1;
const int32_t STATE_FAIL = -1;
std::vector<std::string> columns;
}

const int32_t ReminderStore::STATE_OK = 0;

int32_t ReminderStore::ReminderStoreDataCallBack::OnCreate(NativeRdb::RdbStore &store)
{
    ANSR_LOGD("Create table.");
    std::string CREATE_REMINDER_TABLE = "CREATE TABLE IF NOT EXISTS " + REMINDER_DB_TABLE + " ("
        + ReminderRequest::sqlOfAddColumns + ")";
    ANSR_LOGD("CreateTable:%{public}s", CREATE_REMINDER_TABLE.c_str());
    return store.ExecuteSql(CREATE_REMINDER_TABLE);
}

int32_t ReminderStore::ReminderStoreDataCallBack::OnUpgrade(
    NativeRdb::RdbStore &store, int32_t oldVersion, int32_t newVersion)
{
    return NativeRdb::E_OK;
}

int32_t ReminderStore::Init()
{
    ANSR_LOGD("Reminder store init.");
    int32_t errCode(STATE_FAIL);

    if (access(REMINDER_DB_DIR.c_str(), F_OK) != 0) {
        int createDir = mkdir(REMINDER_DB_DIR.c_str(), S_IRWXU);
        if (createDir != 0) {
            ANSR_LOGE("Failed to create directory %{public}s", REMINDER_DB_DIR.c_str());
            return errCode;
        }
    }

    ReminderRequest::InitDbColumns();
    ReminderRequestCalendar::InitDbColumns();
    ReminderRequestAlarm::InitDbColumns();
    columns.insert(columns.begin(),
        ReminderRequest::columns.begin(), ReminderRequest::columns.end());

    std::string dbConfig = REMINDER_DB_DIR + REMINDER_DB_NAME;
    NativeRdb::RdbStoreConfig config_(dbConfig);
    ReminderStoreDataCallBack rdbDataCallBack_;
    rdbStore_ = NativeRdb::RdbHelper::GetRdbStore(config_, REMINDER_RDB_VERSION, rdbDataCallBack_, errCode);
    if (rdbStore_ == nullptr) {
        ANSR_LOGE("ReminderStore init fail, errCode %{public}d.", errCode);
        return errCode;
    }
    return ReminderStore::InitData();
}

int32_t ReminderStore::InitData()
{
    ANSR_LOGD("Reminder data init.");
    if (rdbStore_ == nullptr) {
        ANSR_LOGE("Rdb store is not initialized.");
        return STATE_FAIL;
    }
    std::string deleteCondition = ReminderRequest::IS_EXPIRED + " is true";
    ReminderStore::Delete(deleteCondition);

    int32_t statusChangedRows = STATE_FAIL;
    NativeRdb::ValuesBucket statusValues;
    statusValues.PutInt(ReminderRequest::STATE, ReminderRequest::REMINDER_STATUS_INACTIVE);
    int32_t statusResult = rdbStore_->Update(statusChangedRows, REMINDER_DB_TABLE, statusValues);
    ANSR_LOGD("Change status to inactive, changed rows: %{public}d.", statusChangedRows);
    if (statusResult != NativeRdb::E_OK) {
        ANSR_LOGE("Init data failed.");
        return STATE_FAIL;
    }

    int32_t activeChangedRows = STATE_FAIL;
    NativeRdb::ValuesBucket activeValues;
    activeValues.PutString(ReminderRequest::IS_ACTIVE, "false");
    std::string activeUpdateCondition = ReminderRequest::IS_ACTIVE + " is true";
    std::vector<std::string> activeWhereArgs;
    int32_t activeResult = rdbStore_->Update(
        activeChangedRows, REMINDER_DB_TABLE, activeValues, activeUpdateCondition, activeWhereArgs);
    ANSR_LOGD("Change status isActive to false, changed rows: %{public}d.", activeChangedRows);
    if (activeResult != NativeRdb::E_OK) {
        ANSR_LOGE("Init data failed.");
        return STATE_FAIL;
    }

    int32_t scheduledChangedRows = STATE_FAIL;
    NativeRdb::ValuesBucket scheduledValues;
    scheduledValues.PutString(ReminderRequest::HAS_SCHEDULED_TIMEOUT, "false");
    std::string scheduledUpdateCondition = ReminderRequest::HAS_SCHEDULED_TIMEOUT + " is true";
    std::vector<std::string> scheduledWhereArgs;
    int32_t scheduledResult = rdbStore_->Update(
        scheduledChangedRows, REMINDER_DB_TABLE, scheduledValues, scheduledUpdateCondition, scheduledWhereArgs);
    ANSR_LOGD("Change status has_ScheduledTimeout to false, changed rows: %{public}d.", scheduledChangedRows);
    if (scheduledResult != NativeRdb::E_OK) {
        ANSR_LOGE("Init data failed.");
        return STATE_FAIL;
    }
    return ReminderStore::STATE_OK;
}

int32_t ReminderStore::Delete(int32_t reminderId)
{
    std::string deleteCondition = ReminderRequest::REMINDER_ID
        + " = " + std::to_string(reminderId);
    return ReminderStore::Delete(deleteCondition);
}

int32_t ReminderStore::DeleteUser(int32_t userId)
{
    std::string deleteCondition = ReminderRequest::USER_ID + " = " + std::to_string(userId);
    return ReminderStore::Delete(deleteCondition);
}

int32_t ReminderStore::Delete(const std::string &pkg, int32_t userId)
{
    std::string deleteCondition = ReminderRequest::PKG_NAME + " = " + pkg + " and "
        + ReminderRequest::USER_ID + " = " + std::to_string(userId);
    return ReminderStore::Delete(deleteCondition);
}

int32_t ReminderStore::Delete(const std::string &deleteCondition)
{
    if (rdbStore_ == nullptr) {
        ANSR_LOGE("Rdb store is not initialized.");
        return STATE_FAIL;
    }
    int32_t deletedRows = STATE_FAIL;
    std::vector<std::string> whereArgs;
    int32_t result = rdbStore_->Delete(deletedRows, REMINDER_DB_TABLE, deleteCondition, whereArgs);
    if (result != NativeRdb::E_OK) {
        ANSR_LOGE("Delete operation failed, deleteConditon: %{public}s," \
            "result: %{public}d.", deleteCondition.c_str(), result);
    }
    ANSR_LOGD("Delete operation done, deleteConditon: %{public}s," \
        "deleted rows: %{public}d.", deleteCondition.c_str(), deletedRows);
    return deletedRows;
}

int64_t ReminderStore::UpdateOrInsert(
    const sptr<ReminderRequest> &reminder, const sptr<NotificationBundleOption> &bundleOption)
{
    if (reminder->GetReminderType() == ReminderRequest::ReminderType::TIMER) {
        ANSR_LOGI("Countdown not support persist.");
        return STATE_FAIL;
    }
    int64_t isSuccess = STATE_FAIL;
    if (rdbStore_ == nullptr) {
        ANSR_LOGE("Rdb store is not initialized.");
        return isSuccess;
    }
    if (bundleOption == nullptr) {
        ANSR_LOGE("BundleOption is null.");
        return isSuccess;
    }
    if (IsReminderExist(reminder)) {
        isSuccess = Update(reminder, bundleOption);
    } else {
        isSuccess = Insert(reminder, bundleOption);
    }
    return isSuccess;
}

int64_t ReminderStore::Insert(
    const sptr<ReminderRequest> &reminder, const sptr<NotificationBundleOption> &bundleOption)
{
    if (rdbStore_ == nullptr) {
        ANSR_LOGE("Rdb store is not initialized.");
        return STATE_FAIL;
    }
    int64_t rowId = STATE_FAIL;
    NativeRdb::ValuesBucket values;
    ReminderStore::GenerateData(reminder, bundleOption, values);
    int32_t result = rdbStore_->Insert(rowId, REMINDER_DB_TABLE, values);
    if (result != NativeRdb::E_OK) {
        ANSR_LOGE("Insert operation failed, result: %{public}d, reminderId=%{public}d.",
            result, reminder->GetReminderId());
        return result;
    }
    ANSR_LOGD("Insert successfully, reminderId=%{public}d.", reminder->GetReminderId());
    return result;
}

int64_t ReminderStore::Update(
    const sptr<ReminderRequest> &reminder, const sptr<NotificationBundleOption> &bundleOption)
{
    if (rdbStore_ == nullptr) {
        ANSR_LOGE("Rdb store is not initialized.");
        return STATE_FAIL;
    }
    int32_t changedRows = STATE_FAIL;
    NativeRdb::ValuesBucket values;
    ReminderStore::GenerateData(reminder, bundleOption, values);
    std::string updateCondition = ReminderRequest::REMINDER_ID
        + " = " + std::to_string(reminder->GetReminderId());
    std::vector<std::string> whereArgs;
    int32_t result = rdbStore_->Update(changedRows, REMINDER_DB_TABLE, values, updateCondition, whereArgs);
    if ((result != NativeRdb::E_OK) || (changedRows <= 0)) {
        ANSR_LOGE("Update operation failed, result: %{public}d, updated rows: %{public}d, reminderId=%{public}d.",
            result, changedRows, reminder->GetReminderId());
        return result;
    }
    ANSR_LOGD("Update successfully, updated rows: %{public}d, reminderId=%{public}d.",
        changedRows, reminder->GetReminderId());
    return result;
}

bool ReminderStore::IsReminderExist(const sptr<ReminderRequest> &reminder)
{
    std::string queryCondition = "select " + ReminderRequest::REMINDER_ID
        + " from " + REMINDER_DB_TABLE + " where "
        + ReminderRequest::REMINDER_ID + " = " + std::to_string(reminder->GetReminderId());
    std::vector<std::string> whereArgs;
    std::unique_ptr<NativeRdb::AbsSharedResultSet> queryResultSet = rdbStore_->QuerySql(queryCondition, whereArgs);
    if (queryResultSet == nullptr) {
        ANSR_LOGE("QueryResultSet is null.");
        return false;
    }
    int32_t resultNum;
    queryResultSet->GetRowCount(resultNum);
    if (resultNum == 0) {
        return false;
    }
    return true;
}

std::shared_ptr<NativeRdb::AbsSharedResultSet> ReminderStore::Query(const std::string &queryCondition) const
{
    std::unique_ptr<NativeRdb::AbsSharedResultSet> queryResultSet;
    if (rdbStore_ == nullptr) {
        ANSR_LOGE("Rdb store is not initialized.");
        return queryResultSet;
    }
    std::vector<std::string> whereArgs;
    queryResultSet = rdbStore_->QuerySql(queryCondition, whereArgs);
    return queryResultSet;
}

uint8_t ReminderStore::GetColumnIndex(const std::string& name)
{
    uint8_t index = 0;
    for (auto it = columns.begin(); it != columns.end(); ++it) {
        if (name == (*it)) {
            break;
        }
        index++;
    }
    return index;
}

int32_t ReminderStore::GetMaxId()
{
    if (rdbStore_ == nullptr) {
        ANSR_LOGE("Rdb store is not initialized.");
        return STATE_FAIL;
    }
    std::string queryCondition = "select " + ReminderRequest::REMINDER_ID
        + " from " + REMINDER_DB_TABLE + " order by "
        + ReminderRequest::REMINDER_ID + " desc";
    std::shared_ptr<NativeRdb::AbsSharedResultSet> queryResultSet = ReminderStore::Query(queryCondition);
    if (queryResultSet == nullptr) {
        ANSR_LOGE("QueryResultSet is null.");
        return STATE_FAIL;
    }
    int32_t resultNum;
    queryResultSet->GetRowCount(resultNum);
    if (resultNum == 0) {
        ANSR_LOGI("QueryResultSet is zero.");
        return STATE_FAIL;
    }
    queryResultSet->GoToNextRow();
    int32_t maxId = STATE_FAIL;
    int32_t result = queryResultSet->GetInt(0, maxId);
    if (result != NativeRdb::E_OK) {
        ANSR_LOGE("Query operation failed, result %{public}d.", result);
    }
    ANSR_LOGD("MaxId: %{public}d.", maxId);
    return maxId;
}

std::vector<sptr<ReminderRequest>> ReminderStore::GetAllValidReminders()
{
    std::string queryCondition = "select * from " + REMINDER_DB_TABLE + " where "
        + ReminderRequest::IS_EXPIRED + " is false order by "
        + ReminderRequest::TRIGGER_TIME + " asc";
    ANSR_LOGD("Get all reminders");
    return GetReminders(queryCondition);
}

std::vector<sptr<ReminderRequest>> ReminderStore::GetReminders(const std::string &queryCondition)
{
    std::vector<sptr<ReminderRequest>> reminders;
    if (rdbStore_ == nullptr) {
        ANSR_LOGE("Rdb store is not initialized.");
        return reminders;
    }
    std::shared_ptr<NativeRdb::AbsSharedResultSet> queryResultSet = Query(queryCondition);
    if (queryResultSet == nullptr) {
        return reminders;
    }
    bool isAtLastRow = false;
    queryResultSet->IsAtLastRow(isAtLastRow);
    while (!isAtLastRow) {
        queryResultSet->GoToNextRow();
        sptr<ReminderRequest> reminder;
        reminder = BuildReminder(queryResultSet);
        reminders.push_back(reminder);
        queryResultSet->IsAtLastRow(isAtLastRow);
    }
    ANSR_LOGD("Size=%{public}zu", reminders.size());
    return reminders;
}

sptr<ReminderRequest> ReminderStore::BuildReminder(const std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet)
{
    int32_t reminderType;
    int32_t reminderId;
    resultSet->GetInt(ReminderStore::GetColumnIndex(ReminderRequest::REMINDER_TYPE), reminderType);
    resultSet->GetInt(ReminderStore::GetColumnIndex(ReminderRequest::REMINDER_ID), reminderId);

    sptr<ReminderRequest> reminder = nullptr;
    switch (reminderType) {
        case (static_cast<int32_t>(ReminderRequest::ReminderType::TIMER)): {
            reminder = new (std::nothrow) ReminderRequestTimer(reminderId);
            break;
        }
        case (static_cast<int32_t>(ReminderRequest::ReminderType::CALENDAR)): {
            reminder = new (std::nothrow) ReminderRequestCalendar(reminderId);
            break;
        }
        case (static_cast<int32_t>(ReminderRequest::ReminderType::ALARM)): {
            reminder = new (std::nothrow) ReminderRequestAlarm(reminderId);
            break;
        }
        default: {
            ANSR_LOGE("ReminderType from database is error, reminderType %{public}d.", reminderType);
            break;
        }
    }
    if (reminder != nullptr) {
        reminder->RecoverFromDb(resultSet);
        ANSR_LOGI("BuildReminder success.");
    } else {
        ANSR_LOGW("BuildReminder fail.");
    }
    return reminder;
}

bool ReminderStore::GetBundleOption(const int32_t &reminderId, sptr<NotificationBundleOption> &bundleOption) const
{
    std::string queryCondition = "select " + ReminderRequest::PKG_NAME + ", "
        + ReminderRequest::UID + " from " + REMINDER_DB_TABLE + " where "
        + ReminderRequest::REMINDER_ID + "=" + std::to_string(reminderId);
    std::shared_ptr<NativeRdb::AbsSharedResultSet> queryResultSet = Query(queryCondition);
    if (queryResultSet == nullptr) {
        return false;
    }
    bool isAtLastRow = false;
    queryResultSet->IsAtLastRow(isAtLastRow);
    if (isAtLastRow) {
        return false;
    }
    queryResultSet->GoToNextRow();
    std::string pkgName;
    GetStringVal(queryResultSet, ReminderRequest::PKG_NAME, pkgName);
    int32_t uid;
    GetInt32Val(queryResultSet, ReminderRequest::UID, uid);
    bundleOption->SetBundleName(pkgName);
    bundleOption->SetUid(uid);
    return true;
}

void ReminderStore::GetInt32Val(std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet,
    const std::string &name, int32_t &value) const
{
    int32_t columnIndex;
    resultSet->GetColumnIndex(name, columnIndex);
    resultSet->GetInt(columnIndex, value);
}

void ReminderStore::GetStringVal(std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet,
    const std::string &name, std::string &value) const
{
    int32_t columnIndex;
    resultSet->GetColumnIndex(name, columnIndex);
    resultSet->GetString(columnIndex, value);
}

void ReminderStore::GenerateData(const sptr<ReminderRequest> &reminder,
    const sptr<NotificationBundleOption> &bundleOption, NativeRdb::ValuesBucket &values) const
{
    ReminderRequest::AppendValuesBucket(reminder, bundleOption, values);
    ReminderRequestCalendar::AppendValuesBucket(reminder, bundleOption, values);
    ReminderRequestAlarm::AppendValuesBucket(reminder, bundleOption, values);
}
}  // namespace Notification
}  // namespace OHOS
