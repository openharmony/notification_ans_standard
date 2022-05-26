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
#include <chrono>
#include <condition_variable>
#include <functional>
#include <gtest/gtest.h>
#include <list>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "advanced_notification_service.h"
#include "ans_const_define.h"
#include "ans_inner_errors.h"
#include "ans_manager_proxy.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "datetime_ex.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "mock_ipc_skeleton.h"
#include "mock_single_kv_store.h"
#include "notification_content.h"
#include "notification_helper.h"
#include "notification_long_text_content.h"
#include "notification_subscriber.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace AppExecFwk {
void MockSetDistributedNotificationEnabled(bool enable);
}  // AppExecFwk
}  // namespace OHOS

using namespace testing::ext;
using namespace OHOS::Media;

namespace OHOS {
namespace Notification {
namespace {
const std::string APP_NAME = "bundleName";
const std::string NOTIFICATION_LABEL_0 = "Label0";
const std::string NOTIFICATION_LABEL_1 = "Label1";
const std::string NOTIFICATION_LABEL_2 = "Label2";
const std::string AN_NOT_EXIST_KEY = "AN_NOT_EXIST_KEY";
const std::string KEY_SPLITER = "_";

const std::string KVSTORE_APP_ID = "advanced_notification_service";
const std::string KVSTORE_NOTIFICATION_STORE_ID = "distributed_notification";
const std::string KVSTORE_PREFERENCES_STORE_ID = "distributed_preferences";
const std::string KVSTORE_SCREEN_STATUS_STORE_ID = "distributed_screen_status";

constexpr int UID = 1;
constexpr int USER_ID = 0;
constexpr int CANCEL_REASON_DELETE = 2;
constexpr int APP_CANCEL_REASON_DELETE = 8;
constexpr int APP_CANCEL_ALL_REASON_DELETE = 9;
}  // namespace

enum class SubscriberEventType {
    ON_SUBSCRIBERESULT,
    ON_UNSUBSCRIBERESULT,
    ON_DIED,
    ON_UPDATE,
    ON_DND_CHANGED,
    ON_ENABLED_NOTIFICATION_CHANGED,
    ON_CANCELED,
    ON_CANCELED_WITH_SORTINGMAP_AND_DELETEREASON,
    ON_CONSUMED,
    ON_CONSUMED_WITH_SORTINGMAP,
    UNKNOWN,
};

class SubscriberEvent {
public:
    virtual ~SubscriberEvent()
    {}
    SubscriberEventType GetType()
    {
        return type_;
    }

protected:
    explicit SubscriberEvent(SubscriberEventType type) : type_(type)
    {}

    SubscriberEventType type_;
};

class OnSubscribeResultEvent : public SubscriberEvent {
public:
    ~OnSubscribeResultEvent() override
    {}

    OnSubscribeResultEvent() : SubscriberEvent(SubscriberEventType::ON_SUBSCRIBERESULT)
    {}
};

class OnUnSubscribeResultEvent : public SubscriberEvent {
public:
    OnUnSubscribeResultEvent() : SubscriberEvent(SubscriberEventType::ON_UNSUBSCRIBERESULT)
    {}

    ~OnUnSubscribeResultEvent() override
    {}
};

class OnDiedEvent : public SubscriberEvent {
public:
    OnDiedEvent() : SubscriberEvent(SubscriberEventType::ON_DIED)
    {}

    ~OnDiedEvent() override
    {}
};

class OnUpdatedEvent : public SubscriberEvent {
public:
    explicit OnUpdatedEvent(const std::shared_ptr<NotificationSortingMap> &sortingMap)
        : SubscriberEvent(SubscriberEventType::ON_UPDATE), sortingMap_(sortingMap)
    {}

    ~OnUpdatedEvent() override
    {}

    std::shared_ptr<NotificationSortingMap> GetNotificationSortingMap()
    {
        return sortingMap_;
    }

private:
    std::shared_ptr<NotificationSortingMap> sortingMap_;
};

class OnDoNotDisturbDateChangedEvent : public SubscriberEvent {
public:
    explicit OnDoNotDisturbDateChangedEvent(const std::shared_ptr<NotificationDoNotDisturbDate> &date)
        : SubscriberEvent(SubscriberEventType::ON_DND_CHANGED), date_(date)
    {}

    ~OnDoNotDisturbDateChangedEvent() override
    {}

    const std::shared_ptr<NotificationDoNotDisturbDate> &GetDoNotDisturbDate() const
    {
        return date_;
    }

private:
    std::shared_ptr<NotificationDoNotDisturbDate> date_;
};

class OnEnabledNotificationChangedEvent : public SubscriberEvent {
public:
    explicit OnEnabledNotificationChangedEvent(const std::shared_ptr<EnabledNotificationCallbackData> &callbackData)
        : SubscriberEvent(SubscriberEventType::ON_ENABLED_NOTIFICATION_CHANGED), callbackData_(callbackData)
    {}

    ~OnEnabledNotificationChangedEvent() override
    {}

    const std::shared_ptr<EnabledNotificationCallbackData> &GetEnabledNotificationCallbackData() const
    {
        return callbackData_;
    }

private:
    std::shared_ptr<EnabledNotificationCallbackData> callbackData_;
};

class OnOnCanceledEvent : public SubscriberEvent {
public:
    explicit OnOnCanceledEvent(const std::shared_ptr<Notification> &request)
        : SubscriberEvent(SubscriberEventType::ON_CANCELED), request_(request)
    {}

    ~OnOnCanceledEvent() override
    {}

    std::shared_ptr<Notification> GetRequest()
    {
        return request_;
    }

private:
    std::shared_ptr<Notification> request_;
};

class OnOnCanceledWithSortingMapAndDeleteReasonEvent : public SubscriberEvent {
public:
    explicit OnOnCanceledWithSortingMapAndDeleteReasonEvent(const std::shared_ptr<Notification> &request,
        const std::shared_ptr<NotificationSortingMap> &sortingMap, int deleteReason)
        : SubscriberEvent(SubscriberEventType::ON_CANCELED_WITH_SORTINGMAP_AND_DELETEREASON),
          request_(request),
          sortingMap_(sortingMap),
          deleteReason_(deleteReason)
    {}

    ~OnOnCanceledWithSortingMapAndDeleteReasonEvent() override
    {}

    std::shared_ptr<Notification> GetRequest()
    {
        return request_;
    }
    std::shared_ptr<NotificationSortingMap> GetSortingMap()
    {
        return sortingMap_;
    }
    int GetDeleteReason()
    {
        return deleteReason_;
    }

private:
    std::shared_ptr<Notification> request_;
    std::shared_ptr<NotificationSortingMap> sortingMap_;
    int deleteReason_;
};

class OnConsumedEvent : public SubscriberEvent {
public:
    explicit OnConsumedEvent(const std::shared_ptr<Notification> &request)
        : SubscriberEvent(SubscriberEventType::ON_CONSUMED), request_(request)
    {}

    ~OnConsumedEvent() override
    {}

    std::shared_ptr<Notification> GetRequest()
    {
        return request_;
    }

private:
    std::shared_ptr<Notification> request_;
};

class OnConsumedWithSortingMapEvent : public SubscriberEvent {
public:
    explicit OnConsumedWithSortingMapEvent(
        const std::shared_ptr<Notification> &request, const std::shared_ptr<NotificationSortingMap> &sortingMap)
        : SubscriberEvent(SubscriberEventType::ON_CONSUMED_WITH_SORTINGMAP), request_(request), sortingMap_(sortingMap)
    {
        type_ = SubscriberEventType::ON_CONSUMED_WITH_SORTINGMAP;
    }

    ~OnConsumedWithSortingMapEvent() override
    {}

    std::shared_ptr<Notification> GetRequest()
    {
        return request_;
    }

    std::shared_ptr<NotificationSortingMap> GetSortingMap()
    {
        return sortingMap_;
    }

private:
    std::shared_ptr<Notification> request_;
    std::shared_ptr<NotificationSortingMap> sortingMap_;
};

class TestAnsSubscriber : public NotificationSubscriber {
public:
    void OnConnected() override
    {
        std::shared_ptr<OnSubscribeResultEvent> event = std::make_shared<OnSubscribeResultEvent>();
        std::unique_lock<std::mutex> lck(mtx_);
        events_.push_back(event);
    }
    void OnDisconnected() override
    {
        std::shared_ptr<OnUnSubscribeResultEvent> event = std::make_shared<OnUnSubscribeResultEvent>();
        std::unique_lock<std::mutex> lck(mtx_);
        events_.push_back(event);
    }
    void OnDied() override
    {}
    void OnUpdate(const std::shared_ptr<NotificationSortingMap> &sortingMap) override
    {
        std::shared_ptr<OnUpdatedEvent> event = std::make_shared<OnUpdatedEvent>(sortingMap);
        std::unique_lock<std::mutex> lck(mtx_);
        events_.push_back(event);
    }
    void OnDoNotDisturbDateChange(const std::shared_ptr<NotificationDoNotDisturbDate> &date) override
    {
        std::shared_ptr<OnDoNotDisturbDateChangedEvent> event = std::make_shared<OnDoNotDisturbDateChangedEvent>(date);
        std::unique_lock<std::mutex> lck(mtx_);
        events_.push_back(event);
    }
    void OnEnabledNotificationChanged(
        const std::shared_ptr<EnabledNotificationCallbackData> &callbackData) override
    {
        std::shared_ptr<OnEnabledNotificationChangedEvent> event =
            std::make_shared<OnEnabledNotificationChangedEvent>(callbackData);
        std::unique_lock<std::mutex> lck(mtx_);
        events_.push_back(event);
    }
    void OnCanceled(const std::shared_ptr<Notification> &request) override
    {
        std::shared_ptr<OnOnCanceledEvent> event = std::make_shared<OnOnCanceledEvent>(request);
        std::unique_lock<std::mutex> lck(mtx_);
        events_.push_back(event);
    }
    void OnCanceled(const std::shared_ptr<Notification> &request,
        const std::shared_ptr<NotificationSortingMap> &sortingMap, int deleteReason) override
    {
        std::shared_ptr<OnOnCanceledWithSortingMapAndDeleteReasonEvent> event =
            std::make_shared<OnOnCanceledWithSortingMapAndDeleteReasonEvent>(request, sortingMap, deleteReason);
        std::unique_lock<std::mutex> lck(mtx_);
        events_.push_back(event);
    }
    void OnConsumed(const std::shared_ptr<Notification> &request) override
    {
        std::shared_ptr<OnConsumedEvent> event = std::make_shared<OnConsumedEvent>(request);
        std::unique_lock<std::mutex> lck(mtx_);
        events_.push_back(event);
    }

    void OnConsumed(const std::shared_ptr<Notification> &request,
        const std::shared_ptr<NotificationSortingMap> &sortingMap) override
    {
        std::shared_ptr<OnConsumedWithSortingMapEvent> event =
            std::make_shared<OnConsumedWithSortingMapEvent>(request, sortingMap);
        std::unique_lock<std::mutex> lck(mtx_);
        events_.push_back(event);
    }

    std::list<std::shared_ptr<SubscriberEvent>> GetEvents()
    {
        std::unique_lock<std::mutex> lck(mtx_);
        return events_;
    }

    void ClearEvents()
    {
        std::unique_lock<std::mutex> lck(mtx_);
        events_.clear();
    }

private:
    std::mutex mtx_;
    std::list<std::shared_ptr<SubscriberEvent>> events_;
};

class AnsFWModuleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    inline void SleepForFC()
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    const std::string DELIMITER = "|";
    const std::string LOCAL_DEVICE_ID = "<localDeviceId>";
    const std::string REMOTE_DEVICE_ID = "<remoteDeviceId>";
    inline std::string GenerateDistributedKey(const NotificationRequest &req, const std::string &deviceId)
    {
        return std::string()
            .append(deviceId)
            .append(DELIMITER)
            .append(APP_NAME)
            .append(DELIMITER)
            .append(req.GetLabel())
            .append(DELIMITER)
            .append(ToString(req.GetNotificationId()));
    }

    bool GetRequestInDistributedEntryList(
        NotificationRequest &req, std::vector<DistributedKv::Entry> &entries, DistributedKv::Entry &outEntry)
    {
        std::string localDistributedKey = GenerateDistributedKey(req, LOCAL_DEVICE_ID);
        for (auto entry : entries) {
            if (entry.key.ToString() == localDistributedKey) {
                outEntry = entry;
                return true;
            }
        }
        return false;
    }

    bool GetRequestInNotificationList(NotificationRequest &req,
        std::vector<std::shared_ptr<Notification>> &notificationList, std::shared_ptr<Notification> &outNotification)
    {
        for (auto notification : notificationList) {
            if (notification->GetNotificationRequest().GetNotificationId() == req.GetNotificationId() &&
                notification->GetNotificationRequest().GetLabel() == req.GetLabel()) {
                outNotification = notification;
                return true;
            }
        }
        return false;
    }

    NotificationRequest CreateDistributedRequest(std::string label)
    {
        int32_t notificationId = 1;
        auto normalContent = std::make_shared<NotificationNormalContent>();
        auto content = std::make_shared<NotificationContent>(normalContent);
        NotificationRequest request(notificationId);
        request.SetLabel(label);
        request.SetContent(content);
        request.SetDistributed(true);
        std::vector<std::string> devices = {"<localDeviceType>"};
        request.SetDevicesSupportDisplay(devices);
        return request;
    }

    void PublishCommonEventScreenStatus(bool isScreenOn)
    {
        EventFwk::Want want;
        EventFwk::CommonEventData data;
        if (isScreenOn) {
            data.SetWant(want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_ON));
        } else {
            data.SetWant(want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF));
        }

        EventFwk::CommonEventManager::PublishCommonEvent(data);
    }

    void SetDistributedScreenStatus(bool isScreenOn)
    {
        DistributedKv::AppId appId = {.appId = KVSTORE_APP_ID};
        DistributedKv::StoreId storeId = {.storeId = KVSTORE_SCREEN_STATUS_STORE_ID};
        std::shared_ptr<DistributedKv::MockSingleKvStore> pointer =
            DistributedKv::MockSingleKvStore::GetMockKvStorePointer(appId, storeId);
        if (pointer) {
            DistributedKv::Key key("<remoteDeviceId>" + DELIMITER + "screen_status");
            DistributedKv::Value value(isScreenOn ? "on" : "off");
            pointer->Put(key, value);
        }
    }
};

class EventParser {
public:
    EventParser()
    {
        waitOnSubscriber_ = 0;
        waitOnUnSubscriber_ = 0;
        waitOnConsumed_ = false;
        onConsumedReq_.clear();
        onConsumedWithSortingMapReq_.clear();
        onConsumedWithSortingMapSor_.clear();
        waitOnConsumedWithSortingMap_ = false;
        waitOnCanceled_ = false;
        onCanceledReq_.clear();
        onCanceledWithSortingMapReq_.clear();
        onCanceledWithSortingMapSor_.clear();
        onCanceledWithSortingMapDelRea_.clear();
        waitOnCanceledWithSortingMapAndDeleteReason_ = false;
    }

    ~EventParser()
    {}

    void Parse(std::list<std::shared_ptr<SubscriberEvent>> events)
    {
        GTEST_LOG_(INFO) << "TestAnsSubscriber::Parse event size=" << events.size();
        for (auto event : events) {
            GTEST_LOG_(INFO) << "TestAnsSubscriber::Parse event type=" << static_cast<int>(event->GetType());
            if (event->GetType() == SubscriberEventType::ON_SUBSCRIBERESULT) {
                waitOnSubscriber_ = true;
            } else if (event->GetType() == SubscriberEventType::ON_CONSUMED) {
                std::shared_ptr<OnConsumedEvent> ev = std::static_pointer_cast<OnConsumedEvent>(event);
                waitOnConsumed_ = true;
                onConsumedReq_.push_back(ev->GetRequest());
            } else if (event->GetType() == SubscriberEventType::ON_CONSUMED_WITH_SORTINGMAP) {
                std::shared_ptr<OnConsumedWithSortingMapEvent> ev =
                    std::static_pointer_cast<OnConsumedWithSortingMapEvent>(event);
                waitOnConsumedWithSortingMap_ = true;
                onConsumedWithSortingMapReq_.push_back(ev->GetRequest());
                onConsumedWithSortingMapSor_.push_back(ev->GetSortingMap());
            } else if (event->GetType() == SubscriberEventType::ON_CANCELED) {
                std::shared_ptr<OnOnCanceledEvent> ev = std::static_pointer_cast<OnOnCanceledEvent>(event);
                waitOnCanceled_ = true;
                onCanceledReq_.push_back(ev->GetRequest());
            } else if (event->GetType() == SubscriberEventType::ON_CANCELED_WITH_SORTINGMAP_AND_DELETEREASON) {
                std::shared_ptr<OnOnCanceledWithSortingMapAndDeleteReasonEvent> ev =
                    std::static_pointer_cast<OnOnCanceledWithSortingMapAndDeleteReasonEvent>(event);
                waitOnCanceledWithSortingMapAndDeleteReason_ = true;
                onCanceledWithSortingMapReq_.push_back(ev->GetRequest());
                onCanceledWithSortingMapSor_.push_back(ev->GetSortingMap());
                onCanceledWithSortingMapDelRea_.push_back(ev->GetDeleteReason());
            } else if (event->GetType() == SubscriberEventType::ON_UNSUBSCRIBERESULT) {
                waitOnUnSubscriber_ = true;
            }
        }
    }

    void SetWaitOnConsumed(bool bl)
    {
        waitOnConsumed_ = bl;
    }

    void SetWaitOnCanceled(bool bl)
    {
        waitOnCanceled_ = bl;
    }

    void SetWaitOnCanceledWithSortingMapAndDeleteReason(bool bl)
    {
        waitOnCanceledWithSortingMapAndDeleteReason_ = bl;
    }

    void SetWaitOnUnSubscriber()
    {
        waitOnUnSubscriber_ = NotificationConstant::SubscribeResult::RESOURCES_FAIL;
    }

    bool GetWaitOnSubscriber() const
    {
        return waitOnSubscriber_;
    }

    bool GetWaitOnUnSubscriber() const
    {
        return waitOnUnSubscriber_;
    }

    bool GetWaitOnConsumed() const
    {
        return waitOnConsumed_;
    }

    std::vector<std::shared_ptr<Notification>> GetOnConsumedReq() const
    {
        return onConsumedReq_;
    }

    std::vector<std::shared_ptr<Notification>> GetOnConsumedWithSortingMapReq() const
    {
        return onConsumedWithSortingMapReq_;
    }

    std::vector<std::shared_ptr<NotificationSortingMap>> GetOnConsumedWithSortingMapSor() const
    {
        return onConsumedWithSortingMapSor_;
    }

    bool GetWaitOnConsumedWithSortingMap() const
    {
        return waitOnConsumedWithSortingMap_;
    }

    bool GetWaitOnCanceled() const
    {
        return waitOnCanceled_;
    }

    bool GetWaitOnCanceledWithSortingMapAndDeleteReason() const
    {
        return waitOnCanceledWithSortingMapAndDeleteReason_;
    }

    std::vector<std::shared_ptr<Notification>> GetOnCanceledReq() const
    {
        return onCanceledReq_;
    }

    std::vector<std::shared_ptr<Notification>> GetOnCanceledWithSortingMapReq() const
    {
        return onCanceledWithSortingMapReq_;
    }

    std::vector<std::shared_ptr<NotificationSortingMap>> GetOnCanceledWithSortingMapSor() const
    {
        return onCanceledWithSortingMapSor_;
    }

    std::vector<int> GetOnCanceledWithSortingMapDelRea() const
    {
        return onCanceledWithSortingMapDelRea_;
    }

private:
    bool waitOnSubscriber_ = false;
    bool waitOnUnSubscriber_ = false;
    bool waitOnConsumed_ = false;
    std::vector<std::shared_ptr<Notification>> onConsumedReq_;
    std::vector<std::shared_ptr<Notification>> onConsumedWithSortingMapReq_;
    std::vector<std::shared_ptr<NotificationSortingMap>> onConsumedWithSortingMapSor_;
    bool waitOnConsumedWithSortingMap_ = false;
    bool waitOnCanceled_ = false;
    std::vector<std::shared_ptr<Notification>> onCanceledReq_;
    std::vector<std::shared_ptr<Notification>> onCanceledWithSortingMapReq_;
    std::vector<std::shared_ptr<NotificationSortingMap>> onCanceledWithSortingMapSor_;
    std::vector<int> onCanceledWithSortingMapDelRea_;
    bool waitOnCanceledWithSortingMapAndDeleteReason_ = false;
};

sptr<ISystemAbilityManager> g_systemAbilityManager =
    SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
void AnsFWModuleTest::SetUpTestCase()
{
    sptr<AdvancedNotificationService> service = OHOS::Notification::AdvancedNotificationService::GetInstance();
    OHOS::ISystemAbilityManager::SAExtraProp saExtraProp;
    g_systemAbilityManager->AddSystemAbility(OHOS::ADVANCED_NOTIFICATION_SERVICE_ABILITY_ID, service, saExtraProp);
}

void AnsFWModuleTest::TearDownTestCase()
{}

void AnsFWModuleTest::SetUp()
{}

void AnsFWModuleTest::TearDown()
{
    NotificationHelper::CancelAllNotifications();
    NotificationHelper::RemoveAllSlots();
    NotificationHelper::RemoveNotifications();
    std::vector<sptr<NotificationSlotGroup>> groups;
    NotificationHelper::GetNotificationSlotGroups(groups);
    for (auto group : groups) {
        NotificationHelper::RemoveNotificationSlotGroup(group->GetId().c_str());
    }
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_FlowControl_00100
 * @tc.name      : FlowControl_00100
 * @tc.desc      : Test notification's flow control.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_FlowControl_00100, Function | MediumTest | Level1)
{
    TestAnsSubscriber subscriber;
    NotificationSubscribeInfo info;
    info.AddAppName(APP_NAME);
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber, info), ERR_OK);
    std::shared_ptr<NotificationNormalContent> implContent = std::make_shared<NotificationNormalContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);
    for (uint32_t i = 0; i <= MAX_ACTIVE_NUM_PERSECOND; i++) {
        std::string label = std::to_string(i);
        NotificationRequest req(i);
        req.SetLabel(label);
        req.SetContent(content);
        if (i < MAX_ACTIVE_NUM_PERSECOND) {
            EXPECT_EQ(NotificationHelper::PublishNotification(req), ERR_OK);
        } else {
            EXPECT_EQ(NotificationHelper::PublishNotification(req), (int)ERR_ANS_OVER_MAX_ACTIVE_PERSECOND);
        }
    }
    SleepForFC();
    EXPECT_EQ(NotificationHelper::RemoveNotifications(), ERR_OK);
    SleepForFC();
    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber, info), ERR_OK);
    SleepForFC();
    std::list<std::shared_ptr<SubscriberEvent>> events = subscriber.GetEvents();
    EventParser eventParser;
    eventParser.Parse(events);
    for (uint32_t i = 0; i <= MAX_ACTIVE_NUM_PERSECOND; i++) {
        std::string notificationLabel = std::to_string(i);
        std::string notificationIdStr = std::to_string(i);
        int32_t notificationIdInt = i;
        if (i < MAX_ACTIVE_NUM_PERSECOND) {
            std::stringstream stream;
            stream << KEY_SPLITER << USER_ID << KEY_SPLITER << UID << KEY_SPLITER
                << notificationLabel << KEY_SPLITER << notificationIdInt;
            std::string notificationKey = stream.str();
            NotificationSorting sorting;
            EXPECT_EQ(eventParser.GetOnConsumedReq()[i]->GetLabel().c_str(), notificationLabel);
            EXPECT_EQ(eventParser.GetOnConsumedReq()[i]->GetId(), notificationIdInt);
            EXPECT_EQ(eventParser.GetOnConsumedReq()[i]->GetKey(), notificationKey);
            EXPECT_EQ(eventParser.GetOnConsumedWithSortingMapReq()[i]->GetLabel().c_str(), notificationLabel);
            EXPECT_EQ(eventParser.GetOnConsumedWithSortingMapReq()[i]->GetId(), notificationIdInt);
            EXPECT_EQ(eventParser.GetOnConsumedWithSortingMapReq()[i]->GetKey(), notificationKey);
            EXPECT_TRUE(
                eventParser.GetOnConsumedWithSortingMapSor()[i]->GetNotificationSorting(notificationKey, sorting));
            EXPECT_EQ(sorting.GetKey().c_str(), notificationKey);
        }
    }
    EXPECT_EQ((uint32_t)eventParser.GetOnConsumedReq().size(), MAX_ACTIVE_NUM_PERSECOND);
    EXPECT_EQ((uint32_t)eventParser.GetOnConsumedWithSortingMapReq().size(), MAX_ACTIVE_NUM_PERSECOND);
    EXPECT_TRUE(eventParser.GetWaitOnSubscriber());
    EXPECT_TRUE(eventParser.GetWaitOnUnSubscriber());
    subscriber.ClearEvents();
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_RemoveNotificationsByKey_00100
 * @tc.name      : RemoveNotificationsByKey_00100
 * @tc.desc      : Remove Notification by key.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_RemoveNotificationsByKey_00100, Function | MediumTest | Level1)
{
    TestAnsSubscriber subscriber;
    NotificationSubscribeInfo info;
    info.AddAppName("bundleName");
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber, info), ERR_OK);
    std::shared_ptr<NotificationNormalContent> implContent = std::make_shared<NotificationNormalContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);
    NotificationRequest req(0);
    req.SetLabel(NOTIFICATION_LABEL_0);
    req.SetContent(content);
    EXPECT_EQ(NotificationHelper::PublishNotification(req), ERR_OK);
    std::vector<sptr<Notification>> notifications;
    EXPECT_EQ(NotificationHelper::GetAllActiveNotifications(notifications), ERR_OK);
    std::string key = notifications[0]->GetKey().c_str();
    EXPECT_EQ(NotificationHelper::RemoveNotification(key), ERR_OK);
    SleepForFC();
    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber, info), ERR_OK);
    SleepForFC();
    std::list<std::shared_ptr<SubscriberEvent>> events = subscriber.GetEvents();
    EventParser eventParser;
    eventParser.Parse(events);
    EXPECT_TRUE(eventParser.GetWaitOnConsumed());
    EXPECT_TRUE(eventParser.GetWaitOnConsumedWithSortingMap());
    EXPECT_TRUE(eventParser.GetWaitOnCanceled());
    EXPECT_EQ(eventParser.GetOnConsumedReq()[0]->GetLabel().c_str(), NOTIFICATION_LABEL_0);
    EXPECT_EQ(eventParser.GetOnConsumedReq()[0]->GetId(), 0);
    std::stringstream stream;
    stream << KEY_SPLITER << USER_ID << KEY_SPLITER << UID << KEY_SPLITER << NOTIFICATION_LABEL_0 << KEY_SPLITER << 0;
    std::string notificationKey = stream.str();
    NotificationSorting sorting;
    EXPECT_EQ(eventParser.GetOnCanceledReq()[0]->GetKey(), notificationKey);
    EXPECT_EQ(eventParser.GetOnCanceledWithSortingMapReq()[0]->GetKey(), notificationKey);
    EXPECT_TRUE(eventParser.GetOnConsumedWithSortingMapSor()[0]->GetNotificationSorting(notificationKey, sorting));
    EXPECT_EQ(sorting.GetKey().c_str(), notificationKey);
    EXPECT_EQ(eventParser.GetOnCanceledWithSortingMapDelRea()[0], CANCEL_REASON_DELETE);
    subscriber.ClearEvents();
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_RemoveNotificationsByKey_00200
 * @tc.name      : RemoveNotificationsByKey_00200
 * @tc.desc      : Remove Notification by a nonexistent key.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_RemoveNotificationsByKey_00200, Function | MediumTest | Level1)
{
    TestAnsSubscriber subscriber;
    NotificationSubscribeInfo info;
    info.AddAppName("bundleName");
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber, info), ERR_OK);
    std::shared_ptr<NotificationNormalContent> implContent = std::make_shared<NotificationNormalContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);
    NotificationRequest req(0);
    req.SetLabel(NOTIFICATION_LABEL_0);
    req.SetContent(content);
    EXPECT_EQ(NotificationHelper::PublishNotification(req), ERR_OK);
    std::vector<sptr<Notification>> notifications;
    EXPECT_EQ(NotificationHelper::GetAllActiveNotifications(notifications), ERR_OK);
    EXPECT_EQ(NotificationHelper::RemoveNotification(AN_NOT_EXIST_KEY), (int)ERR_ANS_NOTIFICATION_NOT_EXISTS);
    std::string key = notifications[0]->GetKey().c_str();
    EXPECT_EQ(NotificationHelper::RemoveNotification(key), (int)ERR_OK);
    SleepForFC();
    std::list<std::shared_ptr<SubscriberEvent>> events = subscriber.GetEvents();
    EventParser eventParser;
    eventParser.Parse(events);
    EXPECT_TRUE(eventParser.GetWaitOnCanceled());
    eventParser.SetWaitOnCanceled(false);
    EXPECT_EQ(NotificationHelper::RemoveNotification(key), (int)ERR_ANS_NOTIFICATION_NOT_EXISTS);
    events = subscriber.GetEvents();
    eventParser.Parse(events);
    EXPECT_TRUE(eventParser.GetWaitOnCanceled());
    SleepForFC();
    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber, info), ERR_OK);
    SleepForFC();
    events = subscriber.GetEvents();
    eventParser.Parse(events);
    EXPECT_TRUE(eventParser.GetWaitOnConsumedWithSortingMap());
    subscriber.ClearEvents();
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_RemoveNotifications_00100
 * @tc.name      : RemoveNotifications_00100
 * @tc.desc      : Remove all Notifications.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_RemoveNotifications_00100, Function | MediumTest | Level1)
{
    TestAnsSubscriber subscriber;
    NotificationSubscribeInfo info;
    info.AddAppName("bundleName");
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber, info), ERR_OK);

    std::shared_ptr<NotificationNormalContent> implContent = std::make_shared<NotificationNormalContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);
    NotificationRequest req(0);
    req.SetLabel(NOTIFICATION_LABEL_0);
    req.SetContent(content);
    EXPECT_EQ(NotificationHelper::PublishNotification(req), ERR_OK);
    SleepForFC();
    EventParser eventParser;
    std::list<std::shared_ptr<SubscriberEvent>> events = subscriber.GetEvents();
    eventParser.Parse(events);
    EXPECT_TRUE(eventParser.GetWaitOnConsumed());
    eventParser.SetWaitOnConsumed(false);

    NotificationRequest req1(1);
    req1.SetLabel(NOTIFICATION_LABEL_0);
    req1.SetContent(content);
    events = subscriber.GetEvents();
    EXPECT_EQ(NotificationHelper::PublishNotification(req1), ERR_OK);
    SleepForFC();
    eventParser.Parse(events);
    EXPECT_TRUE(eventParser.GetWaitOnConsumed());
    SleepForFC();
    EXPECT_EQ(NotificationHelper::RemoveNotifications(USER_ID), ERR_OK);
    std::vector<sptr<Notification>> notifications;
    EXPECT_EQ(NotificationHelper::GetAllActiveNotifications(notifications), ERR_OK);
    EXPECT_EQ((int)notifications.size(), (int)0);

    SleepForFC();
    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber, info), ERR_OK);
    events = subscriber.GetEvents();
    eventParser.Parse(events);
    EXPECT_TRUE(eventParser.GetWaitOnCanceled());
    EXPECT_TRUE(eventParser.GetWaitOnCanceledWithSortingMapAndDeleteReason());
    subscriber.ClearEvents();
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_RemoveNotifications_00200
 * @tc.name      : RemoveNotifications_00200
 * @tc.desc      : Remove Notifications when no Notification.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_RemoveNotifications_00200, Function | MediumTest | Level1)
{
    EventParser eventParser;
    TestAnsSubscriber subscriber;
    NotificationSubscribeInfo info;
    info.AddAppName("bundleName");
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber, info), ERR_OK);
    EXPECT_EQ(NotificationHelper::RemoveNotifications(), ERR_OK);
    SleepForFC();
    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber, info), ERR_OK);
    std::list<std::shared_ptr<SubscriberEvent>> events = subscriber.GetEvents();
    eventParser.Parse(events);
    EXPECT_FALSE(eventParser.GetWaitOnConsumed());
    EXPECT_FALSE(eventParser.GetWaitOnConsumedWithSortingMap());
    EXPECT_FALSE(eventParser.GetWaitOnCanceled());
    EXPECT_FALSE(eventParser.GetWaitOnCanceledWithSortingMapAndDeleteReason());
    subscriber.ClearEvents();
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_UnSubscriber_00100
 * @tc.name      : UnSubscriber_00100
 * @tc.desc      : Remove Subscriber.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_UnSubscriber_00100, Function | MediumTest | Level1)
{
    TestAnsSubscriber subscriber;
    NotificationSubscribeInfo info;
    info.AddAppName("ANS_FW_MT_UnSubscriber_00100");
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber, info), ERR_OK);
    SleepForFC();
    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber, info), ERR_OK);
    SleepForFC();
    std::list<std::shared_ptr<SubscriberEvent>> events = subscriber.GetEvents();
    bool waitOnSubscriber = false;
    bool waitOnUnSubscriber = false;

    for (auto event : events) {
        if (event->GetType() == SubscriberEventType::ON_SUBSCRIBERESULT) {
            waitOnSubscriber = true;
        } else if (event->GetType() == SubscriberEventType::ON_UNSUBSCRIBERESULT) {
            waitOnUnSubscriber = true;
        }
    }
    EXPECT_TRUE(waitOnSubscriber);

    subscriber.ClearEvents();
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_UnSubscriber_00200
 * @tc.name      : UnSubscriber_00200
 * @tc.desc      : Remove Subscriber.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_UnSubscriber_00200, Function | MediumTest | Level1)
{
    EventParser eventParser;
    TestAnsSubscriber subscriber;
    NotificationSubscribeInfo info;
    info.AddAppName("ANS_FW_MT_UnSubscriber_00100");
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber, info), ERR_OK);
    SleepForFC();
    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber, info), (int)ERR_OK);
    SleepForFC();
    std::list<std::shared_ptr<SubscriberEvent>> events = subscriber.GetEvents();
    bool waitOnSubscriber = false;
    bool waitOnUnSubscriber = false;
    for (auto event : events) {
        if (event->GetType() == SubscriberEventType::ON_SUBSCRIBERESULT) {
            waitOnSubscriber = true;
        } else if (event->GetType() == SubscriberEventType::ON_UNSUBSCRIBERESULT) {
            waitOnUnSubscriber = true;
        }
    }
    EXPECT_TRUE(waitOnSubscriber);
    EXPECT_TRUE(waitOnUnSubscriber);

    waitOnSubscriber = false;
    waitOnUnSubscriber = false;
    subscriber.ClearEvents();
    EXPECT_NE(NotificationHelper::UnSubscribeNotification(subscriber, info), (int)ERR_OK);
    events = subscriber.GetEvents();
    for (auto event : events) {
        if (event->GetType() == SubscriberEventType::ON_UNSUBSCRIBERESULT) {
            waitOnUnSubscriber = true;
        }
    }
    EXPECT_FALSE(waitOnUnSubscriber);
    subscriber.ClearEvents();
    SleepForFC();
}
/**
 *
 * @tc.number    : ANS_FW_MT_Subscriber_00100
 * @tc.name      : Subscriber_00100
 * @tc.desc      : Subscriber Notifications.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_Subscriber_00100, Function | MediumTest | Level1)
{
    TestAnsSubscriber subscriber;
    NotificationSubscribeInfo info;
    info.AddAppName("ANS_FW_MT_UnSubscriber_00100");
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber, info), ERR_OK);
    SleepForFC();
    std::list<std::shared_ptr<SubscriberEvent>> events = subscriber.GetEvents();
    bool waitOnSubscriber = false;
    bool waitOnUnSubscriber = false;
    for (auto event : events) {
        if (event->GetType() == SubscriberEventType::ON_SUBSCRIBERESULT) {
            waitOnSubscriber = true;
        } else if (event->GetType() == SubscriberEventType::ON_UNSUBSCRIBERESULT) {
            waitOnUnSubscriber = true;
        }
    }
    EXPECT_TRUE(waitOnSubscriber);
    subscriber.ClearEvents();
    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber, info), ERR_OK);
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_CancelNotificationById_00100
 * @tc.name      : CancelNotificationById_00100
 * @tc.desc      : Cancel Notification By Id.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_CancelNotificationById_00100, Function | MediumTest | Level1)
{
    EventParser eventParser;
    TestAnsSubscriber subscriber;
    NotificationSubscribeInfo info;
    info.AddAppName("bundleName");
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber, info), ERR_OK);
    std::shared_ptr<NotificationNormalContent> implContent = std::make_shared<NotificationNormalContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);
    NotificationRequest req(1);
    req.SetLabel(NOTIFICATION_LABEL_0);
    req.SetContent(content);
    EXPECT_EQ(NotificationHelper::PublishNotification(req), ERR_OK);
    SleepForFC();
    std::vector<sptr<Notification>> notifications;
    EXPECT_EQ(NotificationHelper::GetAllActiveNotifications(notifications), ERR_OK);
    int32_t id = notifications[0]->GetId();
    SleepForFC();
    EXPECT_EQ(NotificationHelper::CancelNotification(NOTIFICATION_LABEL_0, id), ERR_OK);
    SleepForFC();
    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber, info), ERR_OK);

    SleepForFC();
    std::list<std::shared_ptr<SubscriberEvent>> events = subscriber.GetEvents();

    eventParser.Parse(events);
    EXPECT_TRUE(eventParser.GetWaitOnConsumed());
    EXPECT_TRUE(eventParser.GetWaitOnConsumedWithSortingMap());
    EXPECT_TRUE(eventParser.GetWaitOnCanceled());
    EXPECT_EQ(eventParser.GetOnConsumedReq()[0]->GetLabel().c_str(), NOTIFICATION_LABEL_0);
    EXPECT_EQ(eventParser.GetOnConsumedReq()[0]->GetId(), 1);
    std::stringstream stream;
    stream << KEY_SPLITER << USER_ID << KEY_SPLITER << UID << KEY_SPLITER << NOTIFICATION_LABEL_0 << KEY_SPLITER << 1;
    std::string notificationKey = stream.str();
    NotificationSorting sorting;
    EXPECT_EQ(eventParser.GetOnCanceledReq()[0]->GetKey(), notificationKey);
    EXPECT_EQ(eventParser.GetOnCanceledWithSortingMapReq()[0]->GetKey(), notificationKey);
    EXPECT_TRUE(eventParser.GetOnConsumedWithSortingMapSor()[0]->GetNotificationSorting(notificationKey, sorting));
    EXPECT_EQ(sorting.GetKey().c_str(), notificationKey);
    EXPECT_EQ(eventParser.GetOnCanceledWithSortingMapDelRea()[0], APP_CANCEL_REASON_DELETE);
    subscriber.ClearEvents();
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_CancelNotificationById_00200
 * @tc.name      : CancelNotificationById_00200
 * @tc.desc      : Cancel Notification By Id when Id is not exist.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_CancelNotificationById_00200, Function | MediumTest | Level1)
{
    EventParser eventParser;
    TestAnsSubscriber subscriber;
    NotificationSubscribeInfo info;
    info.AddAppName("bundleName");
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber, info), ERR_OK);

    std::shared_ptr<NotificationNormalContent> implContent = std::make_shared<NotificationNormalContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);
    NotificationRequest req(1);
    req.SetLabel(NOTIFICATION_LABEL_0);
    req.SetContent(content);
    EXPECT_EQ(NotificationHelper::PublishNotification(req), ERR_OK);
    SleepForFC();
    std::vector<sptr<Notification>> notifications;
    EXPECT_EQ(NotificationHelper::GetAllActiveNotifications(notifications), ERR_OK);
    int32_t id = 0;
    SleepForFC();

    eventParser.SetWaitOnCanceled(false);
    eventParser.SetWaitOnCanceledWithSortingMapAndDeleteReason(false);
    EXPECT_EQ(NotificationHelper::CancelNotification(NOTIFICATION_LABEL_0, id), (int)ERR_ANS_NOTIFICATION_NOT_EXISTS);
    SleepForFC();
    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber, info), ERR_OK);

    SleepForFC();
    std::list<std::shared_ptr<SubscriberEvent>> events = subscriber.GetEvents();
    eventParser.Parse(events);
    EXPECT_TRUE(eventParser.GetWaitOnConsumed());
    EXPECT_TRUE(eventParser.GetWaitOnConsumedWithSortingMap());
    EXPECT_FALSE(eventParser.GetWaitOnCanceled());
    EXPECT_FALSE(eventParser.GetWaitOnCanceledWithSortingMapAndDeleteReason());
    subscriber.ClearEvents();
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_CancelAllNotifications_00100
 * @tc.name      :
 * @tc.desc      : Cancel all notifications.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_CancelAllNotifications_00100, Function | MediumTest | Level1)
{
    TestAnsSubscriber subscriber;
    NotificationSubscribeInfo info;
    info.AddAppName("bundleName");
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber, info), ERR_OK);

    std::shared_ptr<NotificationNormalContent> implContent = std::make_shared<NotificationNormalContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);
    NotificationRequest req0(0);
    req0.SetLabel(NOTIFICATION_LABEL_0);
    req0.SetContent(content);
    EXPECT_EQ(NotificationHelper::PublishNotification(req0), ERR_OK);

    NotificationRequest req1(1);
    req1.SetLabel(NOTIFICATION_LABEL_1);
    req1.SetContent(content);
    EXPECT_EQ(NotificationHelper::PublishNotification(req1), ERR_OK);
    SleepForFC();
    EXPECT_EQ(NotificationHelper::CancelAllNotifications(), ERR_OK);
    SleepForFC();
    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber, info), ERR_OK);

    SleepForFC();
    std::list<std::shared_ptr<SubscriberEvent>> events = subscriber.GetEvents();

    EventParser eventParser;
    eventParser.Parse(events);
    EXPECT_TRUE(eventParser.GetWaitOnConsumed());
    EXPECT_TRUE(eventParser.GetWaitOnConsumedWithSortingMap());
    EXPECT_TRUE(eventParser.GetWaitOnCanceled());
    EXPECT_EQ(eventParser.GetOnConsumedReq()[0]->GetLabel().c_str(), NOTIFICATION_LABEL_0);
    EXPECT_EQ(eventParser.GetOnConsumedReq()[0]->GetId(), 0);
    std::stringstream stream0;
    stream0 << KEY_SPLITER << USER_ID << KEY_SPLITER << UID << KEY_SPLITER << NOTIFICATION_LABEL_0 << KEY_SPLITER << 0;
    std::string notificationKey0 = stream0.str();
    NotificationSorting sorting0;
    EXPECT_EQ(eventParser.GetOnCanceledReq()[0]->GetKey(), notificationKey0);
    EXPECT_EQ(eventParser.GetOnCanceledWithSortingMapReq()[0]->GetKey(), notificationKey0);
    EXPECT_TRUE(eventParser.GetOnConsumedWithSortingMapSor()[0]->GetNotificationSorting(notificationKey0, sorting0));
    EXPECT_EQ(sorting0.GetKey().c_str(), notificationKey0);
    EXPECT_EQ(eventParser.GetOnCanceledWithSortingMapDelRea()[0], APP_CANCEL_ALL_REASON_DELETE);

    EXPECT_EQ(eventParser.GetOnConsumedReq()[1]->GetLabel().c_str(), NOTIFICATION_LABEL_1);
    EXPECT_EQ(eventParser.GetOnConsumedReq()[1]->GetId(), 1);
    std::stringstream stream1;
    stream1 << KEY_SPLITER << USER_ID << KEY_SPLITER << UID << KEY_SPLITER << NOTIFICATION_LABEL_1 << KEY_SPLITER << 1;
    std::string notificationKey1 = stream1.str();
    NotificationSorting sorting1;
    EXPECT_EQ(eventParser.GetOnCanceledReq()[1]->GetKey(), notificationKey1);
    EXPECT_EQ(eventParser.GetOnCanceledWithSortingMapReq()[1]->GetKey(), notificationKey1);
    EXPECT_TRUE(eventParser.GetOnConsumedWithSortingMapSor()[1]->GetNotificationSorting(notificationKey1, sorting1));
    EXPECT_EQ(sorting1.GetKey().c_str(), notificationKey1);
    EXPECT_EQ(eventParser.GetOnCanceledWithSortingMapDelRea()[1], APP_CANCEL_ALL_REASON_DELETE);

    subscriber.ClearEvents();
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_PublishSoundNotification_00100
 * @tc.name      :
 * @tc.desc      : Publish a sound notification.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_PublishSoundNotification_00100, Function | MediumTest | Level1)
{
    std::vector<sptr<NotificationSlot>> slots;
    NotificationSlot slot1;
    slot1.SetType(NotificationConstant::SlotType::SOCIAL_COMMUNICATION);
    slot1.SetSound(Uri("."));
    slot1.SetVibrationStyle(std::vector<int64_t>(1, 1));
    EXPECT_EQ(NotificationHelper::AddNotificationSlot(slot1), (int)ERR_OK);

    TestAnsSubscriber subscriber;
    NotificationSubscribeInfo info;
    info.AddAppName("bundleName");
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber, info), ERR_OK);

    std::shared_ptr<NotificationNormalContent> implContent = std::make_shared<NotificationNormalContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);
    NotificationRequest req(0);
    req.SetLabel(NOTIFICATION_LABEL_0);
    req.SetContent(content);
    req.SetSlotType(NotificationConstant::SlotType::SOCIAL_COMMUNICATION);
    EXPECT_EQ(NotificationHelper::PublishNotification(req), ERR_OK);
    SleepForFC();
    EXPECT_EQ(NotificationHelper::CancelAllNotifications(), ERR_OK);
    SleepForFC();
    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber, info), ERR_OK);
    SleepForFC();
    std::list<std::shared_ptr<SubscriberEvent>> events = subscriber.GetEvents();

    EventParser eventParser;
    eventParser.Parse(events);
    EXPECT_TRUE(eventParser.GetWaitOnConsumed());
    EXPECT_TRUE(eventParser.GetWaitOnConsumedWithSortingMap());
    EXPECT_TRUE(eventParser.GetWaitOnCanceled());
    EXPECT_TRUE(eventParser.GetWaitOnCanceledWithSortingMapAndDeleteReason());
    subscriber.ClearEvents();
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_PublishVibrationNotification_00100
 * @tc.name      :
 * @tc.desc      : Publish a vibration notification.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_PublishVibrationNotification_00100, Function | MediumTest | Level1)
{
    std::vector<sptr<NotificationSlot>> slots;
    NotificationSlot slot1;
    slot1.SetType(NotificationConstant::SlotType::SOCIAL_COMMUNICATION);
    slot1.SetEnableVibration(true);
    slot1.SetVibrationStyle(std::vector<int64_t>(1, 1));
    EXPECT_EQ(NotificationHelper::AddNotificationSlot(slot1), (int)ERR_OK);

    TestAnsSubscriber subscriber;
    NotificationSubscribeInfo info;
    info.AddAppName("bundleName");
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber, info), ERR_OK);

    std::shared_ptr<NotificationNormalContent> implContent = std::make_shared<NotificationNormalContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);
    NotificationRequest req(0);
    req.SetLabel(NOTIFICATION_LABEL_0);
    req.SetContent(content);
    req.SetSlotType(NotificationConstant::SlotType::SOCIAL_COMMUNICATION);
    EXPECT_EQ(NotificationHelper::PublishNotification(req), ERR_OK);
    SleepForFC();
    EXPECT_EQ(NotificationHelper::CancelAllNotifications(), ERR_OK);
    SleepForFC();
    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber, info), ERR_OK);
    SleepForFC();
    std::list<std::shared_ptr<SubscriberEvent>> events = subscriber.GetEvents();
    EventParser eventParser;
    eventParser.Parse(events);
    EXPECT_TRUE(eventParser.GetWaitOnConsumed());
    EXPECT_TRUE(eventParser.GetWaitOnConsumedWithSortingMap());
    EXPECT_TRUE(eventParser.GetWaitOnCanceled());
    EXPECT_TRUE(eventParser.GetWaitOnCanceledWithSortingMapAndDeleteReason());
    subscriber.ClearEvents();
    SleepForFC();
}

inline std::shared_ptr<PixelMap> MakePixelMap(int32_t width, int32_t height)
{
    const int32_t PIXEL_BYTES = 4;
    std::shared_ptr<PixelMap> pixelMap = std::make_shared<PixelMap>();
    if (pixelMap == nullptr) {
        return pixelMap;
    }
    ImageInfo info;
    info.size.width = width;
    info.size.height = height;
    info.pixelFormat = PixelFormat::ARGB_8888;
    info.colorSpace = ColorSpace::SRGB;
    pixelMap->SetImageInfo(info);
    int32_t rowDataSize = width * PIXEL_BYTES;
    uint32_t bufferSize = rowDataSize * height;
    void *buffer = malloc(bufferSize);
    if (buffer != nullptr) {
        pixelMap->SetPixelsAddr(buffer, nullptr, bufferSize, AllocatorType::HEAP_ALLOC, nullptr);
    }
    EXPECT_NE(buffer, nullptr);
    return pixelMap;
}

/**
 *
 * @tc.number    : ANS_FW_MT_PublishNotificationWithPixelMap_00100
 * @tc.name      :
 * @tc.desc      : Publish a notification with pixelMap.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_PublishNotificationWithPixelMap_00100, Function | MediumTest | Level1)
{
    const int BIG_PICTURE_WIDTH = 400;
    const int BIG_PICTURE_HEIGHT = 300;
    const int ICON_SIZE = 36;

    NotificationRequest req;
    req.SetSlotType(NotificationConstant::SlotType::OTHER);
    req.SetLabel("label");
    std::shared_ptr<NotificationPictureContent> pictureContent = std::make_shared<NotificationPictureContent>();
    EXPECT_NE(pictureContent, nullptr);
    pictureContent->SetText("notification text");
    pictureContent->SetTitle("notification title");
    std::shared_ptr<PixelMap> bigPicture = MakePixelMap(BIG_PICTURE_WIDTH, BIG_PICTURE_HEIGHT);
    EXPECT_NE(bigPicture, nullptr);
    pictureContent->SetBigPicture(bigPicture);
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(pictureContent);
    EXPECT_NE(content, nullptr);
    req.SetContent(content);
    std::shared_ptr<PixelMap> littleIcon = MakePixelMap(ICON_SIZE, ICON_SIZE);
    req.SetLittleIcon(littleIcon);
    std::shared_ptr<PixelMap> bigIcon = MakePixelMap(ICON_SIZE, ICON_SIZE);
    req.SetBigIcon(bigIcon);
    EXPECT_EQ(NotificationHelper::PublishNotification(req), ERR_OK);
}

/**
 *
 * @tc.number    : ANS_FW_MT_PublishNotificationWithPixelMap_00200
 * @tc.name      :
 * @tc.desc      : Publish a notification with oversize pixelMap.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_PublishNotificationWithPixelMap_00200, Function | MediumTest | Level1)
{
    const int BIG_PICTURE_WIDTH = 1024;
    const int BIG_PICTURE_HEIGHT = 1024;
    const int ICON_SIZE = 36;

    NotificationRequest req;
    req.SetSlotType(NotificationConstant::SlotType::OTHER);
    req.SetLabel("label");
    std::shared_ptr<NotificationPictureContent> pictureContent = std::make_shared<NotificationPictureContent>();
    EXPECT_NE(pictureContent, nullptr);
    pictureContent->SetText("notification text");
    pictureContent->SetTitle("notification title");
    std::shared_ptr<PixelMap> bigPicture = MakePixelMap(BIG_PICTURE_WIDTH, BIG_PICTURE_HEIGHT);
    EXPECT_NE(bigPicture, nullptr);
    pictureContent->SetBigPicture(bigPicture);
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(pictureContent);
    EXPECT_NE(content, nullptr);
    req.SetContent(content);
    std::shared_ptr<PixelMap> littleIcon = MakePixelMap(ICON_SIZE, ICON_SIZE);
    req.SetLittleIcon(littleIcon);
    std::shared_ptr<PixelMap> bigIcon = MakePixelMap(ICON_SIZE, ICON_SIZE);
    req.SetBigIcon(bigIcon);
    EXPECT_EQ(NotificationHelper::PublishNotification(req), (int)ERR_ANS_PICTURE_OVER_SIZE);
}

/**
 *
 * @tc.number    : ANS_FW_MT_PublishNotificationWithPixelMap_00300
 * @tc.name      :
 * @tc.desc      : Publish a notification with oversize pixelMap.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_PublishNotificationWithPixelMap_00300, Function | MediumTest | Level1)
{
    const int BIG_PICTURE_WIDTH = 400;
    const int BIG_PICTURE_HEIGHT = 300;
    const int ICON_SIZE = 256;

    NotificationRequest req;
    req.SetSlotType(NotificationConstant::SlotType::OTHER);
    req.SetLabel("label");
    std::shared_ptr<NotificationPictureContent> pictureContent = std::make_shared<NotificationPictureContent>();
    EXPECT_NE(pictureContent, nullptr);
    pictureContent->SetText("notification text");
    pictureContent->SetTitle("notification title");
    std::shared_ptr<PixelMap> bigPicture = MakePixelMap(BIG_PICTURE_WIDTH, BIG_PICTURE_HEIGHT);
    EXPECT_NE(bigPicture, nullptr);
    pictureContent->SetBigPicture(bigPicture);
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(pictureContent);
    EXPECT_NE(content, nullptr);
    req.SetContent(content);
    std::shared_ptr<PixelMap> littleIcon = MakePixelMap(ICON_SIZE, ICON_SIZE);
    req.SetLittleIcon(littleIcon);
    std::shared_ptr<PixelMap> bigIcon = MakePixelMap(ICON_SIZE, ICON_SIZE);
    req.SetBigIcon(bigIcon);
    EXPECT_EQ(NotificationHelper::PublishNotification(req), (int)ERR_ANS_ICON_OVER_SIZE);
}

/**
 *
 * @tc.number    : ANS_FW_MT_OnDoNotDisturbDateChange_00100
 * @tc.name      :
 * @tc.desc      : OnDoNotDisturbDateChange callback.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_OnDoNotDisturbDateChange_00100, Function | MediumTest | Level1)
{
    TestAnsSubscriber subscriber;
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber), ERR_OK);

    NotificationDoNotDisturbDate date(NotificationConstant::DoNotDisturbType::NONE, 0, 0);
    EXPECT_EQ(NotificationHelper::SetDoNotDisturbDate(date), ERR_OK);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::list<std::shared_ptr<SubscriberEvent>> events = subscriber.GetEvents();
    for (auto event : events) {
        if (event->GetType() == SubscriberEventType::ON_DND_CHANGED) {
            std::shared_ptr<OnDoNotDisturbDateChangedEvent> ev =
                std::static_pointer_cast<OnDoNotDisturbDateChangedEvent>(event);
            auto date = ev->GetDoNotDisturbDate();
            ASSERT_NE(date, nullptr);
            EXPECT_EQ(date->GetDoNotDisturbType(), NotificationConstant::DoNotDisturbType::NONE);
        }
    }

    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber), ERR_OK);
}

/**
 *
 * @tc.number    : ANS_FW_MT_OnDoNotDisturbDateChange_00200
 * @tc.name      :
 * @tc.desc      : OnDoNotDisturbDateChange callback.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_OnDoNotDisturbDateChange_00200, Function | MediumTest | Level1)
{
    TestAnsSubscriber subscriber;
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber), ERR_OK);

    std::chrono::system_clock::time_point timePoint = std::chrono::system_clock::now();
    auto beginDuration = std::chrono::duration_cast<std::chrono::milliseconds>(timePoint.time_since_epoch());
    int64_t beginDate = beginDuration.count();
    timePoint += std::chrono::hours(1);
    auto endDuration = std::chrono::duration_cast<std::chrono::milliseconds>(timePoint.time_since_epoch());
    int64_t endDate = endDuration.count();
    NotificationDoNotDisturbDate date(NotificationConstant::DoNotDisturbType::ONCE, beginDate, endDate);
    EXPECT_EQ(NotificationHelper::SetDoNotDisturbDate(date), ERR_OK);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::list<std::shared_ptr<SubscriberEvent>> events = subscriber.GetEvents();
    for (auto event : events) {
        if (event->GetType() == SubscriberEventType::ON_DND_CHANGED) {
            std::shared_ptr<OnDoNotDisturbDateChangedEvent> ev =
                std::static_pointer_cast<OnDoNotDisturbDateChangedEvent>(event);
            auto date = ev->GetDoNotDisturbDate();
            ASSERT_NE(date, nullptr);
            EXPECT_EQ(date->GetDoNotDisturbType(), NotificationConstant::DoNotDisturbType::ONCE);
        }
    }

    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber), ERR_OK);
}

/**
 *
 * @tc.number    : ANS_FW_MT_OnDoNotDisturbDateChange_00300
 * @tc.name      :
 * @tc.desc      : OnDoNotDisturbDateChange callback.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_OnDoNotDisturbDateChange_00300, Function | MediumTest | Level1)
{
    TestAnsSubscriber subscriber;
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber), ERR_OK);

    std::chrono::system_clock::time_point timePoint = std::chrono::system_clock::now();
    auto beginDuration = std::chrono::duration_cast<std::chrono::milliseconds>(timePoint.time_since_epoch());
    int64_t beginDate = beginDuration.count();
    timePoint += std::chrono::hours(1);
    auto endDuration = std::chrono::duration_cast<std::chrono::milliseconds>(timePoint.time_since_epoch());
    int64_t endDate = endDuration.count();
    NotificationDoNotDisturbDate date(NotificationConstant::DoNotDisturbType::DAILY, beginDate, endDate);
    EXPECT_EQ(NotificationHelper::SetDoNotDisturbDate(date), ERR_OK);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::list<std::shared_ptr<SubscriberEvent>> events = subscriber.GetEvents();
    for (auto event : events) {
        if (event->GetType() == SubscriberEventType::ON_DND_CHANGED) {
            std::shared_ptr<OnDoNotDisturbDateChangedEvent> ev =
                std::static_pointer_cast<OnDoNotDisturbDateChangedEvent>(event);
            auto date = ev->GetDoNotDisturbDate();
            ASSERT_NE(date, nullptr);
            EXPECT_EQ(date->GetDoNotDisturbType(), NotificationConstant::DoNotDisturbType::DAILY);
        }
    }

    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber), ERR_OK);
}

/**
 *
 * @tc.number    : ANS_FW_MT_OnDoNotDisturbDateChange_00400
 * @tc.name      :
 * @tc.desc      : OnDoNotDisturbDateChange callback.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_OnDoNotDisturbDateChange_00400, Function | MediumTest | Level1)
{
    TestAnsSubscriber subscriber;
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber), ERR_OK);

    std::chrono::system_clock::time_point timePoint = std::chrono::system_clock::now();
    auto beginDuration = std::chrono::duration_cast<std::chrono::milliseconds>(timePoint.time_since_epoch());
    int64_t beginDate = beginDuration.count();
    timePoint += std::chrono::hours(1);
    auto endDuration = std::chrono::duration_cast<std::chrono::milliseconds>(timePoint.time_since_epoch());
    int64_t endDate = endDuration.count();
    NotificationDoNotDisturbDate date(NotificationConstant::DoNotDisturbType::CLEARLY, beginDate, endDate);
    EXPECT_EQ(NotificationHelper::SetDoNotDisturbDate(date), ERR_OK);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::list<std::shared_ptr<SubscriberEvent>> events = subscriber.GetEvents();
    for (auto event : events) {
        if (event->GetType() == SubscriberEventType::ON_DND_CHANGED) {
            std::shared_ptr<OnDoNotDisturbDateChangedEvent> ev =
                std::static_pointer_cast<OnDoNotDisturbDateChangedEvent>(event);
            auto date = ev->GetDoNotDisturbDate();
            ASSERT_NE(date, nullptr);
            EXPECT_EQ(date->GetDoNotDisturbType(), NotificationConstant::DoNotDisturbType::CLEARLY);
        }
    }

    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber), ERR_OK);
}

static NotificationDoNotDisturbDate GetDoNotDisturbDateInstance(
    NotificationConstant::DoNotDisturbType type, int64_t intervalHours)
{
    std::chrono::time_point<std::chrono::system_clock> beginTp = std::chrono::system_clock::now();

    auto beginDur = std::chrono::duration_cast<std::chrono::milliseconds>(beginTp.time_since_epoch());
    auto beginMs = beginDur.count();

    auto endDur = beginDur + std::chrono::hours(intervalHours);
    auto endMs = endDur.count();

    return {type, beginMs, endMs};
}

/**
 *
 * @tc.number    : ANS_FW_MT_GetDoNotDisturbDate_00100
 * @tc.name      :
 * @tc.desc      : GetDoNotDisturbDate.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_GetDoNotDisturbDate_00100, Function | MediumTest | Level1)
{
    NotificationDoNotDisturbDate setDate(NotificationConstant::DoNotDisturbType::NONE, 0, 0);
    EXPECT_EQ(NotificationHelper::SetDoNotDisturbDate(setDate), ERR_OK);

    NotificationDoNotDisturbDate getDate;
    EXPECT_EQ(NotificationHelper::GetDoNotDisturbDate(getDate), ERR_OK);
    EXPECT_EQ(getDate.GetDoNotDisturbType(), NotificationConstant::DoNotDisturbType::NONE);
    EXPECT_EQ(getDate.GetBeginDate(), 0);
    EXPECT_EQ(getDate.GetEndDate(), 0);
}

/**
 *
 * @tc.number    : ANS_FW_MT_DoesSupportDoNotDisturbMode_00100
 * @tc.name      :
 * @tc.desc      : DoesSupportDoNotDisturbMode.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_DoesSupportDoNotDisturbMode_00100, Function | MediumTest | Level1)
{
    bool isSupport = false;
    EXPECT_EQ(NotificationHelper::DoesSupportDoNotDisturbMode(isSupport), ERR_OK);
    EXPECT_EQ(isSupport, SUPPORT_DO_NOT_DISTRUB);
}

/**
 * @tc.number    : ANS_Interface_MT_DoNotDisturb_01000
 * @tc.name      : DoNotDisturb_01000
 * @tc.desc      : Set and get DoNotDisturbDate. E.g. 01:40 ~ 02:40
 * @tc.expected  : Set and get DoNotDisturbDate successfully.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_DoNotDisturb_01000, Function | MediumTest | Level1)
{
    auto srcDate = GetDoNotDisturbDateInstance(NotificationConstant::DoNotDisturbType::ONCE, 1);
    EXPECT_EQ(NotificationHelper::SetDoNotDisturbDate(srcDate), ERR_OK);

    NotificationDoNotDisturbDate disDate;
    EXPECT_EQ(NotificationHelper::GetDoNotDisturbDate(disDate), ERR_OK);

    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_01000:: srcDate : beginMs : " << srcDate.GetBeginDate();
    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_01000:: srcDate : endMs   : " << srcDate.GetEndDate();
    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_01000:: disDate : beginMs : " << disDate.GetBeginDate();
    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_01000:: disDate : endMs   : " << disDate.GetEndDate();

    EXPECT_EQ(srcDate.GetDoNotDisturbType(), disDate.GetDoNotDisturbType());
}

/**
 * @tc.number    : ANS_Interface_MT_DoNotDisturb_02000
 * @tc.name      : DoNotDisturb_02000
 * @tc.desc      : Set and get DoNotDisturbDate. E.g. 1970-01-01 01:40 ~ 1970-01-02 01:40
 * @tc.expected  : Set and get DoNotDisturbDate successfully.
 */
HWTEST_F(AnsFWModuleTest, ANS_Interface_MT_DoNotDisturb_02000, Function | MediumTest | Level1)
{
    auto srcDate = GetDoNotDisturbDateInstance(NotificationConstant::DoNotDisturbType::ONCE, 24);
    EXPECT_EQ(NotificationHelper::SetDoNotDisturbDate(srcDate), ERR_OK);

    NotificationDoNotDisturbDate disDate;
    EXPECT_EQ(NotificationHelper::GetDoNotDisturbDate(disDate), ERR_OK);

    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_02000:: srcDate : beginMs : " << srcDate.GetBeginDate();
    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_02000:: srcDate : endMs   : " << srcDate.GetEndDate();
    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_02000:: disDate : beginMs : " << disDate.GetBeginDate();
    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_02000:: disDate : endMs   : " << disDate.GetEndDate();

    EXPECT_EQ(srcDate.GetDoNotDisturbType(), disDate.GetDoNotDisturbType());

    EXPECT_NE(disDate.GetBeginDate(), disDate.GetEndDate());
}

/**
 * @tc.number    : ANS_Interface_MT_DoNotDisturb_03000
 * @tc.name      : DoNotDisturb_03000
 * @tc.desc      : Set and get DoNotDisturbDate. E.g. 1970-01-01 01:40 ~ 1970-01-02 02:40
 * @tc.expected  : Set and get DoNotDisturbDate successfully.
 */
HWTEST_F(AnsFWModuleTest, ANS_Interface_MT_DoNotDisturb_03000, Function | MediumTest | Level1)
{
    auto srcDate = GetDoNotDisturbDateInstance(NotificationConstant::DoNotDisturbType::ONCE, 25);
    EXPECT_EQ(NotificationHelper::SetDoNotDisturbDate(srcDate), ERR_OK);

    NotificationDoNotDisturbDate disDate;
    EXPECT_EQ(NotificationHelper::GetDoNotDisturbDate(disDate), ERR_OK);

    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_03000:: srcDate : beginMs : " << srcDate.GetBeginDate();
    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_03000:: srcDate : endMs   : " << srcDate.GetEndDate();
    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_03000:: disDate : beginMs : " << disDate.GetBeginDate();
    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_03000:: disDate : endMs   : " << disDate.GetEndDate();

    EXPECT_EQ(srcDate.GetDoNotDisturbType(), disDate.GetDoNotDisturbType());
}

/**
 * @tc.number    : ANS_Interface_MT_DoNotDisturb_04000
 * @tc.name      : DoNotDisturb_04000
 * @tc.desc      : Set and get DoNotDisturbDate.  E.g. 01:40 ~ 07:40
 * @tc.expected  : Set and get DoNotDisturbDate successfully.
 */
HWTEST_F(AnsFWModuleTest, ANS_Interface_MT_DoNotDisturb_04000, Function | MediumTest | Level1)
{
    auto srcDate = GetDoNotDisturbDateInstance(NotificationConstant::DoNotDisturbType::DAILY, 6);
    EXPECT_EQ(NotificationHelper::SetDoNotDisturbDate(srcDate), ERR_OK);

    NotificationDoNotDisturbDate disDate;
    EXPECT_EQ(NotificationHelper::GetDoNotDisturbDate(disDate), ERR_OK);

    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_04000:: srcDate : beginMs : " << srcDate.GetBeginDate();
    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_04000:: srcDate : endMs   : " << srcDate.GetEndDate();
    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_04000:: disDate : beginMs : " << disDate.GetBeginDate();
    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_04000:: disDate : endMs   : " << disDate.GetEndDate();

    EXPECT_EQ(srcDate.GetDoNotDisturbType(), disDate.GetDoNotDisturbType());
}

/**
 * @tc.number    : ANS_Interface_MT_DoNotDisturb_05000
 * @tc.name      : DoNotDisturb_05000
 * @tc.desc      : Set and get DoNotDisturbDate.  E.g. 1970-01-01 01:40 ~ 1970-01-02 01:40
 * @tc.expected  : Set and get DoNotDisturbDate successfully.
 */
HWTEST_F(AnsFWModuleTest, ANS_Interface_MT_DoNotDisturb_05000, Function | MediumTest | Level1)
{
    auto srcDate = GetDoNotDisturbDateInstance(NotificationConstant::DoNotDisturbType::DAILY, 24);
    EXPECT_EQ(NotificationHelper::SetDoNotDisturbDate(srcDate), ERR_OK);

    NotificationDoNotDisturbDate disDate;
    EXPECT_EQ(NotificationHelper::GetDoNotDisturbDate(disDate), ERR_OK);

    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_05000:: srcDate : beginMs : " << srcDate.GetBeginDate();
    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_05000:: srcDate : endMs   : " << srcDate.GetEndDate();
    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_05000:: disDate : beginMs : " << disDate.GetBeginDate();
    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_05000:: disDate : endMs   : " << disDate.GetEndDate();

    EXPECT_EQ(srcDate.GetDoNotDisturbType(), disDate.GetDoNotDisturbType());

    EXPECT_NE(disDate.GetBeginDate(), disDate.GetEndDate());
}

/**
 * @tc.number    : ANS_Interface_MT_DoNotDisturb_06000
 * @tc.name      : DoNotDisturb_06000
 * @tc.desc      : Set and get DoNotDisturbDate.  E.g. 1970-01-01 01:40 ~ 1970-01-02 02:40
 * @tc.expected  : Set and get DoNotDisturbDate successfully.
 */
HWTEST_F(AnsFWModuleTest, ANS_Interface_MT_DoNotDisturb_06000, Function | MediumTest | Level1)
{
    auto srcDate = GetDoNotDisturbDateInstance(NotificationConstant::DoNotDisturbType::DAILY, 25);
    EXPECT_EQ(NotificationHelper::SetDoNotDisturbDate(srcDate), ERR_OK);

    NotificationDoNotDisturbDate disDate;
    EXPECT_EQ(NotificationHelper::GetDoNotDisturbDate(disDate), ERR_OK);

    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_06000:: srcDate : beginMs : " << srcDate.GetBeginDate();
    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_06000:: srcDate : endMs   : " << srcDate.GetEndDate();
    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_06000:: disDate : beginMs : " << disDate.GetBeginDate();
    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_06000:: disDate : endMs   : " << disDate.GetEndDate();

    EXPECT_EQ(srcDate.GetDoNotDisturbType(), disDate.GetDoNotDisturbType());
}

/**
 * @tc.number    : ANS_Interface_MT_DoNotDisturb_07000
 * @tc.name      : DoNotDisturb_07000
 * @tc.desc      : Set and get DoNotDisturbDate.  E.g. 1970-01-01 01:40 ~ 1970-01-03 01:40
 * @tc.expected  : Set and get DoNotDisturbDate successfully.
 */
HWTEST_F(AnsFWModuleTest, ANS_Interface_MT_DoNotDisturb_07000, Function | MediumTest | Level1)
{
    auto srcDate = GetDoNotDisturbDateInstance(NotificationConstant::DoNotDisturbType::CLEARLY, 48);
    EXPECT_EQ(NotificationHelper::SetDoNotDisturbDate(srcDate), ERR_OK);

    NotificationDoNotDisturbDate disDate;
    EXPECT_EQ(NotificationHelper::GetDoNotDisturbDate(disDate), ERR_OK);

    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_07000:: srcDate : beginMs : " << srcDate.GetBeginDate();
    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_07000:: srcDate : endMs   : " << srcDate.GetEndDate();
    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_07000:: disDate : beginMs : " << disDate.GetBeginDate();
    GTEST_LOG_(INFO) << "ANS_Interface_MT_DoNotDisturb_07000:: disDate : endMs   : " << disDate.GetEndDate();

    EXPECT_EQ(srcDate.GetDoNotDisturbType(), disDate.GetDoNotDisturbType());
}

/**
 *
 * @tc.number    : ANS_FW_MT_DistributedNotification_SetEnable_00100
 * @tc.name      : DistributedNotification_SetEnable_00100
 * @tc.desc      : Set distributed notification enable.
 */
HWTEST_F(AnsFWModuleTest, DistributedNotification_SetEnable_00100, Function | MediumTest | Level1)
{
    ANS_LOGI("%{public}s", test_info_->name());
    bool enable;

    ASSERT_EQ(NotificationHelper::EnableDistributed(false), ERR_OK);
    ASSERT_EQ(NotificationHelper::IsDistributedEnabled(enable), ERR_OK);
    ASSERT_EQ(enable, false);

    ASSERT_EQ(NotificationHelper::EnableDistributed(true), ERR_OK);
    ASSERT_EQ(NotificationHelper::IsDistributedEnabled(enable), ERR_OK);
    ASSERT_EQ(enable, true);
}

#ifdef DISTRIBUTED_NOTIFICATION_SUPPORTED
/**
 *
 * @tc.number    : ANS_FW_MT_DistributedNotification_SetEnableByBundle_00100
 * @tc.name      : DistributedNotification_SetEnableByBundle_00100
 * @tc.desc      : Set distributed notification enable by bundle.
 */
HWTEST_F(AnsFWModuleTest, DistributedNotification_SetEnableByBundle_00100, Function | MediumTest | Level1)
{
    ANS_LOGI("%{public}s", test_info_->name());
    bool enable;
    NotificationBundleOption bundleOption(APP_NAME, UID);

    ASSERT_EQ(NotificationHelper::EnableDistributedByBundle(bundleOption, false), ERR_OK);
    ASSERT_EQ(NotificationHelper::IsDistributedEnableByBundle(bundleOption, enable), ERR_OK);
    ASSERT_EQ(enable, false);

    ASSERT_EQ(NotificationHelper::EnableDistributedByBundle(bundleOption, true), ERR_OK);
    ASSERT_EQ(NotificationHelper::IsDistributedEnableByBundle(bundleOption, enable), ERR_OK);
    ASSERT_EQ(enable, true);

    ASSERT_EQ(NotificationHelper::EnableDistributedSelf(false), ERR_OK);
    ASSERT_EQ(NotificationHelper::IsDistributedEnableByBundle(bundleOption, enable), ERR_OK);
    ASSERT_EQ(enable, false);

    ASSERT_EQ(NotificationHelper::EnableDistributedSelf(true), ERR_OK);
    ASSERT_EQ(NotificationHelper::IsDistributedEnableByBundle(bundleOption, enable), ERR_OK);
    ASSERT_EQ(enable, true);
}

/**
 *
 * @tc.number    : ANS_FW_MT_DistributedNotification_Publish_00100
 * @tc.name      : DistributedNotification_Publish_00100
 * @tc.desc      : publish a notification to distributed kvstore.
 */
HWTEST_F(AnsFWModuleTest, DistributedNotification_Publish_00100, Function | MediumTest | Level1)
{
    ANS_LOGI("%{public}s", test_info_->name());
    NotificationRequest request = CreateDistributedRequest(test_info_->name());

    DistributedKv::AppId appId = {.appId = KVSTORE_APP_ID};
    DistributedKv::StoreId storeId = {.storeId = KVSTORE_NOTIFICATION_STORE_ID};
    std::shared_ptr<DistributedKv::MockSingleKvStore> pointer =
        DistributedKv::MockSingleKvStore::GetMockKvStorePointer(appId, storeId);
    std::vector<DistributedKv::Entry> entries;

    ASSERT_EQ(NotificationHelper::PublishNotification(request), ERR_OK);
    ASSERT_EQ(pointer->GetEntries(DistributedKv::Key(""), entries), DistributedKv::Status::SUCCESS);
    DistributedKv::Entry outEntry;
    ASSERT_EQ(GetRequestInDistributedEntryList(request, entries, outEntry), true);
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_DistributedNotification_Publish_00200
 * @tc.name      : DistributedNotification_Publish_00200
 * @tc.desc      : publish a local notification not use distributed kvstore.
 */
HWTEST_F(AnsFWModuleTest, DistributedNotification_Publish_00200, Function | MediumTest | Level1)
{
    ANS_LOGI("%{public}s", test_info_->name());
    NotificationRequest request = CreateDistributedRequest(test_info_->name());
    request.SetDistributed(false);

    DistributedKv::AppId appId = {.appId = KVSTORE_APP_ID};
    DistributedKv::StoreId storeId = {.storeId = KVSTORE_NOTIFICATION_STORE_ID};
    std::shared_ptr<DistributedKv::MockSingleKvStore> pointer =
        DistributedKv::MockSingleKvStore::GetMockKvStorePointer(appId, storeId);
    std::vector<DistributedKv::Entry> entries;

    ASSERT_EQ(NotificationHelper::PublishNotification(request), ERR_OK);
    ASSERT_EQ(pointer->GetEntries(DistributedKv::Key(""), entries), DistributedKv::Status::SUCCESS);
    DistributedKv::Entry outEntry;
    ASSERT_EQ(GetRequestInDistributedEntryList(request, entries, outEntry), false);
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_DistributedNotification_Publish_00300
 * @tc.name      : DistributedNotification_Publish_00300 MockSetDistributedNotificationEnabled
 * @tc.desc      : publish a distributed notification when DistributedNotificationEnabled is false in application info.
 */
HWTEST_F(AnsFWModuleTest, DistributedNotification_Publish_00300, Function | MediumTest | Level1)
{
    ANS_LOGI("%{public}s", test_info_->name());
    NotificationRequest request = CreateDistributedRequest(test_info_->name());
    request.SetDistributed(true);

    DistributedKv::AppId appId = {.appId = KVSTORE_APP_ID};
    DistributedKv::StoreId storeId = {.storeId = KVSTORE_NOTIFICATION_STORE_ID};
    std::shared_ptr<DistributedKv::MockSingleKvStore> pointer =
        DistributedKv::MockSingleKvStore::GetMockKvStorePointer(appId, storeId);
    std::vector<DistributedKv::Entry> entries;

    AppExecFwk::MockSetDistributedNotificationEnabled(false);
    ASSERT_EQ(NotificationHelper::PublishNotification(request), ERR_OK);
    ASSERT_EQ(pointer->GetEntries(DistributedKv::Key(""), entries), DistributedKv::Status::SUCCESS);
    DistributedKv::Entry outEntry;
    ASSERT_EQ(GetRequestInDistributedEntryList(request, entries, outEntry), true);
    AppExecFwk::MockSetDistributedNotificationEnabled(true);
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_DistributedNotification_Cancel_00100
 * @tc.name      : DistributedNotification_Cancel_00100
 * @tc.desc      : cancel a notification to distributed kvstore ,use CancelNotification(label, id).
 */
HWTEST_F(AnsFWModuleTest, DistributedNotification_Cancel_00100, Function | MediumTest | Level1)
{
    ANS_LOGI("%{public}s", test_info_->name());
    NotificationRequest request = CreateDistributedRequest(test_info_->name());

    DistributedKv::AppId appId = {.appId = KVSTORE_APP_ID};
    DistributedKv::StoreId storeId = {.storeId = KVSTORE_NOTIFICATION_STORE_ID};
    std::shared_ptr<DistributedKv::MockSingleKvStore> pointer =
        DistributedKv::MockSingleKvStore::GetMockKvStorePointer(appId, storeId);
    std::vector<DistributedKv::Entry> entries;

    ASSERT_EQ(NotificationHelper::PublishNotification(request), ERR_OK);
    ASSERT_EQ(pointer->GetEntries(DistributedKv::Key(""), entries), DistributedKv::Status::SUCCESS);
    DistributedKv::Entry outEntry;
    ASSERT_EQ(GetRequestInDistributedEntryList(request, entries, outEntry), true);

    ASSERT_EQ(NotificationHelper::CancelNotification(request.GetLabel(), request.GetNotificationId()), ERR_OK);
    ASSERT_EQ(pointer->GetEntries(DistributedKv::Key(""), entries), DistributedKv::Status::SUCCESS);
    ASSERT_EQ(GetRequestInDistributedEntryList(request, entries, outEntry), false);
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_DistributedNotification_Cancel_00200
 * @tc.name      : DistributedNotification_Cancel_00200
 * @tc.desc      : cancel a notification to distributed kvstore ,use CancelAllNotifications().
 */
HWTEST_F(AnsFWModuleTest, DistributedNotification_Cancel_00200, Function | MediumTest | Level1)
{
    ANS_LOGI("%{public}s", test_info_->name());
    NotificationRequest request = CreateDistributedRequest(test_info_->name());

    DistributedKv::AppId appId = {.appId = KVSTORE_APP_ID};
    DistributedKv::StoreId storeId = {.storeId = KVSTORE_NOTIFICATION_STORE_ID};
    std::shared_ptr<DistributedKv::MockSingleKvStore> pointer =
        DistributedKv::MockSingleKvStore::GetMockKvStorePointer(appId, storeId);
    std::vector<DistributedKv::Entry> entries;

    ASSERT_EQ(NotificationHelper::PublishNotification(request), ERR_OK);
    ASSERT_EQ(pointer->GetEntries(DistributedKv::Key(""), entries), DistributedKv::Status::SUCCESS);
    DistributedKv::Entry outEntry;
    ASSERT_EQ(GetRequestInDistributedEntryList(request, entries, outEntry), true);

    request.SetNotificationId(request.GetNotificationId() + 1);
    ASSERT_EQ(NotificationHelper::PublishNotification(request), ERR_OK);
    ASSERT_EQ(pointer->GetEntries(DistributedKv::Key(""), entries), DistributedKv::Status::SUCCESS);
    ASSERT_EQ(GetRequestInDistributedEntryList(request, entries, outEntry), true);

    ASSERT_EQ(NotificationHelper::CancelAllNotifications(), ERR_OK);
    ASSERT_EQ(pointer->GetEntries(DistributedKv::Key(""), entries), DistributedKv::Status::SUCCESS);
    ASSERT_EQ(entries.size(), std::size_t(0));
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_DistributedNotification_Remove_00100
 * @tc.name      : DistributedNotification_Remove_00100
 * @tc.desc      : remove a notification to distributed kvstore ,use RemoveNotification(bundleOption, id, label).
 */
HWTEST_F(AnsFWModuleTest, DistributedNotification_Remove_00100, Function | MediumTest | Level1)
{
    ANS_LOGI("%{public}s", test_info_->name());
    NotificationRequest request = CreateDistributedRequest(test_info_->name());

    DistributedKv::AppId appId = {.appId = KVSTORE_APP_ID};
    DistributedKv::StoreId storeId = {.storeId = KVSTORE_NOTIFICATION_STORE_ID};
    std::shared_ptr<DistributedKv::MockSingleKvStore> pointer =
        DistributedKv::MockSingleKvStore::GetMockKvStorePointer(appId, storeId);
    std::vector<DistributedKv::Entry> entries;

    ASSERT_EQ(NotificationHelper::PublishNotification(request), ERR_OK);
    ASSERT_EQ(pointer->GetEntries(DistributedKv::Key(""), entries), DistributedKv::Status::SUCCESS);
    DistributedKv::Entry outEntry;
    ASSERT_EQ(GetRequestInDistributedEntryList(request, entries, outEntry), true);

    NotificationBundleOption bundleOption(APP_NAME, UID);
    ASSERT_EQ(
        NotificationHelper::RemoveNotification(bundleOption, request.GetNotificationId(), request.GetLabel()), ERR_OK);
    ASSERT_EQ(pointer->GetEntries(DistributedKv::Key(""), entries), DistributedKv::Status::SUCCESS);
    ASSERT_EQ(GetRequestInDistributedEntryList(request, entries, outEntry), false);
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_DistributedNotification_Remove_00200
 * @tc.name      : DistributedNotification_Remove_00200
 * @tc.desc      : remove a notification to distributed kvstore ,use RemoveNotifications().
 */
HWTEST_F(AnsFWModuleTest, DistributedNotification_Remove_00200, Function | MediumTest | Level1)
{
    ANS_LOGI("%{public}s", test_info_->name());
    NotificationRequest request = CreateDistributedRequest(test_info_->name());

    DistributedKv::AppId appId = {.appId = KVSTORE_APP_ID};
    DistributedKv::StoreId storeId = {.storeId = KVSTORE_NOTIFICATION_STORE_ID};
    std::shared_ptr<DistributedKv::MockSingleKvStore> pointer =
        DistributedKv::MockSingleKvStore::GetMockKvStorePointer(appId, storeId);
    std::vector<DistributedKv::Entry> entries;

    ASSERT_EQ(NotificationHelper::PublishNotification(request), ERR_OK);
    ASSERT_EQ(pointer->GetEntries(DistributedKv::Key(""), entries), DistributedKv::Status::SUCCESS);
    DistributedKv::Entry outEntry;
    ASSERT_EQ(GetRequestInDistributedEntryList(request, entries, outEntry), true);

    request.SetNotificationId(request.GetNotificationId() + 1);
    ASSERT_EQ(NotificationHelper::PublishNotification(request), ERR_OK);
    ASSERT_EQ(pointer->GetEntries(DistributedKv::Key(""), entries), DistributedKv::Status::SUCCESS);
    ASSERT_EQ(GetRequestInDistributedEntryList(request, entries, outEntry), true);

    ASSERT_EQ(NotificationHelper::RemoveNotifications(USER_ID), ERR_OK);
    ASSERT_EQ(pointer->GetEntries(DistributedKv::Key(""), entries), DistributedKv::Status::SUCCESS);
    ASSERT_EQ(entries.size(), std::size_t(0));
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_DistributedNotification_Remove_00300
 * @tc.name      : DistributedNotification_Remove_00300
 * @tc.desc      : remove a notification to distributed kvstore ,use RemoveNotificationsByBundle(bundleOption).
 */
HWTEST_F(AnsFWModuleTest, DistributedNotification_Remove_00300, Function | MediumTest | Level1)
{
    ANS_LOGI("%{public}s", test_info_->name());
    NotificationRequest request = CreateDistributedRequest(test_info_->name());

    DistributedKv::AppId appId = {.appId = KVSTORE_APP_ID};
    DistributedKv::StoreId storeId = {.storeId = KVSTORE_NOTIFICATION_STORE_ID};
    std::shared_ptr<DistributedKv::MockSingleKvStore> pointer =
        DistributedKv::MockSingleKvStore::GetMockKvStorePointer(appId, storeId);
    std::vector<DistributedKv::Entry> entries;

    ASSERT_EQ(NotificationHelper::PublishNotification(request), ERR_OK);
    ASSERT_EQ(pointer->GetEntries(DistributedKv::Key(""), entries), DistributedKv::Status::SUCCESS);
    DistributedKv::Entry outEntry;
    ASSERT_EQ(GetRequestInDistributedEntryList(request, entries, outEntry), true);

    request.SetNotificationId(request.GetNotificationId() + 1);
    ASSERT_EQ(NotificationHelper::PublishNotification(request), ERR_OK);
    ASSERT_EQ(pointer->GetEntries(DistributedKv::Key(""), entries), DistributedKv::Status::SUCCESS);
    ASSERT_EQ(GetRequestInDistributedEntryList(request, entries, outEntry), true);

    NotificationBundleOption bundleOption(APP_NAME, UID);
    ASSERT_EQ(NotificationHelper::RemoveNotificationsByBundle(bundleOption), ERR_OK);
    ASSERT_EQ(pointer->GetEntries(DistributedKv::Key(""), entries), DistributedKv::Status::SUCCESS);
    ASSERT_EQ(entries.size(), std::size_t(0));
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_DistributedNotification_Subscribe_00100
 * @tc.name      : DistributedNotification_Subscribe_00100
 * @tc.desc      : distributed kvstore callback data insert/update/delete.
 */
HWTEST_F(AnsFWModuleTest, DistributedNotification_Subscribe_00100, Function | MediumTest | Level1)
{
    ANS_LOGI("%{public}s", test_info_->name());
    NotificationRequest request = CreateDistributedRequest(test_info_->name());
    std::string jsonString;
    NotificationJsonConverter::ConvertToJsonString(&request, jsonString);

    DistributedKv::AppId appId = {.appId = KVSTORE_APP_ID};
    DistributedKv::StoreId storeId = {.storeId = KVSTORE_NOTIFICATION_STORE_ID};
    std::shared_ptr<DistributedKv::MockSingleKvStore> pointer =
        DistributedKv::MockSingleKvStore::GetMockKvStorePointer(appId, storeId);
    std::vector<DistributedKv::Entry> entries;

    TestAnsSubscriber subscriber;
    ASSERT_EQ(NotificationHelper::SubscribeNotification(subscriber), ERR_OK);

    DistributedKv::Key key(GenerateDistributedKey(request, REMOTE_DEVICE_ID));
    DistributedKv::Value value(jsonString);
    pointer->InsertDataToDoCallback(key, value);
    SleepForFC();

    EventParser parser1;
    parser1.Parse(subscriber.GetEvents());
    auto notificationList = parser1.GetOnConsumedWithSortingMapReq();
    EXPECT_NE(notificationList.size(), std::size_t(0));
    std::shared_ptr<Notification> outNotification;
    EXPECT_EQ(GetRequestInNotificationList(request, notificationList, outNotification), true);
    subscriber.ClearEvents();

    pointer->UpdateDataToDoCallback(key, value);
    SleepForFC();

    EventParser parser2;
    parser2.Parse(subscriber.GetEvents());
    notificationList = parser2.GetOnConsumedWithSortingMapReq();
    EXPECT_NE(notificationList.size(), std::size_t(0));
    EXPECT_EQ(GetRequestInNotificationList(request, notificationList, outNotification), true);
    subscriber.ClearEvents();

    pointer->DeleteDataToDoCallback(key);
    SleepForFC();

    EventParser parser3;
    parser3.Parse(subscriber.GetEvents());
    notificationList = parser3.GetOnCanceledReq();
    EXPECT_NE(notificationList.size(), std::size_t(0));
    EXPECT_EQ(GetRequestInNotificationList(request, notificationList, outNotification), true);
    subscriber.ClearEvents();
    ASSERT_EQ(NotificationHelper::UnSubscribeNotification(subscriber), ERR_OK);

    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_DistributedNotification_Subscribe_00200
 * @tc.name      : DistributedNotification_Subscribe_00200
 * @tc.desc      : distributed kvstore callback data, delete notification after OnConsumed.
 */
HWTEST_F(AnsFWModuleTest, DistributedNotification_Subscribe_00200, Function | MediumTest | Level1)
{
    ANS_LOGI("%{public}s", test_info_->name());
    NotificationRequest request = CreateDistributedRequest(test_info_->name());
    request.SetOwnerBundleName(APP_NAME);
    request.SetCreatorBundleName(APP_NAME);
    std::string jsonString;
    NotificationJsonConverter::ConvertToJsonString(&request, jsonString);

    DistributedKv::AppId appId = {.appId = KVSTORE_APP_ID};
    DistributedKv::StoreId storeId = {.storeId = KVSTORE_NOTIFICATION_STORE_ID};
    std::shared_ptr<DistributedKv::MockSingleKvStore> pointer =
        DistributedKv::MockSingleKvStore::GetMockKvStorePointer(appId, storeId);
    std::vector<DistributedKv::Entry> entries;

    TestAnsSubscriber subscriber;
    ASSERT_EQ(NotificationHelper::SubscribeNotification(subscriber), ERR_OK);

    DistributedKv::Key key(GenerateDistributedKey(request, REMOTE_DEVICE_ID));
    DistributedKv::Value value(jsonString);
    pointer->InsertDataToDoCallback(key, value);
    SleepForFC();

    EventParser parser1;
    parser1.Parse(subscriber.GetEvents());
    auto notificationList = parser1.GetOnConsumedWithSortingMapReq();
    EXPECT_NE(notificationList.size(), std::size_t(0));
    std::shared_ptr<Notification> outNotification;
    EXPECT_EQ(GetRequestInNotificationList(request, notificationList, outNotification), true);

    EXPECT_EQ(NotificationHelper::RemoveNotifications(), ERR_OK);

    EXPECT_EQ(pointer->GetEntries(DistributedKv::Key(""), entries), DistributedKv::Status::SUCCESS);
    EXPECT_EQ(entries.size(), std::size_t(0));
    subscriber.ClearEvents();
    ASSERT_EQ(NotificationHelper::UnSubscribeNotification(subscriber), ERR_OK);
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_Interface_MT_GetDeviceRemindType_00100
 * @tc.name      : GetDeviceRemindType_00100
 * @tc.desc      : Get device remind type.
 */
HWTEST_F(AnsFWModuleTest, ANS_Interface_MT_GetDeviceRemindType_00100, Function | MediumTest | Level1)
{
    auto rType = NotificationConstant::RemindType::NONE;
    EXPECT_EQ(NotificationHelper::GetDeviceRemindType(rType), ERR_OK);
    ANS_LOGI("ANS_Interface_MT_GetDeviceRemindType_00100:: rType : %{public}d", static_cast<int32_t>(rType));

    EXPECT_NE(rType, NotificationConstant::RemindType::NONE);
}

/**
 *
 * @tc.number    : ANS_FW_MT_DistributedNotification_ScreenStatusChange_00100
 * @tc.name      : ScreenStatusChange_00100
 * @tc.desc      : Receive local screen status from common event to kvstore.
 */
HWTEST_F(AnsFWModuleTest, ScreenStatusChange_00100, Function | MediumTest | Level1)
{
    ANS_LOGI("%{public}s", test_info_->name());
    DistributedKv::AppId appId = {.appId = KVSTORE_APP_ID};
    DistributedKv::StoreId storeId = {.storeId = KVSTORE_SCREEN_STATUS_STORE_ID};
    std::shared_ptr<DistributedKv::MockSingleKvStore> pointer =
        DistributedKv::MockSingleKvStore::GetMockKvStorePointer(appId, storeId);
    DistributedKv::Key key("<localDeviceId>");
    std::vector<DistributedKv::Entry> entries;

    PublishCommonEventScreenStatus(false);
    EXPECT_EQ(pointer->GetEntries(key, entries), DistributedKv::Status::SUCCESS);
    EXPECT_EQ(entries.size(), std::size_t(1));
    EXPECT_EQ(entries[0].value.ToString(), "off");
    entries.clear();

    PublishCommonEventScreenStatus(true);
    EXPECT_EQ(pointer->GetEntries(key, entries), DistributedKv::Status::SUCCESS);
    EXPECT_EQ(entries.size(), std::size_t(1));
    EXPECT_EQ(entries[0].value.ToString(), "on");
    entries.clear();
}

/**
 *
 * @tc.number    : ANS_FW_MT_DistributedNotification_DefaultRemindPolicy_00100
 * @tc.name      : DefaultRemindPolicy_00100
 * @tc.desc      : Publish a notification when local screen on and remote screen off.
 */
HWTEST_F(AnsFWModuleTest, DefaultRemindPolicy_00100, Function | MediumTest | Level1)
{
    ANS_LOGI("%{public}s", test_info_->name());
    NotificationRequest request = CreateDistributedRequest(test_info_->name());

    PublishCommonEventScreenStatus(true);
    SetDistributedScreenStatus(false);

    TestAnsSubscriber subscriber;
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber), ERR_OK);
    EXPECT_EQ(NotificationHelper::PublishNotification(request), ERR_OK);
    SleepForFC();

    EventParser parser;
    parser.Parse(subscriber.GetEvents());
    auto notificationList = parser.GetOnConsumedWithSortingMapReq();
    std::shared_ptr<Notification> outNotification;
    EXPECT_EQ(GetRequestInNotificationList(request, notificationList, outNotification), true);
    EXPECT_EQ(outNotification->GetRemindType(), NotificationConstant::RemindType::DEVICE_ACTIVE_REMIND);

    subscriber.ClearEvents();
    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber), ERR_OK);
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_DistributedNotification_DefaultRemindPolicy_00200
 * @tc.name      : DefaultRemindPolicy_00200
 * @tc.desc      : Publish a notification when local screen on and remote screen on.
 */
HWTEST_F(AnsFWModuleTest, DefaultRemindPolicy_00200, Function | MediumTest | Level1)
{
    ANS_LOGI("%{public}s", test_info_->name());
    NotificationRequest request = CreateDistributedRequest(test_info_->name());

    PublishCommonEventScreenStatus(true);
    SetDistributedScreenStatus(true);

    TestAnsSubscriber subscriber;
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber), ERR_OK);
    EXPECT_EQ(NotificationHelper::PublishNotification(request), ERR_OK);
    SleepForFC();

    EventParser parser;
    parser.Parse(subscriber.GetEvents());
    auto notificationList = parser.GetOnConsumedWithSortingMapReq();
    std::shared_ptr<Notification> outNotification;
    EXPECT_EQ(GetRequestInNotificationList(request, notificationList, outNotification), true);
    EXPECT_EQ(outNotification->GetRemindType(), NotificationConstant::RemindType::DEVICE_ACTIVE_REMIND);

    subscriber.ClearEvents();
    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber), ERR_OK);
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_DistributedNotification_DefaultRemindPolicy_00300
 * @tc.name      : DefaultRemindPolicy_00300
 * @tc.desc      : Publish a notification when local screen off and remote screen off.
 */
HWTEST_F(AnsFWModuleTest, DefaultRemindPolicy_00300, Function | MediumTest | Level1)
{
    ANS_LOGI("%{public}s", test_info_->name());
    NotificationRequest request = CreateDistributedRequest(test_info_->name());

    PublishCommonEventScreenStatus(false);
    SetDistributedScreenStatus(false);

    TestAnsSubscriber subscriber;
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber), ERR_OK);
    EXPECT_EQ(NotificationHelper::PublishNotification(request), ERR_OK);
    SleepForFC();

    EventParser parser;
    parser.Parse(subscriber.GetEvents());
    auto notificationList = parser.GetOnConsumedWithSortingMapReq();
    std::shared_ptr<Notification> outNotification;
    EXPECT_EQ(GetRequestInNotificationList(request, notificationList, outNotification), true);
    EXPECT_EQ(outNotification->GetRemindType(), NotificationConstant::RemindType::DEVICE_IDLE_REMIND);

    subscriber.ClearEvents();
    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber), ERR_OK);
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_DistributedNotification_DefaultRemindPolicy_00400
 * @tc.name      : DefaultRemindPolicy_00400
 * @tc.desc      : Publish a notification when local screen off and remote screen on.
 */
HWTEST_F(AnsFWModuleTest, DefaultRemindPolicy_00400, Function | MediumTest | Level1)
{
    ANS_LOGI("%{public}s", test_info_->name());
    NotificationRequest request = CreateDistributedRequest(test_info_->name());

    PublishCommonEventScreenStatus(false);
    SetDistributedScreenStatus(true);

    TestAnsSubscriber subscriber;
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber), ERR_OK);
    EXPECT_EQ(NotificationHelper::PublishNotification(request), ERR_OK);
    SleepForFC();

    EventParser parser;
    parser.Parse(subscriber.GetEvents());
    auto notificationList = parser.GetOnConsumedWithSortingMapReq();
    std::shared_ptr<Notification> outNotification;
    EXPECT_EQ(GetRequestInNotificationList(request, notificationList, outNotification), true);
    EXPECT_EQ(outNotification->GetRemindType(), NotificationConstant::RemindType::DEVICE_IDLE_DONOT_REMIND);

    subscriber.ClearEvents();
    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber), ERR_OK);
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_DistributedNotification_DefaultRemindPolicy_00500
 * @tc.name      : DefaultRemindPolicy_00500
 * @tc.desc      : Receive distributed notification when screen is on.
 */
HWTEST_F(AnsFWModuleTest, DefaultRemindPolicy_00500, Function | MediumTest | Level1)
{
    ANS_LOGI("%{public}s", test_info_->name());
    std::vector<std::string> devices = {"<localDeviceType>"};
    NotificationRequest request = CreateDistributedRequest(test_info_->name());
    request.SetOwnerBundleName(APP_NAME);
    request.SetCreatorBundleName(APP_NAME);
    request.SetDevicesSupportDisplay(devices);
    std::string jsonString;
    NotificationJsonConverter::ConvertToJsonString(&request, jsonString);

    PublishCommonEventScreenStatus(true);

    DistributedKv::AppId appId = {.appId = KVSTORE_APP_ID};
    DistributedKv::StoreId storeId = {.storeId = KVSTORE_NOTIFICATION_STORE_ID};
    std::shared_ptr<DistributedKv::MockSingleKvStore> pointer =
        DistributedKv::MockSingleKvStore::GetMockKvStorePointer(appId, storeId);

    TestAnsSubscriber subscriber;
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber), ERR_OK);

    DistributedKv::Key key(GenerateDistributedKey(request, REMOTE_DEVICE_ID));
    DistributedKv::Value value(jsonString);
    pointer->InsertDataToDoCallback(key, value);
    SleepForFC();
    SleepForFC();

    EventParser parser;
    parser.Parse(subscriber.GetEvents());
    auto notificationList = parser.GetOnConsumedWithSortingMapReq();
    std::shared_ptr<Notification> outNotification;
    EXPECT_EQ(GetRequestInNotificationList(request, notificationList, outNotification), true);
    EXPECT_EQ(outNotification->GetRemindType(), NotificationConstant::RemindType::DEVICE_ACTIVE_REMIND);

    subscriber.ClearEvents();
    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber), ERR_OK);
    SleepForFC();
}

/**
 *
 * @tc.number    : ANS_FW_MT_DistributedNotification_DefaultRemindPolicy_00600
 * @tc.name      : DefaultRemindPolicy_00600
 * @tc.desc      : Receive distributed notification when screen is off.
 */
HWTEST_F(AnsFWModuleTest, DefaultRemindPolicy_00600, Function | MediumTest | Level1)
{
    ANS_LOGI("%{public}s", test_info_->name());
    std::vector<std::string> devices = {"<localDeviceType>"};
    NotificationRequest request = CreateDistributedRequest(test_info_->name());
    request.SetOwnerBundleName(APP_NAME);
    request.SetCreatorBundleName(APP_NAME);
    request.SetDevicesSupportDisplay(devices);
    std::string jsonString;
    NotificationJsonConverter::ConvertToJsonString(&request, jsonString);

    PublishCommonEventScreenStatus(false);

    DistributedKv::AppId appId = {.appId = KVSTORE_APP_ID};
    DistributedKv::StoreId storeId = {.storeId = KVSTORE_NOTIFICATION_STORE_ID};
    std::shared_ptr<DistributedKv::MockSingleKvStore> pointer =
        DistributedKv::MockSingleKvStore::GetMockKvStorePointer(appId, storeId);

    TestAnsSubscriber subscriber;
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber), ERR_OK);

    DistributedKv::Key key(GenerateDistributedKey(request, REMOTE_DEVICE_ID));
    DistributedKv::Value value(jsonString);
    pointer->InsertDataToDoCallback(key, value);
    SleepForFC();

    EventParser parser;
    parser.Parse(subscriber.GetEvents());
    auto notificationList = parser.GetOnConsumedWithSortingMapReq();
    std::shared_ptr<Notification> outNotification;
    EXPECT_EQ(GetRequestInNotificationList(request, notificationList, outNotification), true);
    EXPECT_EQ(outNotification->GetRemindType(), NotificationConstant::RemindType::DEVICE_IDLE_DONOT_REMIND);

    subscriber.ClearEvents();
    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber), ERR_OK);
    SleepForFC();
}
#endif

HWTEST_F(AnsFWModuleTest, ANS_Interface_MT_PublishContinuousTask_07100, Function | MediumTest | Level1)
{
    MockIPCSkeleton::SetCallingUid(SYSTEM_SERVICE_UID);
    TestAnsSubscriber subscriber;
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber), ERR_OK);

    std::shared_ptr<NotificationNormalContent> implContent = std::make_shared<NotificationNormalContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);
    NotificationRequest req(0);
    req.SetContent(content);
    req.SetLabel(NOTIFICATION_LABEL_0);
    EXPECT_EQ(NotificationHelper::PublishContinuousTaskNotification(req), ERR_OK);
    SleepForFC();
    EventParser eventParser;
    std::list<std::shared_ptr<SubscriberEvent>> events = subscriber.GetEvents();
    eventParser.Parse(events);
    EXPECT_TRUE(eventParser.GetWaitOnConsumed());
    eventParser.SetWaitOnConsumed(false);

    std::vector<sptr<Notification>> notifications;
    EXPECT_EQ(NotificationHelper::GetAllActiveNotifications(notifications), ERR_OK);
    EXPECT_NE((int)notifications.size(), (int)0);
    int32_t id = notifications[0]->GetId();
    MockIPCSkeleton::SetCallingUid(SYSTEM_SERVICE_UID);
    EXPECT_EQ(NotificationHelper::CancelContinuousTaskNotification(NOTIFICATION_LABEL_0, id), ERR_OK);
    EXPECT_EQ(NotificationHelper::GetAllActiveNotifications(notifications), ERR_OK);
    EXPECT_EQ((int)notifications.size(), (int)0);
    SleepForFC();

    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber), ERR_OK);
    events = subscriber.GetEvents();
    eventParser.Parse(events);
    EXPECT_TRUE(eventParser.GetWaitOnCanceled());
    EXPECT_TRUE(eventParser.GetWaitOnCanceledWithSortingMapAndDeleteReason());
    subscriber.ClearEvents();
    SleepForFC();
    MockIPCSkeleton::SetCallingUid(1);
}

HWTEST_F(AnsFWModuleTest, ANS_Interface_MT_PublishContinuousTask_07200, Function | MediumTest | Level1)
{
    MockIPCSkeleton::SetCallingUid(SYSTEM_SERVICE_UID);
    TestAnsSubscriber subscriber;
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber), ERR_OK);

    std::shared_ptr<NotificationNormalContent> implContent = std::make_shared<NotificationNormalContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);
    NotificationRequest req(0);
    req.SetContent(content);
    req.SetLabel(NOTIFICATION_LABEL_0);
    EXPECT_EQ(NotificationHelper::PublishContinuousTaskNotification(req), ERR_OK);
    SleepForFC();
    EventParser eventParser;
    std::list<std::shared_ptr<SubscriberEvent>> events = subscriber.GetEvents();
    eventParser.Parse(events);
    EXPECT_TRUE(eventParser.GetWaitOnConsumed());
    eventParser.SetWaitOnConsumed(false);

    std::vector<sptr<Notification>> notifications;
    EXPECT_EQ(NotificationHelper::GetAllActiveNotifications(notifications), ERR_OK);
    EXPECT_NE((int)notifications.size(), (int)0);
    std::string key = notifications[0]->GetKey().c_str();
    EXPECT_EQ(NotificationHelper::RemoveNotification(key), (int)ERR_OK);
    int32_t id = notifications[0]->GetId();
    MockIPCSkeleton::SetCallingUid(SYSTEM_SERVICE_UID);
    EXPECT_EQ(NotificationHelper::CancelContinuousTaskNotification(NOTIFICATION_LABEL_0, id), ERR_OK);
    EXPECT_EQ(NotificationHelper::GetAllActiveNotifications(notifications), ERR_OK);
    EXPECT_EQ((int)notifications.size(), (int)0);
    SleepForFC();

    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber), ERR_OK);
    events = subscriber.GetEvents();
    eventParser.Parse(events);
    EXPECT_TRUE(eventParser.GetWaitOnCanceled());
    EXPECT_TRUE(eventParser.GetWaitOnCanceledWithSortingMapAndDeleteReason());
    subscriber.ClearEvents();
    SleepForFC();
    MockIPCSkeleton::SetCallingUid(1);
}

HWTEST_F(AnsFWModuleTest, ANS_Interface_MT_PublishContinuousTask_07300, Function | MediumTest | Level1)
{
    MockIPCSkeleton::SetCallingUid(SYSTEM_SERVICE_UID);
    TestAnsSubscriber subscriber;
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber), ERR_OK);

    std::shared_ptr<NotificationNormalContent> implContent = std::make_shared<NotificationNormalContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);
    NotificationRequest req(0);
    req.SetContent(content);
    req.SetLabel(NOTIFICATION_LABEL_0);
    EXPECT_EQ(NotificationHelper::PublishContinuousTaskNotification(req), ERR_OK);
    SleepForFC();
    EventParser eventParser;
    std::list<std::shared_ptr<SubscriberEvent>> events = subscriber.GetEvents();
    eventParser.Parse(events);
    EXPECT_TRUE(eventParser.GetWaitOnConsumed());
    eventParser.SetWaitOnConsumed(false);

    std::vector<sptr<Notification>> notifications;
    EXPECT_EQ(NotificationHelper::GetAllActiveNotifications(notifications), ERR_OK);
    EXPECT_NE((int)notifications.size(), (int)0);
    int32_t id = notifications[0]->GetId();
    EXPECT_EQ(NotificationHelper::CancelNotification(id), (int)ERR_ANS_NOTIFICATION_NOT_EXISTS);
    MockIPCSkeleton::SetCallingUid(SYSTEM_SERVICE_UID);
    EXPECT_EQ(NotificationHelper::CancelContinuousTaskNotification(NOTIFICATION_LABEL_0, id), ERR_OK);
    EXPECT_EQ(NotificationHelper::GetAllActiveNotifications(notifications), ERR_OK);
    EXPECT_EQ((int)notifications.size(), (int)0);
    SleepForFC();

    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber), ERR_OK);
    events = subscriber.GetEvents();
    eventParser.Parse(events);
    EXPECT_TRUE(eventParser.GetWaitOnCanceled());
    EXPECT_TRUE(eventParser.GetWaitOnCanceledWithSortingMapAndDeleteReason());
    subscriber.ClearEvents();
    SleepForFC();
    MockIPCSkeleton::SetCallingUid(1);
}

HWTEST_F(AnsFWModuleTest, ANS_Interface_MT_PublishContinuousTask_07400, Function | MediumTest | Level1)
{
    std::shared_ptr<NotificationNormalContent> implContent = std::make_shared<NotificationNormalContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);
    NotificationRequest req(0);
    req.SetContent(content);
    req.SetLabel(NOTIFICATION_LABEL_0);
    EXPECT_EQ(NotificationHelper::PublishContinuousTaskNotification(req), (int)ERR_ANS_NOT_SYSTEM_SERVICE);
}

HWTEST_F(AnsFWModuleTest, ANS_Interface_MT_PublishContinuousTask_07500, Function | MediumTest | Level1)
{
    TestAnsSubscriber subscriber;
    NotificationSubscribeInfo info;
    info.AddAppName("bundleName");
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber, info), ERR_OK);

    std::shared_ptr<NotificationNormalContent> implContent = std::make_shared<NotificationNormalContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);
    NotificationRequest req0(0);
    req0.SetLabel(NOTIFICATION_LABEL_0);
    req0.SetContent(content);
    EXPECT_EQ(NotificationHelper::PublishNotification(req0), ERR_OK);

    NotificationRequest req1(1);
    req1.SetLabel(NOTIFICATION_LABEL_1);
    req1.SetContent(content);
    EXPECT_EQ(NotificationHelper::PublishNotification(req1), ERR_OK);
    EXPECT_EQ(
        NotificationHelper::CancelContinuousTaskNotification(NOTIFICATION_LABEL_1, 1), (int)ERR_ANS_NOT_SYSTEM_SERVICE);
    EXPECT_EQ(NotificationHelper::CancelAllNotifications(), ERR_OK);
    SleepForFC();
    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber, info), ERR_OK);
    SleepForFC();
    std::list<std::shared_ptr<SubscriberEvent>> events = subscriber.GetEvents();

    EventParser eventParser;
    eventParser.Parse(events);
    EXPECT_TRUE(eventParser.GetWaitOnConsumed());
    EXPECT_TRUE(eventParser.GetWaitOnConsumedWithSortingMap());
    EXPECT_TRUE(eventParser.GetWaitOnCanceled());
    EXPECT_TRUE(eventParser.GetWaitOnCanceledWithSortingMapAndDeleteReason());
    subscriber.ClearEvents();
    SleepForFC();
}

HWTEST_F(AnsFWModuleTest, ANS_Interface_MT_PublishContinuousTask_07600, Function | MediumTest | Level1)
{
    MockIPCSkeleton::SetCallingUid(SYSTEM_SERVICE_UID);
    TestAnsSubscriber subscriber;
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber), ERR_OK);

    std::shared_ptr<NotificationNormalContent> implContent = std::make_shared<NotificationNormalContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);
    NotificationRequest req(0);
    req.SetContent(content);
    req.SetLabel(NOTIFICATION_LABEL_0);
    EXPECT_EQ(NotificationHelper::PublishContinuousTaskNotification(req), ERR_OK);

    SleepForFC();
    EventParser eventParser;
    std::list<std::shared_ptr<SubscriberEvent>> events = subscriber.GetEvents();
    eventParser.Parse(events);
    EXPECT_TRUE(eventParser.GetWaitOnConsumed());

    EXPECT_EQ(eventParser.GetOnConsumedReq()[0]->GetLabel().c_str(), NOTIFICATION_LABEL_0);
    EXPECT_EQ(eventParser.GetOnConsumedReq()[0]->GetId(), 0);
    EXPECT_EQ(eventParser.GetOnConsumedReq()[0]->IsUnremovable(), true);
    EXPECT_EQ(eventParser.GetOnConsumedReq()[0]->GetSourceType(), NotificationConstant::SourceType::TYPE_CONTINUOUS);

    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber), ERR_OK);
    SleepForFC();
    subscriber.ClearEvents();
    MockIPCSkeleton::SetCallingUid(1);
}

HWTEST_F(AnsFWModuleTest, ANS_Interface_MT_PublishContinuousTask_07700, Function | MediumTest | Level1)
{
    MockIPCSkeleton::SetCallingUid(SYSTEM_SERVICE_UID);
    TestAnsSubscriber subscriber;
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber), ERR_OK);

    std::shared_ptr<NotificationNormalContent> implContent = std::make_shared<NotificationNormalContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);
    NotificationRequest req(0);
    req.SetContent(content);
    req.SetLabel(NOTIFICATION_LABEL_0);
    EXPECT_EQ(NotificationHelper::PublishContinuousTaskNotification(req), ERR_OK);
    SleepForFC();
    EventParser eventParser;
    std::list<std::shared_ptr<SubscriberEvent>> events = subscriber.GetEvents();
    eventParser.Parse(events);
    EXPECT_TRUE(eventParser.GetWaitOnConsumed());
    eventParser.SetWaitOnConsumed(false);

    std::vector<sptr<Notification>> notifications;
    EXPECT_EQ(NotificationHelper::GetAllActiveNotifications(notifications), ERR_OK);
    EXPECT_NE((int)notifications.size(), (int)0);
    int32_t id = notifications[0]->GetId();
    EXPECT_EQ(NotificationHelper::CancelAllNotifications(), (int)ERR_OK);
    MockIPCSkeleton::SetCallingUid(SYSTEM_SERVICE_UID);
    EXPECT_EQ(NotificationHelper::GetAllActiveNotifications(notifications), ERR_OK);
    EXPECT_NE((int)notifications.size(), (int)0);
    EXPECT_EQ(NotificationHelper::CancelContinuousTaskNotification(NOTIFICATION_LABEL_0, id), ERR_OK);
    EXPECT_EQ(NotificationHelper::GetAllActiveNotifications(notifications), ERR_OK);
    EXPECT_EQ((int)notifications.size(), (int)0);
    SleepForFC();

    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber), ERR_OK);
    events = subscriber.GetEvents();
    eventParser.Parse(events);
    EXPECT_TRUE(eventParser.GetWaitOnCanceled());
    EXPECT_TRUE(eventParser.GetWaitOnCanceledWithSortingMapAndDeleteReason());
    subscriber.ClearEvents();
    SleepForFC();
    MockIPCSkeleton::SetCallingUid(1);
}

HWTEST_F(AnsFWModuleTest, ANS_Interface_MT_PublishContinuousTask_07800, Function | MediumTest | Level1)
{
    MockIPCSkeleton::SetCallingUid(SYSTEM_SERVICE_UID);
    TestAnsSubscriber subscriber;
    EXPECT_EQ(NotificationHelper::SubscribeNotification(subscriber), ERR_OK);

    std::shared_ptr<NotificationNormalContent> implContent = std::make_shared<NotificationNormalContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);
    NotificationRequest req(0);
    req.SetContent(content);
    req.SetLabel(NOTIFICATION_LABEL_0);
    EXPECT_EQ(NotificationHelper::PublishContinuousTaskNotification(req), ERR_OK);
    SleepForFC();
    EventParser eventParser;
    std::list<std::shared_ptr<SubscriberEvent>> events = subscriber.GetEvents();
    eventParser.Parse(events);
    EXPECT_TRUE(eventParser.GetWaitOnConsumed());
    eventParser.SetWaitOnConsumed(false);

    std::vector<sptr<Notification>> notifications;
    EXPECT_EQ(NotificationHelper::GetAllActiveNotifications(notifications), ERR_OK);
    EXPECT_NE((int)notifications.size(), (int)0);
    int32_t id = notifications[0]->GetId();
    EXPECT_EQ(NotificationHelper::RemoveNotifications(), (int)ERR_OK);
    EXPECT_EQ(NotificationHelper::GetAllActiveNotifications(notifications), ERR_OK);
    EXPECT_NE((int)notifications.size(), (int)0);
    MockIPCSkeleton::SetCallingUid(SYSTEM_SERVICE_UID);
    EXPECT_EQ(NotificationHelper::CancelContinuousTaskNotification(NOTIFICATION_LABEL_0, id), ERR_OK);
    EXPECT_EQ(NotificationHelper::GetAllActiveNotifications(notifications), ERR_OK);
    EXPECT_EQ((int)notifications.size(), (int)0);
    SleepForFC();

    EXPECT_EQ(NotificationHelper::UnSubscribeNotification(subscriber), ERR_OK);
    events = subscriber.GetEvents();
    eventParser.Parse(events);
    EXPECT_TRUE(eventParser.GetWaitOnCanceled());
    EXPECT_TRUE(eventParser.GetWaitOnCanceledWithSortingMapAndDeleteReason());
    subscriber.ClearEvents();
    SleepForFC();
    MockIPCSkeleton::SetCallingUid(1);
}
}  // namespace Notification
}  // namespace OHOS