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
#include <mutex>
#include "ability.h"
#include "ability_handler.h"
#include "ability_manager.h"
#include "ability_manager_interface.h"
#include "async_common_event_result.h"
#include "bundle_info.h"
#include "common_event_manager.h"
#include "common_event_data.h"
#include "common_event_publish_info.h"
#include "common_event_subscribe_info.h"
#include "common_event_subscriber.h"
#include "common_event_support.h"
#include "data_uri_utils.h"
#include "element_name.h"
#include "event_handler.h"
#include "event_runner.h"
#include "inner_event.h"
#include "key_event.h"
#include "logger.h"
#include "matching_skills.h"
#include "module_info.h"
#include "native_implement_eventhandler.h"
#include "pac_map.h"
#include "parcel.h"
#include "patterns_matcher.h"
#include "uri.h"
#include "want.h"
#include "bundle_mgr_proxy.h"
#include "notification_sorting_map.h"
#include "notification_helper.h"
#include "want_agent_helper.h"
#include "notification.h"
#ifdef PRINT_LOG
#undef PRINT_LOG
#endif
#include "launcher_service.h"
#include "launcher_ability_info.h"
#include "task_dispatcher.h"
#include "base_task_dispatcher.h"
#include "spec_task_dispatcher.h"
#include "task_dispatcher_context.h"
#include "ability_manager_service.h"

using Want = OHOS::AAFwk::Want;
namespace OHOS {
namespace Notification {
class TestDumper;
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

std::shared_ptr<Parcel> GetParamParcel();
std::shared_ptr<Want> GetParamWant();
std::vector<std::shared_ptr<Want>> GetParamWantVector();
OHOS::AAFwk::Operation GetParamOperation();
std::shared_ptr<OHOS::EventFwk::AsyncCommonEventResult> GetParamAsyncCommonEventResult();
std::shared_ptr<OHOS::EventFwk::CommonEventData> GetParamCommonEventData();
std::shared_ptr<OHOS::EventFwk::CommonEventManager> GetParamCommonEventManager();
std::shared_ptr<OHOS::EventFwk::CommonEventPublishInfo> GetParamCommonEventPublishInfo();
std::shared_ptr<OHOS::EventFwk::CommonEventSubscribeInfo> GetParamCommonEventSubscribeInfo();
std::shared_ptr<OHOS::EventFwk::CommonEventSubscriber> GetParamCommonEventSubscriber();
std::shared_ptr<OHOS::EventFwk::CommonEventSupport> GetParamCommonEventSupport();
std::shared_ptr<OHOS::EventFwk::MatchingSkills> GetParamMatchingSkills();
sptr<IRemoteObject> GetParamSptrRemote();
std::shared_ptr<OHOS::AppExecFwk::EventRunner> GetParamEventRunner();
std::shared_ptr<OHOS::AppExecFwk::EventHandler> GetParamEventHandler();
std::shared_ptr<OHOS::AppExecFwk::EventQueue> GetParamEventQueue();
std::shared_ptr<EventRunnerNativeImplement> GetParamEventRunnerNativeImplement();
std::shared_ptr<OHOS::AppExecFwk::FileDescriptorListener> GetParamFileDescriptorListener();
std::shared_ptr<OHOS::AppExecFwk::Logger> GetParamLogger();
OHOS::AppExecFwk::EventQueue::Priority GetParamPriority();
TestDumper GetParamDumper();
OHOS::AppExecFwk::InnerEvent::Pointer GetParamInnerEvent();
OHOS::AppExecFwk::InnerEvent::Callback GetParamCallback();
OHOS::AppExecFwk::InnerEvent::TimePoint GetParamTimePoint();
OHOS::EventFwk::CommonEventSubscribeInfo::ThreadMode GetParamThreadMode();

std::shared_ptr<OHOS::AppExecFwk::AbilityContext> GetParamAbilityContext();
std::shared_ptr<OHOS::AppExecFwk::IAbilityEvent> GetParamIAbilityEvent();
sptr<OHOS::AppExecFwk::AbilityThread> GetParamAbilityThread();
std::shared_ptr<OHOS::AppExecFwk::AbilityHandler> GetParamAbilityHandler();
std::shared_ptr<OHOS::AppExecFwk::AbilityStartSetting> GetParamAbilityStartSetting();
std::shared_ptr<OHOS::AppExecFwk::Ability> GetParamAbility();
std::shared_ptr<OHOS::AppExecFwk::OHOSApplication> GetParamOHOSApplication();
std::shared_ptr<OHOS::KeyEvent> GetParamKeyEvent();
OHOS::Uri GetParamUri();

NativeRdb::ValuesBucket GetParamValuesBucket();
OHOS::AppExecFwk::Configuration GetParamConfiguration();
NativeRdb::DataAbilityPredicates GetParamDataAbilityPredicates();
OHOS::AppExecFwk::PacMap GetParamPacMap();
std::shared_ptr<OHOS::AppExecFwk::ComponentContainer> GetParamComponentContainer();
sptr<OHOS::AAFwk::IAbilityConnection> GetParamIAbilityConnection();

std::shared_ptr<OHOS::AppExecFwk::ProcessInfo> GetParamProcessInfo();
std::shared_ptr<OHOS::AppExecFwk::DataUriUtils> GetParamDataUriUtils();
std::shared_ptr<OHOS::AppExecFwk::DataAbilityHelper> GetParamDataAbilityHelper();
std::shared_ptr<OHOS::AppExecFwk::LifeCycle> GetParamLifeCycle();
OHOS::AppExecFwk::LifeCycle::Event GetParamLifeCycleEvent();
std::shared_ptr<OHOS::AppExecFwk::ElementName> GetParamElementName();
std::shared_ptr<OHOS::AAFwk::WantParams> GetParamWantParams();
std::shared_ptr<OHOS::AppExecFwk::AbilityManager> GetParamAbilityManager();
OHOS::AAFwk::PatternsMatcher GetParamPatternsMatcher();
OHOS::AAFwk::MatchType GetParamMatchType();
std::shared_ptr<OHOS::AppExecFwk::Context> GetParamContext();

std::shared_ptr<OHOS::AppExecFwk::BundleMgrProxy> GetParamBundleMgrProxy();
OHOS::AppExecFwk::ApplicationFlag GetParamApplicationFlag();
OHOS::AppExecFwk::ApplicationInfo GetParamApplicationInfo();
std::vector<OHOS::AppExecFwk::ApplicationInfo> GetParamApplicationInfoVector();
OHOS::AppExecFwk::BundleFlag GetParamBundleFlag();
OHOS::AppExecFwk::BundleInfo GetParamBundleInfo();
OHOS::AppExecFwk::AbilityInfo GetParamAbilityInfo();
std::vector<OHOS::AppExecFwk::BundleInfo> GetParamBundleInfoVector();
OHOS::AppExecFwk::HapModuleInfo GetParamHapModuleInfo();
OHOS::AppExecFwk::PermissionDef GetParamPermissionDef();
std::vector<OHOS::AppExecFwk::PermissionDef> GetParamPermissionDefVector();
OHOS::AppExecFwk::IBundleMgr::Message GetParamIBundleMgrMessage();
OHOS::MessageParcel GetParamMessageParcel();
OHOS::AppExecFwk::DumpFlag GetParamDumpFlag();
sptr<OHOS::AppExecFwk::ICleanCacheCallback> GetParamICleanCacheCallback();
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

std::shared_ptr<OHOS::Notification::WantAgent::WantAgentInfo> GetParamWantAgentInfo();
OHOS::Notification::WantAgent::WantAgentConstant::OperationType GetParamOperationType();
OHOS::Notification::WantAgent::WantAgentConstant::Flags GetParamFlags();
std::vector<OHOS::Notification::WantAgent::WantAgentConstant::Flags> GetParamFlagsVector();

std::shared_ptr<OHOS::Notification::Notification> GetParamNotification();
sptr<OHOS::Notification::Notification> GetParamNotificationSptr();
std::vector<sptr<OHOS::Notification::Notification>> GetParamNotificationSptrVector();
std::shared_ptr<OHOS::Notification::WantAgent::PendingWant> GetParamPendingWant();
std::shared_ptr<OHOS::Notification::WantAgent::WantAgent> GetParamWantAgent();
std::shared_ptr<OHOS::Notification::WantAgent::CompletedCallback> GetParamCompletedCallback();
std::shared_ptr<OHOS::Notification::WantAgent::TriggerInfo> GetParamTriggerInfo();
std::shared_ptr<OHOS::Notification::WantAgent::CancelListener> GetParamCancelListener();

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

class TestCommonEventSubscriber : public OHOS::EventFwk::CommonEventSubscriber {
public:
    TestCommonEventSubscriber()
    {}
    ~TestCommonEventSubscriber()
    {}
    virtual void OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data)
    {
        printf("Fuzz Test Reveive Event\n");
    }
};

class TestDumper : public OHOS::AppExecFwk::Dumper {
public:
    void Dump(const std::string &message)
    {
        return;
    }
    std::string GetTag()
    {
        return GetStringParam();
    }
};

class TestFileDescriptorListener : public OHOS::AppExecFwk::FileDescriptorListener {
public:
    TestFileDescriptorListener()
    {}
    ~TestFileDescriptorListener()
    {}
};

class TestIAbilityConnection : public OHOS::AAFwk::IAbilityConnection {
public:
    void OnAbilityConnectDone(
        const OHOS::AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode) override
    {}
    void OnAbilityDisconnectDone(const OHOS::AppExecFwk::ElementName &element, int resultCode) override
    {}
    virtual ~TestIAbilityConnection()
    {}
};

class TestAbilityEvent : public OHOS::AppExecFwk::IAbilityEvent {
public:
    virtual void OnBackPressed()
    {
        printf("Fuzz Test Back Pressed.");
    }
};

class TestLogger : public OHOS::AppExecFwk::Logger {
public:
    void Log(const std::string &line)
    {}
    virtual ~TestLogger()
    {}
};

class TestICleanCacheCallback : public OHOS::AppExecFwk::ICleanCacheCallback {
public:
    TestICleanCacheCallback()
    {}
    void OnCleanCacheFinished(bool succeeded) override
    {}
    virtual ~TestICleanCacheCallback()
    {}
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
        mutex_.unlock();
    }
    void OnDisconnected() override
    {
        std::cout << "TestAnsSubscriber OnDisconnected" << std::endl;
        mutex_.unlock();
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

private:
    static std::mutex mutex_;
};

class TestCompletedCallback : public OHOS::Notification::WantAgent::CompletedCallback {
    void OnSendFinished(const AAFwk::Want &want, int resultCode, const std::string &resultData,
        const AAFwk::WantParams &resultExtras) override
    {}
};

class TestCancelListener : public OHOS::Notification::WantAgent::CancelListener {
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