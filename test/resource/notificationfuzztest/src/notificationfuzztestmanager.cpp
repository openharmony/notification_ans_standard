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

#include "datetime_ex.h"
#include "../include/notificationfuzzconfigparser.h"
#include "../include/notificationfuzztestmanager.h"
#include "../include/notificationgetparam.h"

using namespace OHOS::AppExecFwk;
using namespace OHOS::EventFwk;
namespace OHOS {
namespace Notification {
std::shared_ptr<NotificationFuzzTestManager> NotificationFuzzTestManager::instance = nullptr;
// RegisterNotificationHelper
void NotificationFuzzTestManager::RegisterNotificationHelper()
{
    callFunctionMap_.emplace("NotificationHelperAddNotificationSlot",
        []() { OHOS::Notification::NotificationHelper::AddNotificationSlot(*GetParamNotificationSlot()); });

    callFunctionMap_.emplace("NotificationHelperAddSlotByType",
        []() { OHOS::Notification::NotificationHelper::AddSlotByType(GetParamSlotType()); });

    callFunctionMap_.emplace("NotificationHelperAddNotificationSlots",
        []() { OHOS::Notification::NotificationHelper::AddNotificationSlots(GetParamNotificationSlotVector()); });

    callFunctionMap_.emplace("NotificationHelperRemoveNotificationSlot",
        []() { OHOS::Notification::NotificationHelper::RemoveNotificationSlot(GetParamSlotType()); });

    callFunctionMap_.emplace(
        "NotificationHelperRemoveAllSlots", []() { OHOS::Notification::NotificationHelper::RemoveAllSlots(); });

    callFunctionMap_.emplace("NotificationHelperGetNotificationSlot", []() {
        sptr<OHOS::Notification::NotificationSlot> param = GetParamNotificationSlotSptr();
        OHOS::Notification::NotificationHelper::GetNotificationSlot(GetParamSlotType(), param);
    });

    callFunctionMap_.emplace("NotificationHelperGetNotificationSlots", []() {
        std::vector<sptr<OHOS::Notification::NotificationSlot>> param = GetParamNotificationSlotSptrVector();
        OHOS::Notification::NotificationHelper::GetNotificationSlots(param);
    });

    callFunctionMap_.emplace("NotificationHelperAddNotificationSlotGroup",
        []() { OHOS::Notification::NotificationHelper::AddNotificationSlotGroup(*GetParamNotificationSlotGroup()); });

    callFunctionMap_.emplace("NotificationHelperAddNotificationSlotGroups", []() {
        OHOS::Notification::NotificationHelper::AddNotificationSlotGroups(GetParamNotificationSlotGroupVector());
    });

    callFunctionMap_.emplace("NotificationHelperRemoveNotificationSlotGroup",
        []() { OHOS::Notification::NotificationHelper::RemoveNotificationSlotGroup(GetStringParam()); });

    callFunctionMap_.emplace("NotificationHelperGetNotificationSlotGroup", []() {
        sptr<OHOS::Notification::NotificationSlotGroup> param = GetParamNotificationSlotGroup();
        OHOS::Notification::NotificationHelper::GetNotificationSlotGroup(GetStringParam(), param);
    });

    callFunctionMap_.emplace("NotificationHelperGetNotificationSlotGroups", []() {
        std::vector<sptr<OHOS::Notification::NotificationSlotGroup>> param = GetParamNotificationSlotGroupSptrVector();
        OHOS::Notification::NotificationHelper::GetNotificationSlotGroups(param);
    });

    callFunctionMap_.emplace("NotificationHelperGetNotificationSlotNumAsBundle", []() {
        int param = GetIntParam();
        OHOS::Notification::NotificationHelper::GetNotificationSlotNumAsBundle(
            *GetParamNotificationBundleOption(), param);
    });

    callFunctionMap_.emplace("NotificationHelperPublishNotificationNotificationRequest",
        []() { OHOS::Notification::NotificationHelper::PublishNotification(*GetParamNotificationRequest()); });

    callFunctionMap_.emplace("NotificationHelperPublishNotificationstringNotificationRequest", []() {
        OHOS::Notification::NotificationHelper::PublishNotification(GetStringParam(), *GetParamNotificationRequest());
    });

    callFunctionMap_.emplace("NotificationHelperPublishNotificationNotificationRequeststring", []() {
        OHOS::Notification::NotificationHelper::PublishNotification(*GetParamNotificationRequest(), GetStringParam());
    });

    callFunctionMap_.emplace("NotificationHelperCancelNotificationint32_t",
        []() { OHOS::Notification::NotificationHelper::CancelNotification(GetS32Param()); });

    callFunctionMap_.emplace("NotificationHelperCancelNotificationstringint32_t",
        []() { OHOS::Notification::NotificationHelper::CancelNotification(GetStringParam(), GetS32Param()); });

    callFunctionMap_.emplace("NotificationHelperCancelAllNotifications",
        []() { OHOS::Notification::NotificationHelper::CancelAllNotifications(); });

    callFunctionMap_.emplace("NotificationHelperGetActiveNotificationNums", []() {
        int32_t notificationId = GetS32Param();
        OHOS::Notification::NotificationHelper::GetActiveNotificationNums(notificationId);
    });

    callFunctionMap_.emplace("NotificationHelperGetActiveNotifications", []() {
        std::vector<sptr<OHOS::Notification::NotificationRequest>> param = GetParamNotificationRequestVector();
        OHOS::Notification::NotificationHelper::GetActiveNotifications(param);
    });

    callFunctionMap_.emplace("NotificationHelperGetCurrentAppSorting", []() {
        sptr<OHOS::Notification::NotificationSortingMap> sortingMap = GetParamNotificationSortingMapSptr();
        OHOS::Notification::NotificationHelper::GetCurrentAppSorting(sortingMap);
    });

    callFunctionMap_.emplace("NotificationHelperSetNotificationAgent",
        []() { OHOS::Notification::NotificationHelper::SetNotificationAgent(GetStringParam()); });

    callFunctionMap_.emplace("NotificationHelperGetNotificationAgent", []() {
        std::string agent = GetStringParam();
        OHOS::Notification::NotificationHelper::GetNotificationAgent(agent);
    });

    callFunctionMap_.emplace("NotificationHelperCanPublishNotificationAsBundle", []() {
        bool canPublish = GetBoolParam();
        OHOS::Notification::NotificationHelper::CanPublishNotificationAsBundle(GetStringParam(), canPublish);
    });

    callFunctionMap_.emplace("NotificationHelperPublishNotificationAsBundle", []() {
        OHOS::Notification::NotificationHelper::PublishNotificationAsBundle(
            GetStringParam(), *GetParamNotificationRequest());
    });

    callFunctionMap_.emplace("NotificationHelperSetNotificationBadgeNum",
        []() { OHOS::Notification::NotificationHelper::SetNotificationBadgeNum(); });

    callFunctionMap_.emplace("NotificationHelperSetNotificationBadgeNumint32_t", []() {
        int32_t num = GetS32Param();
        OHOS::Notification::NotificationHelper::SetNotificationBadgeNum(num);
    });

    callFunctionMap_.emplace("NotificationHelperIsAllowedNotify", []() {
        bool allowed = GetBoolParam();
        OHOS::Notification::NotificationHelper::IsAllowedNotify(allowed);
    });

    callFunctionMap_.emplace("NotificationHelperAreNotificationsSuspended", []() {
        bool suspended = GetBoolParam();
        OHOS::Notification::NotificationHelper::AreNotificationsSuspended(suspended);
    });

    callFunctionMap_.emplace("NotificationHelperHasNotificationPolicyAccessPermission", []() {
        bool hasPermission = GetBoolParam();
        OHOS::Notification::NotificationHelper::HasNotificationPolicyAccessPermission(hasPermission);
    });

    callFunctionMap_.emplace("NotificationHelperGetBundleImportance", []() {
        OHOS::Notification::NotificationSlot::NotificationLevel importance = GetParamNotificationLevel();
        OHOS::Notification::NotificationHelper::GetBundleImportance(importance);
    });

    callFunctionMap_.emplace("NotificationHelperSubscribeNotificationNotificationSubscriber", []() {
        TestAnsSubscriber::mutex.lock();
        std::shared_ptr<OHOS::Notification::NotificationSubscriber> subscriber = GetParamNotificationSubscriber();
        struct tm start = {0};
        OHOS::GetSystemCurrentTime(&start);
        OHOS::Notification::NotificationHelper::SubscribeNotification(*subscriber);
        struct tm end = {0};
        int64_t timeout = 0;
        while (!TestAnsSubscriber::mutex.try_lock()) {
            OHOS::GetSystemCurrentTime(&end);
            timeout = OHOS::GetSecondsBetween(start, end);
            if (timeout >= 5L) {
                break;
            }
        }
        TestAnsSubscriber::mutex.unlock();
        TestAnsSubscriber::mutex.lock();
        OHOS::GetSystemCurrentTime(&start);
        OHOS::Notification::NotificationHelper::UnSubscribeNotification(*subscriber);
        while (!TestAnsSubscriber::mutex.try_lock()) {
            OHOS::GetSystemCurrentTime(&end);
            timeout = OHOS::GetSecondsBetween(start, end);
            if (timeout >= 5L) {
                break;
            }
        }
        TestAnsSubscriber::mutex.unlock();
    });

    callFunctionMap_.emplace(
        "NotificationHelperSubscribeNotificationNotificationSubscriberNotificationSubscribeInfo", []() {
            TestAnsSubscriber::mutex.lock();
            std::shared_ptr<OHOS::Notification::NotificationSubscriber> subscriber = GetParamNotificationSubscriber();
            std::shared_ptr<OHOS::Notification::NotificationSubscribeInfo> subscribeInfo =
                GetParamNotificationSubscribeInfo();
            struct tm start = {0};
            OHOS::GetSystemCurrentTime(&start);
            OHOS::Notification::NotificationHelper::SubscribeNotification(*subscriber, *subscribeInfo);
            struct tm end = {0};
            int64_t timeout = 0;
            while (!TestAnsSubscriber::mutex.try_lock()) {
                OHOS::GetSystemCurrentTime(&end);
                timeout = OHOS::GetSecondsBetween(start, end);
                if (timeout >= 5L) {
                    break;
                }
            }
            TestAnsSubscriber::mutex.unlock();
            TestAnsSubscriber::mutex.lock();
            OHOS::GetSystemCurrentTime(&start);
            OHOS::Notification::NotificationHelper::UnSubscribeNotification(*subscriber, *subscribeInfo);
            while (!TestAnsSubscriber::mutex.try_lock()) {
                OHOS::GetSystemCurrentTime(&end);
                timeout = OHOS::GetSecondsBetween(start, end);
                if (timeout >= 5L) {
                    break;
                }
            }
            TestAnsSubscriber::mutex.unlock();
        });

    callFunctionMap_.emplace("NotificationHelperUnSubscribeNotificationNotificationSubscriber", []() {
        TestAnsSubscriber::mutex.lock();
        std::shared_ptr<OHOS::Notification::NotificationSubscriber> subscriber = GetParamNotificationSubscriber();
        struct tm start = {0};
        OHOS::GetSystemCurrentTime(&start);
        OHOS::Notification::NotificationHelper::SubscribeNotification(*subscriber);
        struct tm end = {0};
        int64_t timeout = 0;
        while (!TestAnsSubscriber::mutex.try_lock()) {
            OHOS::GetSystemCurrentTime(&end);
            timeout = OHOS::GetSecondsBetween(start, end);
            if (timeout >= 5L) {
                break;
            }
        }
        TestAnsSubscriber::mutex.unlock();
        TestAnsSubscriber::mutex.lock();
        OHOS::GetSystemCurrentTime(&start);
        OHOS::Notification::NotificationHelper::UnSubscribeNotification(*subscriber);
        while (!TestAnsSubscriber::mutex.try_lock()) {
            OHOS::GetSystemCurrentTime(&end);
            timeout = OHOS::GetSecondsBetween(start, end);
            if (timeout >= 5L) {
                break;
            }
        }
        TestAnsSubscriber::mutex.unlock();
    });

    callFunctionMap_.emplace(
        "NotificationHelperUnSubscribeNotificationNotificationSubscriberNotificationSubscribeInfo", []() {
            TestAnsSubscriber::mutex.lock();
            std::shared_ptr<OHOS::Notification::NotificationSubscriber> subscriber = GetParamNotificationSubscriber();
            std::shared_ptr<OHOS::Notification::NotificationSubscribeInfo> subscribeInfo =
                GetParamNotificationSubscribeInfo();
            struct tm start = {0};
            OHOS::GetSystemCurrentTime(&start);
            OHOS::Notification::NotificationHelper::SubscribeNotification(*subscriber, *subscribeInfo);
            struct tm end = {0};
            int64_t timeout = 0;
            while (!TestAnsSubscriber::mutex.try_lock()) {
                OHOS::GetSystemCurrentTime(&end);
                timeout = OHOS::GetSecondsBetween(start, end);
                if (timeout >= 5L) {
                    break;
                }
            }
            TestAnsSubscriber::mutex.unlock();
            TestAnsSubscriber::mutex.lock();
            OHOS::GetSystemCurrentTime(&start);
            OHOS::Notification::NotificationHelper::UnSubscribeNotification(*subscriber, *subscribeInfo);
            while (!TestAnsSubscriber::mutex.try_lock()) {
                OHOS::GetSystemCurrentTime(&end);
                timeout = OHOS::GetSecondsBetween(start, end);
                if (timeout >= 5L) {
                    break;
                }
            }
            TestAnsSubscriber::mutex.unlock();
        });

    callFunctionMap_.emplace("NotificationHelperRemoveNotificationstring",
        []() { OHOS::Notification::NotificationHelper::RemoveNotification(GetStringParam()); });

    callFunctionMap_.emplace("NotificationHelperRemoveNotificationNotificationBundleOptionint32_tstring", []() {
        OHOS::Notification::NotificationHelper::RemoveNotification(
            *GetParamNotificationBundleOption(), GetS32Param(), GetStringParam());
    });

    callFunctionMap_.emplace("NotificationHelperRemoveAllNotifications",
        []() { OHOS::Notification::NotificationHelper::RemoveAllNotifications(*GetParamNotificationBundleOption()); });

    callFunctionMap_.emplace("NotificationHelperRemoveNotifications",
        []() { OHOS::Notification::NotificationHelper::RemoveNotifications(); });

    callFunctionMap_.emplace("NotificationHelperRemoveNotificationsByBundle", []() {
        OHOS::Notification::NotificationHelper::RemoveNotificationsByBundle(*GetParamNotificationBundleOption());
    });

    callFunctionMap_.emplace("NotificationHelperGetNotificationSlotsForBundle", []() {
        std::vector<sptr<OHOS::Notification::NotificationSlot>> param = GetParamNotificationSlotSptrVector();
        OHOS::Notification::NotificationHelper::GetNotificationSlotsForBundle(
            *GetParamNotificationBundleOption(), param);
    });

    callFunctionMap_.emplace("NotificationHelperUpdateNotificationSlots", []() {
        OHOS::Notification::NotificationHelper::UpdateNotificationSlots(
            *GetParamNotificationBundleOption(), GetParamNotificationSlotSptrVector());
    });

    callFunctionMap_.emplace("NotificationHelperUpdateNotificationSlotGroups", []() {
        OHOS::Notification::NotificationHelper::UpdateNotificationSlotGroups(
            *GetParamNotificationBundleOption(), GetParamNotificationSlotGroupSptrVector());
    });

    callFunctionMap_.emplace("NotificationHelperGetAllActiveNotificationsvectorsptrNotification", []() {
        std::vector<sptr<OHOS::Notification::Notification>> notification = GetParamNotificationSptrVector();
        OHOS::Notification::NotificationHelper::GetAllActiveNotifications(notification);
    });

    callFunctionMap_.emplace("NotificationHelperGetAllActiveNotificationsstringNotification", []() {
        std::vector<sptr<OHOS::Notification::Notification>> notification = GetParamNotificationSptrVector();
        OHOS::Notification::NotificationHelper::GetAllActiveNotifications(GetStringVectorParam(), notification);
    });

    callFunctionMap_.emplace("NotificationHelperIsAllowedNotify", []() {
        bool param = GetBoolParam();
        OHOS::Notification::NotificationHelper::IsAllowedNotify(*GetParamNotificationBundleOption(), param);
    });

    callFunctionMap_.emplace("NotificationHelperSetNotificationsEnabledForAllBundles", []() {
        OHOS::Notification::NotificationHelper::SetNotificationsEnabledForAllBundles(GetStringParam(), GetBoolParam());
    });

    callFunctionMap_.emplace("NotificationHelperSetNotificationsEnabledForDefaultBundle", []() {
        OHOS::Notification::NotificationHelper::SetNotificationsEnabledForDefaultBundle(
            GetStringParam(), GetBoolParam());
    });

    callFunctionMap_.emplace("NotificationHelperSetNotificationsEnabledForSpecifiedBundle", []() {
        std::string param = GetStringParam();
        OHOS::Notification::NotificationHelper::SetNotificationsEnabledForSpecifiedBundle(
            *GetParamNotificationBundleOption(), param, GetBoolParam());
    });

    callFunctionMap_.emplace("NotificationHelperSetShowBadgeEnabledForBundle", []() {
        OHOS::Notification::NotificationHelper::SetShowBadgeEnabledForBundle(
            *GetParamNotificationBundleOption(), GetBoolParam());
    });

    callFunctionMap_.emplace("NotificationHelperGetShowBadgeEnabledForBundle", []() {
        bool param = GetBoolParam();
        OHOS::Notification::NotificationHelper::GetShowBadgeEnabledForBundle(
            *GetParamNotificationBundleOption(), param);
    });

    callFunctionMap_.emplace("NotificationHelperGetShowBadgeEnabled", []() {
        bool param = GetBoolParam();
        OHOS::Notification::NotificationHelper::GetShowBadgeEnabled(param);
    });

    callFunctionMap_.emplace("NotificationHelperSetDoNotDisturbDate",
        []() { OHOS::Notification::NotificationHelper::SetDoNotDisturbDate(*GetParamNotificationDoNotDisturbDate()); });

    callFunctionMap_.emplace("NotificationHelperGetDoNotDisturbDate", []() {
        OHOS::Notification::NotificationDoNotDisturbDate doNotDisturbDate;
        OHOS::Notification::NotificationHelper::GetDoNotDisturbDate(doNotDisturbDate);
    });
}

// RegisterNotificationSorting
void NotificationFuzzTestManager::RegisterNotificationSorting()
{
    callFunctionMap_.emplace("NotificationSortingGetRanking", []() {
        std::shared_ptr<OHOS::Notification::NotificationSorting> temp = GetParamNotificationSorting();
        temp->GetRanking();
    });

    callFunctionMap_.emplace("NotificationSortingGetKey", []() {
        std::shared_ptr<OHOS::Notification::NotificationSorting> temp = GetParamNotificationSorting();
        temp->GetKey();
    });

    callFunctionMap_.emplace("NotificationSortingGetImportance", []() {
        std::shared_ptr<OHOS::Notification::NotificationSorting> temp = GetParamNotificationSorting();
        temp->GetImportance();
    });

    callFunctionMap_.emplace("NotificationSortingGetSlot", []() {
        std::shared_ptr<OHOS::Notification::NotificationSorting> temp = GetParamNotificationSorting();
        temp->GetSlot();
    });

    callFunctionMap_.emplace("NotificationSortingGetVisiblenessOverride", []() {
        std::shared_ptr<OHOS::Notification::NotificationSorting> temp = GetParamNotificationSorting();
        temp->GetVisiblenessOverride();
    });

    callFunctionMap_.emplace("NotificationSortingIsDisplayBadge", []() {
        std::shared_ptr<OHOS::Notification::NotificationSorting> temp = GetParamNotificationSorting();
        temp->IsDisplayBadge();
    });

    callFunctionMap_.emplace("NotificationSortingIsHiddenNotification", []() {
        std::shared_ptr<OHOS::Notification::NotificationSorting> temp = GetParamNotificationSorting();
        temp->IsHiddenNotification();
    });

    callFunctionMap_.emplace("NotificationSortingGetGroupKeyOverride", []() {
        std::shared_ptr<OHOS::Notification::NotificationSorting> temp = GetParamNotificationSorting();
        temp->GetGroupKeyOverride();
    });
}

// RegisterNotificationSortingMap
void NotificationFuzzTestManager::RegisterNotificationSortingMap()
{
    callFunctionMap_.emplace("NotificationSortingMapGetKey", []() {
        std::shared_ptr<OHOS::Notification::NotificationSortingMap> temp = GetParamNotificationSortingMap();
        temp->GetKey();
    });

    callFunctionMap_.emplace("NotificationSortingMapGetNotificationSorting", []() {
        std::shared_ptr<OHOS::Notification::NotificationSortingMap> temp = GetParamNotificationSortingMap();
        const std::string key = GetStringParam();
        OHOS::Notification::NotificationSorting sorting = *(GetParamNotificationSorting().get());
        temp->GetNotificationSorting(key, sorting);
    });
}

// RegisterNotificationSubscribeInfo
void NotificationFuzzTestManager::RegisterNotificationSubscribeInfo()
{
    callFunctionMap_.emplace("NotificationSubscribeInfoAddAppName", []() {
        std::shared_ptr<OHOS::Notification::NotificationSubscribeInfo> temp = GetParamNotificationSubscribeInfo();
        std::string appName = GetStringParam();
        temp->AddAppName(appName);
    });

    callFunctionMap_.emplace("NotificationSubscribeInfoAddAppNames", []() {
        std::shared_ptr<OHOS::Notification::NotificationSubscribeInfo> temp = GetParamNotificationSubscribeInfo();
        const std::vector<std::string> appNames = GetStringVectorParam();
        temp->AddAppNames(appNames);
    });

    callFunctionMap_.emplace("NotificationSubscribeInfoGetAppNames", []() {
        std::shared_ptr<OHOS::Notification::NotificationSubscribeInfo> temp = GetParamNotificationSubscribeInfo();
        temp->GetAppNames();
    });
}

// RegisterNotificationSubscriber
void NotificationFuzzTestManager::RegisterNotificationSubscriber()
{
    callFunctionMap_.emplace("NotificationSubscriberOnCanceledNotification", []() {
        std::shared_ptr<OHOS::Notification::NotificationSubscriber> temp = GetParamNotificationSubscriber();
        temp->OnCanceled(GetParamNotification());
    });

    callFunctionMap_.emplace("NotificationSubscriberOnCanceledNotificationNotificationSortingMapdeleteReason", []() {
        std::shared_ptr<OHOS::Notification::NotificationSubscriber> temp = GetParamNotificationSubscriber();
        const std::shared_ptr<OHOS::Notification::Notification> request = GetParamNotification();
        const std::shared_ptr<OHOS::Notification::NotificationSortingMap> sortingMap = GetParamNotificationSortingMap();
        int deleteReason = GetIntParam();
        temp->OnCanceled(request, sortingMap, deleteReason);
    });

    callFunctionMap_.emplace("NotificationSubscriberOnSubscribeResult", []() {
        std::shared_ptr<OHOS::Notification::NotificationSubscriber> temp = GetParamNotificationSubscriber();
        temp->OnConnected();
    });

    callFunctionMap_.emplace("NotificationSubscriberOnConsumedNotification", []() {
        std::shared_ptr<OHOS::Notification::NotificationSubscriber> temp = GetParamNotificationSubscriber();
        const std::shared_ptr<OHOS::Notification::Notification> request = GetParamNotification();
        temp->OnConsumed(request);
    });

    callFunctionMap_.emplace("NotificationSubscriberOnConsumedNotificationNotificationSortingMap", []() {
        std::shared_ptr<OHOS::Notification::NotificationSubscriber> temp = GetParamNotificationSubscriber();
        std::shared_ptr<OHOS::Notification::Notification> request = GetParamNotification();
        std::shared_ptr<OHOS::Notification::NotificationSortingMap> sortingMap = GetParamNotificationSortingMap();
        temp->OnConsumed(request, sortingMap);
    });

    callFunctionMap_.emplace("NotificationSubscriberOnUnsubscribeResult", []() {
        std::shared_ptr<OHOS::Notification::NotificationSubscriber> temp = GetParamNotificationSubscriber();
        temp->OnDisconnected();
    });

    callFunctionMap_.emplace("NotificationSubscriberOnUpdate", []() {
        std::shared_ptr<OHOS::Notification::NotificationSubscriber> temp = GetParamNotificationSubscriber();
        const std::shared_ptr<OHOS::Notification::NotificationSortingMap> sortingMap = GetParamNotificationSortingMap();
        temp->OnUpdate(sortingMap);
    });

    callFunctionMap_.emplace("NotificationSubscriberOnDied", []() {
        std::shared_ptr<OHOS::Notification::NotificationSubscriber> temp = GetParamNotificationSubscriber();
        temp->OnDied();
    });

    callFunctionMap_.emplace("NotificationSubscriberOnDisturbModeChanged", []() {
        std::shared_ptr<OHOS::Notification::NotificationSubscriber> temp = GetParamNotificationSubscriber();
        temp->OnDoNotDisturbDateChange(GetParamNotificationDoNotDisturbDate());
    });
}

// RegisterWantAgentHelper
void NotificationFuzzTestManager::RegisterWantAgentHelper()
{
    callFunctionMap_.emplace("WantAgentHelperGetWantAgentAppExecFwkparamsInfo", []() {
        std::shared_ptr<AbilityRuntime::Context> context = OHOS::AbilityRuntime::Context::GetApplicationContext();
        const OHOS::AbilityRuntime::WantAgent::WantAgentInfo paramsInfo = *GetParamWantAgentInfo();
        OHOS::AbilityRuntime::AbilityRuntime::WantAgent::WantAgentHelper::GetWantAgent(context, paramsInfo);
    });

    callFunctionMap_.emplace("WantAgentHelperGetWantAgentparamsInfo", []() {
        const OHOS::AbilityRuntime::WantAgent::WantAgentInfo paramsInfo = *GetParamWantAgentInfo();
        OHOS::AbilityRuntime::AbilityRuntime::WantAgent::WantAgentHelper::GetWantAgent(paramsInfo);
    });

    callFunctionMap_.emplace("WantAgentHelperGetType", []() {
        const std::shared_ptr<OHOS::AbilityRuntime::AbilityRuntime::WantAgent::WantAgent> agent = GetParamWantAgent();
        OHOS::AbilityRuntime::AbilityRuntime::WantAgent::WantAgentHelper::GetType(agent);
    });

    callFunctionMap_.emplace("WantAgentHelperTriggerWantAgent", []() {
        const std::shared_ptr<OHOS::AbilityRuntime::AbilityRuntime::WantAgent::WantAgent> agent = GetParamWantAgent();
        const std::shared_ptr<OHOS::AbilityRuntime::WantAgent::CompletedCallback> callback =
            GetParamCompletedCallback();
        OHOS::AbilityRuntime::WantAgent::TriggerInfo paramsInfo(*GetParamTriggerInfo());
        OHOS::AbilityRuntime::AbilityRuntime::WantAgent::WantAgentHelper::TriggerWantAgent(agent, callback, paramsInfo);
    });

    callFunctionMap_.emplace("WantAgentHelperCancel", []() {
        const std::shared_ptr<OHOS::AbilityRuntime::AbilityRuntime::WantAgent::WantAgent> agent = GetParamWantAgent();
        OHOS::AbilityRuntime::AbilityRuntime::WantAgent::WantAgentHelper::Cancel(agent);
    });

    callFunctionMap_.emplace("WantAgentHelperJudgeEquality", []() {
        const std::shared_ptr<OHOS::AbilityRuntime::AbilityRuntime::WantAgent::WantAgent> agent = GetParamWantAgent();
        const std::shared_ptr<OHOS::AbilityRuntime::AbilityRuntime::WantAgent::WantAgent> otherAgent
            = GetParamWantAgent();
        OHOS::AbilityRuntime::AbilityRuntime::WantAgent::WantAgentHelper::JudgeEquality(agent, otherAgent);
    });
    callFunctionMap_.emplace("WantAgentHelperGetHashCode", []() {
        const std::shared_ptr<OHOS::AbilityRuntime::AbilityRuntime::WantAgent::WantAgent> agent = GetParamWantAgent();
        OHOS::AbilityRuntime::AbilityRuntime::WantAgent::WantAgentHelper::GetHashCode(agent);
    });

    callFunctionMap_.emplace("WantAgentHelperGetBundleName", []() {
        const std::shared_ptr<OHOS::AbilityRuntime::AbilityRuntime::WantAgent::WantAgent> agent = GetParamWantAgent();
        OHOS::AbilityRuntime::AbilityRuntime::WantAgent::WantAgentHelper::GetBundleName(agent);
    });

    callFunctionMap_.emplace("WantAgentHelperGetUid", []() {
        const std::shared_ptr<OHOS::AbilityRuntime::AbilityRuntime::WantAgent::WantAgent> agent = GetParamWantAgent();
        OHOS::AbilityRuntime::AbilityRuntime::WantAgent::WantAgentHelper::GetUid(agent);
    });

    callFunctionMap_.emplace("WantAgentHelperGetWant", []() {
        const std::shared_ptr<OHOS::AbilityRuntime::AbilityRuntime::WantAgent::WantAgent> agent = GetParamWantAgent();
        OHOS::AbilityRuntime::AbilityRuntime::WantAgent::WantAgentHelper::GetWant(agent);
    });

    callFunctionMap_.emplace("WantAgentHelperRegisterCancelListener", []() {
        const std::shared_ptr<OHOS::AbilityRuntime::WantAgent::CancelListener> cancelListener;
        const std::shared_ptr<OHOS::AbilityRuntime::AbilityRuntime::WantAgent::WantAgent> agent = GetParamWantAgent();
        OHOS::AbilityRuntime::AbilityRuntime::WantAgent::WantAgentHelper::
            RegisterCancelListener(cancelListener, agent);
    });

    callFunctionMap_.emplace("WantAgentHelperUnregisterCancelListener", []() {
        const std::shared_ptr<OHOS::AbilityRuntime::WantAgent::CancelListener> cancelListener
            = GetParamCancelListener();
        const std::shared_ptr<OHOS::AbilityRuntime::AbilityRuntime::WantAgent::WantAgent> agent = GetParamWantAgent();
        OHOS::AbilityRuntime::AbilityRuntime::WantAgent::WantAgentHelper::
            UnregisterCancelListener(cancelListener, agent);
    });
}

// RegisterLauncherService
void NotificationFuzzTestManager::RegisterLauncherService()
{
    callFunctionMap_.emplace("LauncherServiceGetAbilityList", []() {
        std::shared_ptr<OHOS::AppExecFwk::LauncherService> temp = GetParamLauncherService();
        const std::string bundleName = GetStringParam();
        const int userId = GetIntParam();
        std::vector<OHOS::AppExecFwk::LauncherAbilityInfo> launcherAbilityInfos = GetParamLauncherAbilityInfoVector();
        temp->GetAbilityList(bundleName, userId, launcherAbilityInfos);
    });

    callFunctionMap_.emplace("LauncherServiceGetAbilityInfo", []() {
        std::shared_ptr<OHOS::AppExecFwk::LauncherService> temp = GetParamLauncherService();
        const Want want = *GetParamWant();
        const int userI = GetIntParam();
        OHOS::AppExecFwk::LauncherAbilityInfo launcherAbilityInfo = *GetParamLauncherAbilityInfo();
        temp->GetAbilityInfo(want, userI, launcherAbilityInfo);
    });

    callFunctionMap_.emplace("LauncherServiceGetApplicationInfo", []() {
        std::shared_ptr<OHOS::AppExecFwk::LauncherService> temp = GetParamLauncherService();
        const std::string bundleName = GetStringParam();
        const ApplicationFlag flags = GetParamApplicationFlag();
        const int userId = GetIntParam();
        ApplicationInfo applicationInfo = GetParamApplicationInfo();
        temp->GetApplicationInfo(bundleName, flags, userId, applicationInfo);
    });

    callFunctionMap_.emplace("LauncherServiceIsBundleEnabled", []() {
        std::shared_ptr<OHOS::AppExecFwk::LauncherService> temp = GetParamLauncherService();
        const std::string bundleName = GetStringParam();
        temp->IsBundleEnabled(bundleName);
    });

    callFunctionMap_.emplace("LauncherServiceIsAbilityEnabled", []() {
        std::shared_ptr<OHOS::AppExecFwk::LauncherService> temp = GetParamLauncherService();
        const OHOS::AppExecFwk::AbilityInfo abilityInfo = GetParamAbilityInfo();
        temp->IsAbilityEnabled(abilityInfo);
    });

    callFunctionMap_.emplace("LauncherServiceRegisterCallback", []() {
        std::shared_ptr<OHOS::AppExecFwk::LauncherService> temp = GetParamLauncherService();
        const sptr<IBundleStatusCallback> callback = GetParamIBundleStatusCallback();
        temp->RegisterCallback(callback);
    });

    callFunctionMap_.emplace("LauncherServiceUnRegisterCallback", []() {
        std::shared_ptr<OHOS::AppExecFwk::LauncherService> temp = GetParamLauncherService();
        temp->UnRegisterCallback();
    });
}

// RegisterIAbilityContinuation
void NotificationFuzzTestManager::RegisterIAbilityContinuation()
{
    callFunctionMap_.emplace("IAbilityContinuationOnStartContinuation", []() {
        std::shared_ptr<OHOS::AppExecFwk::IAbilityContinuation> temp = GetParamAbility();
        temp->OnStartContinuation();
    });

    callFunctionMap_.emplace("IAbilityContinuationOnSaveData", []() {
        std::shared_ptr<OHOS::AppExecFwk::IAbilityContinuation> temp = GetParamAbility();
        WantParams saveData = *GetParamWantParams();
        temp->OnSaveData(saveData);
    });

    callFunctionMap_.emplace("IAbilityContinuationOnRestoreData", []() {
        std::shared_ptr<OHOS::AppExecFwk::IAbilityContinuation> temp = GetParamAbility();
        OHOS::AppExecFwk::WantParams restoreData = *GetParamWantParams();
        temp->OnRestoreData(restoreData);
    });

    callFunctionMap_.emplace("IAbilityContinuationOnCompleteContinuation", []() {
        std::shared_ptr<OHOS::AppExecFwk::IAbilityContinuation> temp = GetParamAbility();
        int result = GetIntParam();
        temp->OnCompleteContinuation(result);
    });
}

// RegisterAbility
void NotificationFuzzTestManager::RegisterAbility()
{
    callFunctionMap_.emplace("AbilityOnRequestPermissionsFromUserResult", []() {
        std::shared_ptr<OHOS::AppExecFwk::Ability> temp = GetParamAbility();
        int requestCode = GetIntParam();
        const std::vector<std::string> permissions = GetStringVectorParam();
        const std::vector<int> grantResults = GetIntVectorParam();
        temp->OnRequestPermissionsFromUserResult(requestCode, permissions, grantResults);
    });

    callFunctionMap_.emplace("AbilityStartAbility", []() {
        std::shared_ptr<OHOS::AppExecFwk::Ability> temp = GetParamAbility();
        const Want want = *GetParamWant();
        temp->StartAbility(want);
    });
}

// RegisterAbilityContext
void NotificationFuzzTestManager::RegisterAbilityContext()
{
    callFunctionMap_.emplace("AbilityStartAbility", []() {
        std::shared_ptr<OHOS::AppExecFwk::Ability> temp = GetParamAbility();
        const AAFwk::Want want = *GetParamWant();
        int requestCode = GetIntParam();
        temp->StartAbility(want, requestCode);
    });
}

// RegisterAbilityLifecycleCallbacks
void NotificationFuzzTestManager::RegisterAbilityLifecycleCallbacks()
{
    callFunctionMap_.emplace("AbilityLifecycleCallbacksOnAbilityActive", []() {
        std::shared_ptr<OHOS::AppExecFwk::AbilityLifecycleCallbacks> temp = GetParamAbilityLifecycleCallbacks();
        const std::shared_ptr<Ability> ability = GetParamAbility();
        temp->OnAbilityActive(ability);
    });
}

// RegisterIAbilityManager
void NotificationFuzzTestManager::RegisterIAbilityManager()
{
    callFunctionMap_.emplace("IAbilityManagerMoveMissionToEnd", []() {
        std::shared_ptr<OHOS::AAFwk::IAbilityManager> temp = GetParamIAbilityManager();
        const sptr<IRemoteObject> token = GetParamSptrRemote();
        const bool nonFirs = GetBoolParam();
        temp->MoveMissionToEnd(token, nonFirs);
    });

    callFunctionMap_.emplace("IAbilityManagerIsFirstInMission", []() {
        std::shared_ptr<OHOS::AAFwk::IAbilityManager> temp = GetParamIAbilityManager();
        const sptr<IRemoteObject> token = GetParamSptrRemote();
        temp->IsFirstInMission(token);
    });

    callFunctionMap_.emplace("IAbilityManagerPowerOn", []() {
        std::shared_ptr<OHOS::AAFwk::IAbilityManager> temp = GetParamIAbilityManager();
        temp->PowerOn();
    });

    callFunctionMap_.emplace("IAbilityManagerPowerOff", []() {
        std::shared_ptr<OHOS::AAFwk::IAbilityManager> temp = GetParamIAbilityManager();
        temp->PowerOff();
    });
}

NotificationFuzzTestManager::NotificationFuzzTestManager()
{
    RegisterNotificationHelper();
    RegisterNotificationSorting();
    RegisterNotificationSortingMap();
    RegisterNotificationSubscribeInfo();
    RegisterNotificationSubscriber();
    RegisterWantAgentHelper();
    RegisterLauncherService();
    RegisterIAbilityContinuation();
    RegisterAbility();
    RegisterAbilityContext();
    RegisterContext();
    RegisterAbilityLifecycleCallbacks();
    RegisterIAbilityManager();
}

void NotificationFuzzTestManager::SetJsonFunction(std::string functionName)
{
    remainderMap_.emplace(functionName, cycle_);
}

void NotificationFuzzTestManager::SetCycle(uint16_t cycle)
{
    cycle_ = cycle;
    for_each(remainderMap_.begin(), remainderMap_.end(), [cycle](std::unordered_map<std::string, int>::reference temp) {
        temp.second = cycle;
    });
}

int GetRandomInt(int minNum, int maxNum)
{
    return GetU16Param() % (maxNum - minNum + 1) + minNum;
}

void NotificationFuzzTestManager::StartFuzzTest()
{
    std::cout << __func__ << std::endl;
    OHOS::NotificationFuzzConfigParser jsonParser;
    OHOS::FuzzTestData tempData;

    std::cout << "parseFromFile start" << std::endl;
    jsonParser.ParseFromFile4FuzzTest(FUZZ_TEST_CONFIG_FILE_PATH, tempData);
    std::cout << "flag :" << tempData.mainLoopFlag << std::endl;
    for_each(tempData.methodVec.begin(), tempData.methodVec.end(), [this](std::vector<std::string>::reference s) {
        SetJsonFunction(s);
    });
    SetCycle(tempData.mainLoopFlag);

    std::vector<std::string> index;
    std::unordered_map<std::string, int>::iterator it = remainderMap_.begin();
    while (it != remainderMap_.end()) {
        if (it->second <= 0) {
            it = remainderMap_.erase(it);
        } else {
            index.push_back(it->first);
            ++it;
        }
    }

    std::cout << remainderMap_.size() << "--------fuzz test start1111--------" << callFunctionMap_.size() << std::endl;
    for (; remainderMap_.size() > 0;) {
        std::string functionName;
        int offset = GetRandomInt(0, index.size() - 1);
        functionName = index[offset];
        if (callFunctionMap_.find(functionName) != callFunctionMap_.end()) {
            std::cout << "call function : " << functionName << std::endl;
            callFunctionMap_[functionName]();
            std::cout << "function end  :" << functionName << std::endl;
        } else {
            std::cout << "can't find function : " << functionName << std::endl;
        }
        remainderMap_[functionName]--;
        if (remainderMap_[functionName] <= 0) {
            remainderMap_.erase(functionName);
            index.erase(index.begin() + offset);
        };
    }
    std::cout << remainderMap_.size() << "--------fuzz test end--------" << std::endl;
}
}  // namespace Notification
}  // namespace OHOS
