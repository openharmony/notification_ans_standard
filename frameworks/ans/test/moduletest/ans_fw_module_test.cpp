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

#include <gtest/gtest.h>
#include <functional>

#include "ans_inner_errors.h"
#include "ans_manager_proxy.h"
#include "advanced_notification_service.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "notification_content.h"
#include "notification_helper.h"
#include "notification_long_text_content.h"
#include "notification_subscriber.h"
#include "system_ability_definition.h"

using namespace testing::ext;
namespace OHOS {
namespace Notification {

const int32_t USLEEP_TIME = 10000;
bool callBackFunReceived = false;
bool OnConsumedReceived = false;
bool OnCanceledReceived = false;
class TestAnsSubscriber : public NotificationSubscriber {
public:
    virtual void OnSubscribeResult(NotificationConstant::SubscribeResult result) override
    {}
    virtual void OnUnsubscribeResult(NotificationConstant::SubscribeResult result) override
    {}
    virtual void OnDied() override
    {}
    virtual void OnUpdate(const std::shared_ptr<NotificationSortingMap> &sortingMap) override
    {}
    virtual void OnDisturbModeChanged(int disturbMode) override
    {}
    virtual void OnCanceled(const std::shared_ptr<Notification> &request) override
    {
        OnCanceledReceived = true;
    }
    virtual void OnCanceled(const std::shared_ptr<Notification> &request,
        const std::shared_ptr<NotificationSortingMap> &sortingMap, int deleteReason) override
    {
        callBackFunReceived = false;
    }
    virtual void OnConsumed(const std::shared_ptr<Notification> &request) override
    {
        OnConsumedReceived = true;
    }
    virtual void OnConsumed(const std::shared_ptr<Notification> &request,
        const std::shared_ptr<NotificationSortingMap> &sortingMap) override
    {
        callBackFunReceived = true;
    }
};

class AnsFWModuleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

sptr<ISystemAbilityManager> systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
void AnsFWModuleTest::SetUpTestCase()
{
    sptr<AdvancedNotificationService> service = OHOS::Notification::AdvancedNotificationService::GetInstance();
    OHOS::ISystemAbilityManager::SAExtraProp saExtraProp;
    systemAbilityManager->AddSystemAbility(OHOS::ADVANCED_NOTIFICATION_SERVICE_ABILITY_ID, service, saExtraProp);
}

void AnsFWModuleTest::TearDownTestCase()
{}

void AnsFWModuleTest::SetUp()
{
    callBackFunReceived = false;
    OnConsumedReceived = false;
    OnCanceledReceived = false;
}

void AnsFWModuleTest::TearDown()
{
    NotificationHelper::CancelAllNotifications();
    sleep(1);
}

/**
 * @tc.number    : ANS_FW_MT_RemoveNotification_00100
 * @tc.name      :
 * @tc.desc      : The subscriber remove all notifications from the notification queue.
 * The subscriber receives the notification of the deletion.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_RemoveNotification_00100, Function | MediumTest | Level1)
{
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));
    

    std::shared_ptr<NotificationLongTextContent> implContent = std::make_shared<NotificationLongTextContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);

    std::string label = "Label";
    NotificationRequest req(0);
    req.SetLabel(label);
    req.SetContent(content);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req));
    EXPECT_EQ(OnConsumedReceived, true);

    EXPECT_EQ(0, NotificationHelper::RemoveNotifications());
    usleep(USLEEP_TIME);
    EXPECT_EQ(OnCanceledReceived, true);
    usleep(USLEEP_TIME);
    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber));
}

/**
 * @tc.number    : ANS_FW_MT_RemoveNotification_00200
 * @tc.name      :
 * @tc.desc      : The subscriber remove the notification from the notification queue according to the specified Key.
 * The subscriber receives the notification of the deletion.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_RemoveNotification_00200, Function | MediumTest | Level1)
{
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));

    std::shared_ptr<NotificationLongTextContent> implContent = std::make_shared<NotificationLongTextContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);

    std::string label = "Label";
    NotificationRequest req(0);
    req.SetLabel(label);
    req.SetContent(content);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req));
    EXPECT_EQ(OnConsumedReceived, true);

    std::vector<sptr<OHOS::Notification::Notification>> notifications;
    EXPECT_EQ(0, NotificationHelper::GetAllActiveNotifications(notifications));
    std::string key = notifications[0]->GetKey().c_str();

    EXPECT_EQ(0, NotificationHelper::RemoveNotification(key));
    usleep(USLEEP_TIME);
    EXPECT_EQ(OnCanceledReceived, true);
    usleep(USLEEP_TIME);
    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber));
}

/**
 * @tc.number    : ANS_FW_MT_RemoveNotification_00300
 * @tc.name      :
 * @tc.desc      : An empty key was used when deleting the notification.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_RemoveNotification_00300, Function | MediumTest | Level1)
{
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));

    std::shared_ptr<NotificationLongTextContent> implContent = std::make_shared<NotificationLongTextContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);

    std::string label = "Label";
    NotificationRequest req(0);
    req.SetLabel(label);
    req.SetContent(content);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req));
    EXPECT_EQ(OnConsumedReceived, true);

    std::vector<sptr<OHOS::Notification::Notification>> notifications;
    EXPECT_EQ(0, NotificationHelper::GetAllActiveNotifications(notifications));
    std::string key = notifications[0]->GetKey().c_str();

    EXPECT_EQ((int)ERR_ANS_INVALID_PARAM, (int)NotificationHelper::RemoveNotification(std::string()));
    usleep(USLEEP_TIME);
    EXPECT_EQ(OnCanceledReceived, false);
    usleep(USLEEP_TIME);
    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber));
}

/**
 * @tc.number    : ANS_FW_MT_RemoveNotification_00400
 * @tc.name      :
 * @tc.desc      : A non-existent key was used when deleting the notification.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_RemoveNotification_00400, Function | MediumTest | Level1)
{
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));

    std::shared_ptr<NotificationLongTextContent> implContent = std::make_shared<NotificationLongTextContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);

    std::string label = "Label";
    NotificationRequest req(0);
    req.SetLabel(label);
    req.SetContent(content);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req));
    EXPECT_EQ(OnConsumedReceived, true);

    std::vector<sptr<OHOS::Notification::Notification>> notifications;
    EXPECT_EQ(0, NotificationHelper::GetAllActiveNotifications(notifications));
    std::string key = "A non-existent key";

    EXPECT_EQ((int)ERR_ANS_NOTIFICATION_NOT_EXISTS, (int)NotificationHelper::RemoveNotification(key));
    usleep(USLEEP_TIME);
    EXPECT_EQ(OnCanceledReceived, false);
    usleep(USLEEP_TIME);
    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber));
}

/**
 * @tc.number    : ANS_FW_MT_RemoveNotification_00500
 * @tc.name      :
 * @tc.desc      : Use the same key to delete repeatedly.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_RemoveNotification_00500, Function | MediumTest | Level1)
{
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));

    std::shared_ptr<NotificationLongTextContent> implContent = std::make_shared<NotificationLongTextContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);

    std::string label = "Label";
    NotificationRequest req(0);
    req.SetLabel(label);
    req.SetContent(content);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req));
    EXPECT_EQ(OnConsumedReceived, true);

    std::vector<sptr<OHOS::Notification::Notification>> notifications;
    EXPECT_EQ(0, NotificationHelper::GetAllActiveNotifications(notifications));
    std::string key = notifications[0]->GetKey().c_str();

    EXPECT_EQ((int)ERR_OK, (int)NotificationHelper::RemoveNotification(key));
    usleep(USLEEP_TIME);
    EXPECT_EQ(OnCanceledReceived, true);
    OnCanceledReceived = false;
    EXPECT_EQ((int)ERR_ANS_NOTIFICATION_NOT_EXISTS, (int)NotificationHelper::RemoveNotification(key));
    usleep(USLEEP_TIME);
    EXPECT_EQ(OnCanceledReceived, false);
    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber));
}

/**
 * @tc.number    : ANS_FW_MT_RemoveNotification_00600
 * @tc.name      :
 * @tc.desc      : Delete an Unremovable notification.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_RemoveNotification_00600, Function | MediumTest | Level1)
{
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));

    std::shared_ptr<NotificationLongTextContent> implContent = std::make_shared<NotificationLongTextContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);

    std::string label = "Label";
    NotificationRequest req(0);
    req.SetLabel(label);
    req.SetContent(content);
    req.SetUnremovable(false);  // TODO should be true
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req));
    EXPECT_EQ(OnConsumedReceived, true);

    std::vector<sptr<OHOS::Notification::Notification>> notifications;
    EXPECT_EQ(0, NotificationHelper::GetAllActiveNotifications(notifications));
    std::string key = notifications[0]->GetKey().c_str();

    EXPECT_EQ((int)ERR_ANS_NOTIFICATION_IS_UNREMOVABLE, (int)NotificationHelper::RemoveNotification(key));
    usleep(USLEEP_TIME);
    EXPECT_EQ(OnCanceledReceived, false);
    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber));
}

/**
 * @tc.number    : ANS_FW_MT_RemoveNotification_00700
 * @tc.name      :
 * @tc.desc      : Delete all notifications including Unremovable notifications.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_RemoveNotification_00700, Function | MediumTest | Level1)
{
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));

    std::shared_ptr<NotificationLongTextContent> implContent = std::make_shared<NotificationLongTextContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);

    std::string label1 = "Label1";
    NotificationRequest req1(0);
    req1.SetLabel(label1);
    req1.SetContent(content);
    req1.SetUnremovable(false);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req1));
    EXPECT_EQ(OnConsumedReceived, true);
    OnConsumedReceived = false;

    std::string label2 = "Label2";
    NotificationRequest req2(0);
    req2.SetLabel(label2);
    req2.SetContent(content);
    req2.SetUnremovable(false);  // TODO should be true
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req2));
    EXPECT_EQ(OnConsumedReceived, true);

    std::vector<sptr<OHOS::Notification::Notification>> notificationsBefor;
    EXPECT_EQ(0, NotificationHelper::GetAllActiveNotifications(notificationsBefor));
    EXPECT_EQ(2, (int)notificationsBefor.size());

    EXPECT_EQ((int)ERR_OK, (int)NotificationHelper::RemoveNotifications());
    usleep(USLEEP_TIME);
    EXPECT_EQ(OnCanceledReceived, true);

    std::vector<sptr<OHOS::Notification::Notification>> notificationsAfter;
    EXPECT_EQ(0, NotificationHelper::GetAllActiveNotifications(notificationsAfter));
    // EXPECT_EQ(1, (int)notificationsAfter.size()); TODO
    // EXPECT_EQ("Label2", notificationsAfter[0]->GetLabel()); TODO

    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber));
}

/**
 * @tc.number    : ANS_FW_MT_RemoveNotification_00800
 * @tc.name      :
 * @tc.desc      : Delete notification based on bundle.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_RemoveNotification_00800, Function | MediumTest | Level1)
{
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));

    std::shared_ptr<NotificationLongTextContent> implContent = std::make_shared<NotificationLongTextContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);

    std::string label1 = "Label1";
    NotificationRequest req1(0);
    req1.SetLabel(label1);
    req1.SetContent(content);
    req1.SetUnremovable(false);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req1));
    EXPECT_EQ(OnConsumedReceived, true);
    OnConsumedReceived = false;

    std::vector<sptr<OHOS::Notification::Notification>> notificationsBefor;
    EXPECT_EQ(0, NotificationHelper::GetAllActiveNotifications(notificationsBefor));
    EXPECT_EQ(1, (int)notificationsBefor.size());

    EXPECT_EQ((int)ERR_OK, (int)NotificationHelper::RemoveNotifications("bundleName"));
    usleep(USLEEP_TIME);
    EXPECT_EQ(OnCanceledReceived, false);

    std::vector<sptr<OHOS::Notification::Notification>> notificationsAfter;
    EXPECT_EQ(0, NotificationHelper::GetAllActiveNotifications(notificationsAfter));
    EXPECT_EQ(0, (int)notificationsAfter.size());

    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber));
}

/**
 * @tc.number    : ANS_FW_MT_RemoveNotification_00900
 * @tc.name      :
 * @tc.desc      : Delete notification based on A non-existent bundle.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_RemoveNotification_00900, Function | MediumTest | Level1)
{
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));

    std::shared_ptr<NotificationLongTextContent> implContent = std::make_shared<NotificationLongTextContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);

    std::string label1 = "Label1";
    NotificationRequest req1(0);
    req1.SetLabel(label1);
    req1.SetContent(content);
    req1.SetUnremovable(false);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req1));
    EXPECT_EQ(OnConsumedReceived, true);
    OnConsumedReceived = false;

    std::vector<sptr<OHOS::Notification::Notification>> notificationsBefor;
    EXPECT_EQ(0, NotificationHelper::GetAllActiveNotifications(notificationsBefor));
    EXPECT_EQ(1, (int)notificationsBefor.size());

    std::string bundleName = "A non-existent bundle";
    EXPECT_EQ((int)ERR_OK, (int)NotificationHelper::RemoveNotifications(bundleName));
    usleep(USLEEP_TIME);
    EXPECT_EQ(OnCanceledReceived, false);

    std::vector<sptr<OHOS::Notification::Notification>> notificationsAfter;
    EXPECT_EQ(0, NotificationHelper::GetAllActiveNotifications(notificationsAfter));
    EXPECT_EQ(1, (int)notificationsAfter.size());

    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber));
}

/**
 * @tc.number    : ANS_FW_MT_GetActiveNotificationNums_00100
 * @tc.name      :
 * @tc.desc      : Get the number of active notifications for the current application.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_GetActiveNotificationNums_00100, Function | MediumTest | Level1)
{
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));

    std::shared_ptr<NotificationLongTextContent> implContent = std::make_shared<NotificationLongTextContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);

    int countBefor = 0;
    EXPECT_EQ((int)ERR_OK, NotificationHelper::GetActiveNotificationNums(countBefor));
    EXPECT_EQ(0, countBefor);

    std::string label1 = "Label1";
    NotificationRequest req1(0);
    req1.SetLabel(label1);
    req1.SetContent(content);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req1));
    EXPECT_EQ(OnConsumedReceived, true);
    OnConsumedReceived = false;

    std::string label2 = "Label2";
    NotificationRequest req2(0);
    req2.SetLabel(label2);
    req2.SetContent(content);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req2));
    EXPECT_EQ(OnConsumedReceived, true);
    OnConsumedReceived = false;

    std::string label3 = "Label3";
    NotificationRequest req3(0);
    req3.SetLabel(label3);
    req3.SetContent(content);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req3));
    EXPECT_EQ(OnConsumedReceived, true);
    OnConsumedReceived = false;

    int countAfter = 0;
    EXPECT_EQ((int)ERR_OK, NotificationHelper::GetActiveNotificationNums(countAfter));
    EXPECT_EQ(3, countAfter);

    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber));
}

/**
 * @tc.number    : ANS_FW_MT_GetActiveNotificationNums_00200
 * @tc.name      :
 * @tc.desc      : Get the number of active notifications for the current application.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_GetActiveNotificationNums_00200, Function | MediumTest | Level1)
{
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));

    std::shared_ptr<NotificationLongTextContent> implContent = std::make_shared<NotificationLongTextContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);

    int countBefor = 0;
    EXPECT_EQ((int)ERR_OK, NotificationHelper::GetActiveNotificationNums(countBefor));
    EXPECT_EQ(0, countBefor);

    std::string label1 = "Label1";
    NotificationRequest req1(0);
    req1.SetLabel(label1);
    req1.SetContent(content);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req1));
    EXPECT_EQ(OnConsumedReceived, true);
    OnConsumedReceived = false;

    std::string label2 = "Label2";
    NotificationRequest req2(0);
    req2.SetLabel(label2);
    req2.SetContent(content);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req2));
    EXPECT_EQ(OnConsumedReceived, true);
    OnConsumedReceived = false;

    std::string label3 = "Label3";
    NotificationRequest req3(0);
    req3.SetLabel(label3);
    req3.SetContent(content);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req3));
    EXPECT_EQ(OnConsumedReceived, true);
    OnConsumedReceived = false;

    int countBefore = 0;
    EXPECT_EQ((int)ERR_OK, NotificationHelper::GetActiveNotificationNums(countBefore));
    EXPECT_EQ(3, countBefore);

    EXPECT_EQ((int)ERR_OK, (int)NotificationHelper::RemoveNotifications());
    usleep(USLEEP_TIME);
    EXPECT_EQ(OnCanceledReceived, true);

    int countAfter = 0;
    EXPECT_EQ((int)ERR_OK, NotificationHelper::GetActiveNotificationNums(countAfter));
    EXPECT_EQ(0, countAfter);

    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber));
}

/**
 * @tc.number    : ANS_FW_MT_GetActiveNotifications_00100
 * @tc.name      :
 * @tc.desc      : Get the active notifications for the current application.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_GetActiveNotifications_00100, Function | MediumTest | Level1)
{
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));

    std::shared_ptr<NotificationLongTextContent> implContent = std::make_shared<NotificationLongTextContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);

    int countBefor = 0;
    EXPECT_EQ((int)ERR_OK, NotificationHelper::GetActiveNotificationNums(countBefor));
    EXPECT_EQ(0, countBefor);

    std::string label1 = "Label1";
    NotificationRequest req1(0);
    req1.SetLabel(label1);
    req1.SetContent(content);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req1));
    EXPECT_EQ(OnConsumedReceived, true);
    OnConsumedReceived = false;

    std::string label2 = "Label2";
    NotificationRequest req2(0);
    req2.SetLabel(label2);
    req2.SetContent(content);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req2));
    EXPECT_EQ(OnConsumedReceived, true);
    OnConsumedReceived = false;

    std::string label3 = "Label3";
    NotificationRequest req3(0);
    req3.SetLabel(label3);
    req3.SetContent(content);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req3));
    EXPECT_EQ(OnConsumedReceived, true);
    OnConsumedReceived = false;

    std::vector<sptr<NotificationRequest>> requests;
    EXPECT_EQ((int)ERR_OK, NotificationHelper::GetActiveNotifications(requests));
    EXPECT_EQ("Label1", requests[0]->GetLabel().c_str());
    EXPECT_EQ("Label2", requests[1]->GetLabel().c_str());
    EXPECT_EQ("Label3", requests[2]->GetLabel().c_str());

    EXPECT_EQ((int)ERR_OK, (int)NotificationHelper::RemoveNotifications());
    usleep(USLEEP_TIME);
    EXPECT_EQ(OnCanceledReceived, true);

    std::vector<sptr<NotificationRequest>> requestsRemoved;
    EXPECT_EQ((int)ERR_OK, NotificationHelper::GetActiveNotifications(requestsRemoved));
    EXPECT_EQ(0, (int)requestsRemoved.size());

    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber));
}

/**
 * @tc.number    : ANS_FW_MT_GetActiveNotifications_00200
 * @tc.name      :
 * @tc.desc      : Get the active notifications for the current application.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_GetActiveNotifications_00200, Function | MediumTest | Level1)
{
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));

    NotificationRequest req1(0);
    req1.SetNotificationId(1);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req1));
    EXPECT_EQ(OnConsumedReceived, true);
    OnConsumedReceived = false;

    NotificationRequest req2(0);
    req2.SetNotificationId(2);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req2));
    EXPECT_EQ(OnConsumedReceived, true);
    OnConsumedReceived = false;

    NotificationRequest req3(0);
    req3.SetNotificationId(3);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req3));
    EXPECT_EQ(OnConsumedReceived, true);
    OnConsumedReceived = false;

    EXPECT_EQ((int)ERR_OK, (int)NotificationHelper::RemoveNotifications());
    usleep(USLEEP_TIME);
    EXPECT_EQ(OnCanceledReceived, true);

    std::vector<sptr<NotificationRequest>> notifications;
    EXPECT_EQ((int)ERR_OK, NotificationHelper::GetActiveNotifications(notifications));
    EXPECT_EQ(nullptr, notifications[0]);
    NotificationHelper::GetActiveNotifications(notifications);
    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber));
}

/**
 * @tc.number    : ANS_FW_MT_GetAllActiveNotifications_00100
 * @tc.name      :
 * @tc.desc      : Get all of active notifications for the current application.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_GetAllActiveNotifications_00100, Function | MediumTest | Level1)
{
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));

    std::shared_ptr<NotificationLongTextContent> implContent = std::make_shared<NotificationLongTextContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);

    std::string label1 = "Label1";
    NotificationRequest req1(0);
    req1.SetLabel(label1);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req1));
    EXPECT_EQ(OnConsumedReceived, true);
    OnConsumedReceived = false;

    std::string label2 = "Label2";
    NotificationRequest req2(0);
    req2.SetLabel(label2);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req2));
    EXPECT_EQ(OnConsumedReceived, true);
    OnConsumedReceived = false;

    std::string label3 = "Label3";
    NotificationRequest req3(0);
    req3.SetLabel(label3);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req3));
    EXPECT_EQ(OnConsumedReceived, true);
    OnConsumedReceived = false;

    std::vector<sptr<Notification>> notifications;
    EXPECT_EQ((int)ERR_OK, NotificationHelper::GetAllActiveNotifications(notifications));
    EXPECT_EQ(label1, notifications[0]->GetLabel());
    EXPECT_EQ(label2, notifications[1]->GetLabel());
    EXPECT_EQ(label3, notifications[2]->GetLabel());

    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber));
}

/**
 * @tc.number    : ANS_FW_MT_GetAllActiveNotifications_00200
 * @tc.name      :
 * @tc.desc      : Get  all of active notifications for the current application.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_GetAllActiveNotifications_00200, Function | MediumTest | Level1)
{
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));
    

    std::shared_ptr<NotificationLongTextContent> implContent = std::make_shared<NotificationLongTextContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);

    std::string label1 = "Label1";
    NotificationRequest req1(0);
    req1.SetLabel(label1);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req1));
    EXPECT_EQ(OnConsumedReceived, true);
    OnConsumedReceived = false;

    std::string label2 = "Label2";
    NotificationRequest req2(0);
    req2.SetLabel(label2);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req2));
    EXPECT_EQ(OnConsumedReceived, true);
    OnConsumedReceived = false;

    std::string label3 = "Label3";
    NotificationRequest req3(0);
    req3.SetLabel(label3);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req3));
    EXPECT_EQ(OnConsumedReceived, true);
    OnConsumedReceived = false;

    EXPECT_EQ((int)ERR_OK, (int)NotificationHelper::RemoveNotifications());
    usleep(USLEEP_TIME);
    EXPECT_EQ(OnCanceledReceived, true);

    std::vector<sptr<Notification>> notifications;
    EXPECT_EQ((int)ERR_OK, NotificationHelper::GetAllActiveNotifications(notifications));
    EXPECT_EQ(nullptr, notifications[0]);

    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber));
}

/**
 * @tc.number    : ANS_FW_MT_GetAllActiveNotifications_key_00100
 * @tc.name      :
 * @tc.desc      : Get the active notifications related by key for the current application.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_GetAllActiveNotifications_key_00100, Function | MediumTest | Level1)
{
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));
    

    std::vector<std::string> key{"key_type1", "key_type2"};

    std::string label1 = "label1";
    std::string label2 = "label2";
    std::string label3 = "label3";

    NotificationRequest req1(0);
    req1.SetSortingKey(key[0]);
    req1.SetLabel(label1);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req1));
    EXPECT_EQ(OnConsumedReceived, true);
    OnConsumedReceived = false;

    NotificationRequest req2(0);
    req2.SetSortingKey(key[0]);
    req2.SetLabel(label2);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req1));
    EXPECT_EQ(OnConsumedReceived, true);
    OnConsumedReceived = false;

    NotificationRequest req3(0);
    req3.SetSortingKey(key[1]);
    req3.SetLabel(label3);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req1));
    EXPECT_EQ(OnConsumedReceived, true);
    OnConsumedReceived = false;

    std::vector<sptr<Notification>> notifications;
    EXPECT_EQ((int)ERR_OK, NotificationHelper::GetAllActiveNotifications(key, notifications));
    EXPECT_EQ(label1, notifications[0]->GetLabel());
    EXPECT_EQ(label2, notifications[1]->GetLabel());
    EXPECT_EQ(label3, notifications[2]->GetLabel());

    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber));
}

/**
 * @tc.number    : ANS_FW_MT_UnSubscribeNotification_00100
 * @tc.name      :
 * @tc.desc      : Cancel a subscribe.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_UnSubscribeNotification_00100, Function | MediumTest | Level1)
{
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));
    

    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber, info));
}

/**
 * @tc.number    : ANS_FW_MT_Subscribe_00100
 * @tc.name      :
 * @tc.desc      : produce a subscribe
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_WantAgent_00100, Function | MediumTest | Level1)
{
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));
    

    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber, info));
}

/**
 * @tc.number    : ANS_FW_MT_DeleteSlots_00100
 * @tc.name      :
 * @tc.desc      : Delete slots.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_DeleteSlots_00100, Function | MediumTest | Level1)
{
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));

    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber, info));
}

/**
 * @tc.number    : ANS_FW_MT_SlotGroupId_00100
 * @tc.name      :
 * @tc.desc      : Delete all Notifications of the app.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_SlotGroupId_00100, Function | MediumTest | Level1)
{
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    std::string AppName = "bundleName";
    info.AddAppName(AppName);
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));

    EXPECT_EQ(0, NotificationHelper::RemoveNotifications(AppName));

    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber, info));

    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber, info));
}

/**
 * @tc.number    : ANS_FW_MT_RemoveNotifications_00100
 * @tc.name      :
 * @tc.desc      : Delete all Notifications of the app.
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_RemoveNotifications_00100, Function | MediumTest | Level1)
{
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    std::string AppName = "bundleName";
    info.AddAppName(AppName);
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));

    EXPECT_EQ(0, NotificationHelper::RemoveNotifications(AppName));

    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber, info));
}

/**
 * @tc.number    : ANS_FW_MT_SoundSlotNotification_00100
 * @tc.name      :
 * @tc.desc      : App published a sound notification .
 */
HWTEST_F(AnsFWModuleTest, ANS_FW_MT_SoundSlotNotification_00100, Function | MediumTest | Level1)
{
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));

    std::string str{"test"};
    std::shared_ptr<NotificationLongTextContent> implContent = std::make_shared<NotificationLongTextContent>(str);
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);

    NotificationRequest req(0);
    req.SetContent(content);
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req));
    EXPECT_EQ(OnConsumedReceived, true);

    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber, info));
}
}  // namespace Notification
}  // namespace OHOS