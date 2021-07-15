#include <gtest/gtest.h>
#include "advanced_notification_service.h"
#include "notification_preferences.h"
#include <functional>
#define private public
#include "notification_subscriber.h"

using namespace testing::ext;
namespace OHOS {
namespace Notification {

const int32_t USLEEP_TIME = 300;
sptr<AdvancedNotificationService> g_advancedNotificationService;
bool passed = false;
class TestAnsSubscriber : public NotificationSubscriber
{
public:
    virtual void OnSubscribeResult(NotificationConstant::SubscribeResult result) override {}
    virtual void OnUnsubscribeResult(NotificationConstant::SubscribeResult result) override {}
    virtual void OnDied() override {}
    virtual void OnUpdate(const std::shared_ptr<NotificationSortingMap> &sortingMap) override {}
    virtual void OnDisturbModeChanged(int disturbMode) override {}
    virtual void OnCanceled(const std::shared_ptr<Notification> &request) override {}
    virtual void OnCanceled(const std::shared_ptr<Notification> &request,
                            const std::shared_ptr<NotificationSortingMap> &sortingMap, int deleteReason) override {}
    virtual void OnConsumed(const std::shared_ptr<Notification> &request) override
    {
        passed = true;
    }
    virtual void OnConsumed(const std::shared_ptr<Notification> &request,
                            const std::shared_ptr<NotificationSortingMap> &sortingMap) override
    {
        passed = true;
    }
};

class AnsModuleTest : public testing::Test
{
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AnsModuleTest::SetUpTestCase()
{
    g_advancedNotificationService = AdvancedNotificationService::GetInstance();
}

void AnsModuleTest::TearDownTestCase()
{
    sptr<AdvancedNotificationService> g_advancedNotificationService = nullptr;
}

void AnsModuleTest::SetUp()
{
    NotificationPreferences::GetInstance().ClearNotificationInRestoreFactorySettings();
}

void AnsModuleTest::TearDown()
{
    NotificationPreferences::GetInstance().ClearNotificationInRestoreFactorySettings();
}

void TestAddSlots()
{
    std::vector<sptr<NotificationSlot>> slots;
    sptr<NotificationSlot> slot0 = new NotificationSlot(NotificationConstant::SlotType::SOCIAL_COMMUNICATION);
    slots.push_back(slot0);
    g_advancedNotificationService->AddSlots(slots);
}

/**
 * @tc.number    : AnsModuleTest_001
 * @tc.name      : AMS_ANS_Subscribe_Publish_0100
 * @tc.desc      : Test the functions of subscribing, publishing, and canceling
 */
HWTEST_F(AnsModuleTest, AnsModuleTest_001, Function | SmallTest | Level1)
{
    TestAddSlots();
    std::string label = "testLabel";
    auto subscriber = new TestAnsSubscriber();
    sptr<NotificationRequest> req = new NotificationRequest(2);
    sptr<NotificationRequest> req1 = new NotificationRequest(1);
    req1->SetLabel(label);
    sptr<NotificationSubscribeInfo> info = new NotificationSubscribeInfo();
    info->AddAppName("bundleName");
    EXPECT_EQ((int)g_advancedNotificationService->Subscribe(subscriber->GetImpl(), info), (int)ERR_OK);
    EXPECT_EQ((int)g_advancedNotificationService->Publish(label, req), (int)ERR_OK);
    EXPECT_EQ((int)g_advancedNotificationService->Publish(label, req1), (int)ERR_OK);
    EXPECT_EQ((int)g_advancedNotificationService->Cancel(1, label), (int)ERR_OK);
    usleep(USLEEP_TIME);
    EXPECT_TRUE(passed);
    g_advancedNotificationService->CancelAll();
    EXPECT_EQ((int)g_advancedNotificationService->Unsubscribe(subscriber->GetImpl(), info), (int)ERR_OK);
    passed = false;
}

/**
 * @tc.number    : AnsModuleTest_002
 * @tc.name      : AMS_ANS_GetActiveNotifications_0100
 * @tc.desc      : Test the function of getting notifications and getting all notifications
 */
HWTEST_F(AnsModuleTest, AnsModuleTest_002, Function | SmallTest | Level1)
{
    TestAddSlots();
    std::string label = "testLabel";
    sptr<NotificationRequest> req = new NotificationRequest(0);
    req->SetLabel(label);
    sptr<NotificationRequest> req1 = new NotificationRequest(1);
    req1->SetLabel(label);
    sptr<NotificationRequest> req2 = new NotificationRequest(2);
    req2->SetLabel("testLabel1");
    sptr<NotificationSubscribeInfo> info = new NotificationSubscribeInfo();
    info->AddAppName("bundleName");
    std::vector<sptr<NotificationRequest>> notificationsReqs;
    std::vector<sptr<Notification>> notifications;
    EXPECT_EQ((int)g_advancedNotificationService->Publish(label, req), (int)ERR_OK);
    EXPECT_EQ((int)g_advancedNotificationService->Publish(label, req1), (int)ERR_OK);
    EXPECT_EQ((int)g_advancedNotificationService->Publish("testLabel1", req2), (int)ERR_OK);
    EXPECT_EQ((int)g_advancedNotificationService->GetActiveNotifications(notificationsReqs), (int)ERR_OK);
    int num;
    g_advancedNotificationService->GetActiveNotificationNums(num);
    EXPECT_EQ((int)num, (int)3);
    EXPECT_EQ((int)g_advancedNotificationService->Cancel(2, "testLabel1"), (int)ERR_OK);
    EXPECT_EQ((int)g_advancedNotificationService->GetAllActiveNotifications(notifications), (int)ERR_OK);
    EXPECT_EQ((int)notifications.size(), (int)2);
    EXPECT_EQ((int)g_advancedNotificationService->CancelAll(), (int)ERR_OK);
}
}
}