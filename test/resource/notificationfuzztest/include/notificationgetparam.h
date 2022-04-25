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
#ifndef NOTIFICATION_GET_PARAM_H
#define NOTIFICATION_GET_PARAM_H
#include <memory>
#include <mutex>
#include <cstdint>
#include <vector>

#include "ability.h"
#include "ability_handler.h"
#include "ability_manager.h"
#include "ability_manager_interface.h"
#include "ability_manager_service.h"
#include "data_uri_utils.h"
#include "element_name.h"
#include "event_runner.h"
#include "launcher_ability_info.h"
#include "launcher_service.h"
#include "uri.h"
#include "want.h"

#ifdef PRINT_LOG
#undef PRINT_LOG
#endif

#include "notification.h"
#include "notification_helper.h"
#include "notification_sorting_map.h"
#include "want_agent_helper.h"

using Want = OHOS::AAFwk::Want;
namespace OHOS {
namespace Notification {
bool GetBoolParam();
uint8_t GetU8Param();
unsigned int GetUIntParam();
uint16_t GetU16Param();
uint32_t GetU32Param();
uint64_t GetU64Param();
int8_t GetS8Param();
int16_t GetS16Param();
int32_t GetS32Param();
int64_t GetS64Param();
char32_t GetChar32Param();

short GetShortParam();
long GetLongParam();
int GetIntParam();
double GetDoubleParam();
float GetFloatParam();
char GetCharParam();
char *GetCharArryParam();
std::string GetStringParam();
std::vector<bool> GetBoolVectorParam();
std::vector<short> GetShortVectorParam();
std::vector<long> GetLongVectorParam();
std::vector<int> GetIntVectorParam();
std::vector<float> GetFloatVectorParam();
std::vector<double> GetDoubleVectorParam();
std::vector<char> GetCharVectorParam();
std::vector<char32_t> GetChar32VectorParam();
std::vector<std::string> GetStringVectorParam();
template <class T>
std::vector<T> GetUnsignVectorParam();
std::vector<int8_t> GetS8VectorParam();
std::vector<int16_t> GetS16VectorParam();
std::vector<int32_t> GetS32VectorParam();
std::vector<int64_t> GetS64VectorParam();
std::shared_ptr<Want> GetParamWant();
sptr<IRemoteObject> GetParamSptrRemote();
std::shared_ptr<OHOS::AppExecFwk::EventRunner> GetParamEventRunner();
std::shared_ptr<OHOS::AppExecFwk::Ability> GetParamAbility();
std::shared_ptr<OHOS::AAFwk::WantParams> GetParamWantParams();
OHOS::AppExecFwk::ApplicationFlag GetParamApplicationFlag();
OHOS::AppExecFwk::ApplicationInfo GetParamApplicationInfo();
OHOS::AppExecFwk::AbilityInfo GetParamAbilityInfo();
sptr<OHOS::AppExecFwk::IBundleStatusCallback> GetParamIBundleStatusCallback();
std::shared_ptr<OHOS::Notification::NotificationSorting> GetParamNotificationSorting();
std::vector<OHOS::Notification::NotificationSorting> GetParamNotificationSortingVector();
std::shared_ptr<OHOS::Notification::NotificationSortingMap> GetParamNotificationSortingMap();
std::shared_ptr<OHOS::Notification::NotificationSlot> GetParamNotificationSlot();
sptr<OHOS::Notification::NotificationSlot> GetParamNotificationSlotSptr();
OHOS::Notification::NotificationConstant::SlotType GetParamSlotType();
std::vector<sptr<OHOS::Notification::NotificationSlot>> GetParamNotificationSlotSptrVector();
std::vector<OHOS::Notification::NotificationSlot> GetParamNotificationSlotVector();
std::vector<OHOS::Notification::NotificationSlotGroup> GetParamNotificationSlotGroupVector();
sptr<OHOS::Notification::NotificationSlotGroup> GetParamNotificationSlotGroup();
std::vector<sptr<OHOS::Notification::NotificationSlotGroup>> GetParamNotificationSlotGroupSptrVector();
std::shared_ptr<OHOS::Notification::NotificationBundleOption> GetParamNotificationBundleOption();
std::shared_ptr<OHOS::Notification::NotificationRequest> GetParamNotificationRequest();
std::vector<sptr<OHOS::Notification::NotificationRequest>> GetParamNotificationRequestVector();
sptr<OHOS::Notification::NotificationSortingMap> GetParamNotificationSortingMapSptr();
OHOS::Notification::NotificationSlot::NotificationLevel GetParamNotificationLevel();
std::shared_ptr<OHOS::Notification::NotificationSubscriber> GetParamNotificationSubscriber();
std::shared_ptr<OHOS::Notification::NotificationSubscribeInfo> GetParamNotificationSubscribeInfo();

std::shared_ptr<OHOS::AbilityRuntime::WantAgent::WantAgentInfo> GetParamWantAgentInfo();
OHOS::AbilityRuntime::WantAgent::WantAgentConstant::OperationType GetParamOperationType();
OHOS::AbilityRuntime::WantAgent::WantAgentConstant::Flags GetParamFlags();
std::vector<OHOS::AbilityRuntime::WantAgent::WantAgentConstant::Flags> GetParamFlagsVector();

std::shared_ptr<OHOS::Notification::Notification> GetParamNotification();
sptr<OHOS::Notification::Notification> GetParamNotificationSptr();
std::vector<sptr<OHOS::Notification::Notification>> GetParamNotificationSptrVector();
std::shared_ptr<OHOS::AbilityRuntime::WantAgent::PendingWant> GetParamPendingWant();
std::shared_ptr<OHOS::AbilityRuntime::AbilityRuntime::WantAgent::WantAgent> GetParamWantAgent();
std::shared_ptr<OHOS::AbilityRuntime::WantAgent::CompletedCallback> GetParamCompletedCallback();
std::shared_ptr<OHOS::AbilityRuntime::WantAgent::TriggerInfo> GetParamTriggerInfo();
std::shared_ptr<OHOS::AbilityRuntime::WantAgent::CancelListener> GetParamCancelListener();

std::shared_ptr<OHOS::AppExecFwk::LauncherService> GetParamLauncherService();
std::vector<OHOS::AppExecFwk::LauncherAbilityInfo> GetParamLauncherAbilityInfoVector();
std::shared_ptr<OHOS::AppExecFwk::LauncherAbilityInfo> GetParamLauncherAbilityInfo();
std::vector<OHOS::AppExecFwk::LauncherShortcutInfo> GetParamLauncherShortcutInfoVector();
std::vector<OHOS::AppExecFwk::FormInfo> GetParamFormInfoVector();
std::vector<OHOS::AppExecFwk::ShortcutInfo> GetParamShortcutInfoVector();
std::vector<OHOS::AppExecFwk::ModuleUsageRecord> GetParamModuleUsageRecordVector();
sptr<OHOS::AppExecFwk::OnPermissionChangedCallback> GetParamOnPermissionChangedCallback();

std::shared_ptr<OHOS::AppExecFwk::TaskDispatcher> GetParamTaskDispatcher();
std::shared_ptr<OHOS::AppExecFwk::BaseTaskDispatcher> GetParamBaseTaskDispatcher();
OHOS::AppExecFwk::TaskPriority GetParamTaskPriority();
std::shared_ptr<OHOS::AppExecFwk::Runnable> GetParamRunnable();
std::shared_ptr<OHOS::AppExecFwk::Group> GetParamGroup();
std::shared_ptr<OHOS::AppExecFwk::IteratableTask<long>> GetParamIteratableTask();
OHOS::Notification::NotificationConstant::SubscribeResult GetParamSubscribeResult();
OHOS::AppExecFwk::MissionInformation GetParamMissionInformation();
std::shared_ptr<OHOS::AppExecFwk::AbilityLifecycleCallbacks> GetParamAbilityLifecycleCallbacks();
std::shared_ptr<OHOS::AAFwk::IAbilityManager> GetParamIAbilityManager();
std::shared_ptr<OHOS::Notification::NotificationDoNotDisturbDate> GetParamNotificationDoNotDisturbDate();
OHOS::Notification::NotificationConstant::DoNotDisturbType GetParamDoNotDisturbType();
class TestRemoteObject : public IRemoteObject {
public:
    TestRemoteObject();
    ~TestRemoteObject();

    int32_t GetObjectRefCount() override
    {
        return 0;
    }

    int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override
    {
        return 0;
    }

    bool IsProxyObject() const override
    {
        return true;
    }

    bool CheckObjectLegality() const override
    {
        return true;
    }

    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient) override
    {
        return true;
    }

    bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient) override
    {
        return true;
    }

    bool Marshalling(Parcel &parcel) const override
    {
        return true;
    }

    sptr<IRemoteBroker> AsInterface() override
    {
        return nullptr;
    }

    int Dump(int fd, const std::vector<std::u16string> &args) override
    {
        return 0;
    }

    std::u16string GetObjectDescriptor() const
    {
        std::u16string descriptor = std::u16string();
        return descriptor;
    }
};

class TestIBundleStatusCallback : public OHOS::AppExecFwk::IBundleStatusCallback {
public:
    TestIBundleStatusCallback()
    {}
    void OnBundleStateChanged(const uint8_t installType, const int32_t resultCode, const std::string &resultMsg,
        const std::string &bundleName) override
    {}
    virtual ~TestIBundleStatusCallback()
    {}
};

class TestAnsSubscriber : public OHOS::Notification::NotificationSubscriber {
public:
    TestAnsSubscriber()
    {}
    ~TestAnsSubscriber()
    {}
    void OnConnected() override
    {
        std::cout << "TestAnsSubscriber OnConnected" << std::endl;
        mutex.unlock();
    }
    void OnDisconnected() override
    {
        std::cout << "TestAnsSubscriber OnDisconnected" << std::endl;
        mutex.unlock();
    }
    void OnDied() override
    {}
    void OnUpdate(const std::shared_ptr<OHOS::Notification::NotificationSortingMap> &sortingMap) override
    {}
    void OnDoNotDisturbDateChange(const std::shared_ptr<NotificationDoNotDisturbDate> &date) override
    {}
    void OnCanceled(const std::shared_ptr<OHOS::Notification::Notification> &request) override
    {}
    void OnCanceled(const std::shared_ptr<OHOS::Notification::Notification> &request,
        const std::shared_ptr<OHOS::Notification::NotificationSortingMap> &sortingMap, int deleteReason) override
    {}
    void OnConsumed(const std::shared_ptr<OHOS::Notification::Notification> &request) override
    {}
    void OnConsumed(const std::shared_ptr<OHOS::Notification::Notification> &request,
        const std::shared_ptr<OHOS::Notification::NotificationSortingMap> &sortingMap) override
    {}

public:
    static std::mutex mutex;
};

class TestCompletedCallback : public OHOS::AbilityRuntime::WantAgent::CompletedCallback {
    void OnSendFinished(const AAFwk::Want &want, int resultCode, const std::string &resultData,
        const AAFwk::WantParams &resultExtras) override
    {}
};

class TestCancelListener : public OHOS::AbilityRuntime::WantAgent::CancelListener {
public:
    void OnCancelled(int resultCode) override
    {}
};

class TestOnPermissionChangedCallback : public OHOS::AppExecFwk::OnPermissionChangedCallback {
public:
    void OnChanged(const int32_t uid) override
    {}
};

class TestAbilityLifecycleCallbacks : public OHOS::AppExecFwk::AbilityLifecycleCallbacks {
public:
    TestAbilityLifecycleCallbacks() = default;
    virtual ~TestAbilityLifecycleCallbacks() = default;
    virtual void OnAbilityStart(const std::shared_ptr<Ability> &ability)
    {}
    virtual void OnAbilityInactive(const std::shared_ptr<Ability> &ability)
    {}
    virtual void OnAbilityBackground(const std::shared_ptr<Ability> &ability)
    {}
    virtual void OnAbilityForeground(const std::shared_ptr<Ability> &ability)
    {}
    virtual void OnAbilityActive(const std::shared_ptr<Ability> &ability)
    {}
    virtual void OnAbilityStop(const std::shared_ptr<Ability> &ability)
    {}
    virtual void OnAbilitySaveState(const OHOS::AppExecFwk::PacMap &outState)
    {}
};
class WantSender : public OHOS::AAFwk::WantSenderStub {
public:
    void Send(OHOS::AAFwk::SenderInfo &senderInfo) override
    {}
};
}  // namespace Notification
}  // namespace OHOS
#endif
