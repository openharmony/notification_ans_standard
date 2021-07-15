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

#include "notification_subscriber_manager.h"
#include <gtest/gtest.h>
#include <iostream>
#define private public
#include "notification_subscriber.h"
using namespace testing::ext;
namespace OHOS {
namespace Notification {

std::shared_ptr<NotificationSubscriberManager> g_notificationSubscriberManager;
class NotificationSubscriberManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NotificationSubscriberManagerTest::SetUpTestCase()
{
    g_notificationSubscriberManager = NotificationSubscriberManager::GetInstance();
}

void NotificationSubscriberManagerTest::TearDownTestCase()
{
    g_notificationSubscriberManager = nullptr;
}

void NotificationSubscriberManagerTest::SetUp()
{}

void NotificationSubscriberManagerTest::TearDown()
{}
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
    {}
    virtual void OnCanceled(const std::shared_ptr<Notification> &request,
        const std::shared_ptr<NotificationSortingMap> &sortingMap, int deleteReason) override
    {}
    virtual void OnConsumed(const std::shared_ptr<Notification> &request) override
    {}
    virtual void OnConsumed(const std::shared_ptr<Notification> &request,
        const std::shared_ptr<NotificationSortingMap> &sortingMap) override
    {}
};

/**
 * @tc.number    : NotificationSubscriberManagerTest_001
 * @tc.name      : AMS_ANS_NotifyUpdated_0100
 * @tc.desc      : Test NotifyUpdated function
 */
HWTEST_F(NotificationSubscriberManagerTest, NotificationSubscriberManagerTest_001, Function | SmallTest | Level1)
{
    const std::vector<NotificationSorting> sortingList;
    sptr<NotificationSortingMap> map = new NotificationSortingMap(sortingList);
    g_notificationSubscriberManager->NotifyUpdated(map);
}

/**
 * @tc.number    : NotificationSubscriberManagerTest_002
 * @tc.name      : AMS_ANS_NotifyDisturbModeChanged_0100
 * @tc.desc      : Test NotifyDisturbModeChanged function
 */
HWTEST_F(NotificationSubscriberManagerTest, NotificationSubscriberManagerTest_002, Function | SmallTest | Level1)
{
    NotificationConstant::DisturbMode mode;
    g_notificationSubscriberManager->NotifyDisturbModeChanged(mode);
}

/**
 * @tc.number    : NotificationSubscriberManagerTest_003
 * @tc.name      : AMS_ANS_NotifyConsumed_0100
 * @tc.desc      : Test NotifyConsumed function
 */
HWTEST_F(NotificationSubscriberManagerTest, NotificationSubscriberManagerTest_003, Function | SmallTest | Level1)
{
    std::vector<NotificationSorting> sortingList;
    sptr<NotificationRequest> request = new NotificationRequest();
    sptr<Notification> notification = new Notification(request);
    sptr<NotificationSortingMap> notificationMap = new NotificationSortingMap(sortingList);
    g_notificationSubscriberManager->NotifyConsumed(notification, notificationMap);
}

/**
 * @tc.number    : NotificationSubscriberManagerTest_004
 * @tc.name      : AMS_ANS_NotifyCanceled_0100
 * @tc.desc      : Test NotifyCanceled function
 */
HWTEST_F(NotificationSubscriberManagerTest, NotificationSubscriberManagerTest_004, Function | SmallTest | Level1)
{
    std::vector<NotificationSorting> sortingList;
    sptr<NotificationRequest> request = new NotificationRequest();
    sptr<Notification> notification = new Notification(request);
    sptr<NotificationSortingMap> notificationMap = new NotificationSortingMap(sortingList);
    int deleteReason = 0;
    g_notificationSubscriberManager->NotifyCanceled(notification, notificationMap, deleteReason);
}

/**
 * @tc.number    : NotificationSubscriberManagerTest_005
 * @tc.name      : AMS_ANS_AddSubscriber_0100
 * @tc.desc      : Test AddSubscriber function
 */
HWTEST_F(NotificationSubscriberManagerTest, NotificationSubscriberManagerTest_005, Function | SmallTest | Level1)
{
    auto subscriber = new TestAnsSubscriber();
    sptr<NotificationSubscribeInfo> info = new NotificationSubscribeInfo();
    EXPECT_EQ(g_notificationSubscriberManager->AddSubscriber(subscriber->GetImpl(), info), (int)ERR_OK);
}

/**
 * @tc.number    : NotificationSubscriberManagerTest_006
 * @tc.name      : AMS_ANS_RemoveSubscriber_0100
 * @tc.desc      : Test RemoveSubscriber function
 */
HWTEST_F(NotificationSubscriberManagerTest, NotificationSubscriberManagerTest_006, Function | SmallTest | Level1)
{
    auto subscriber = new TestAnsSubscriber();
    sptr<NotificationSubscribeInfo> info = new NotificationSubscribeInfo();
    EXPECT_EQ(g_notificationSubscriberManager->RemoveSubscriber(subscriber->GetImpl(), info), (int)ERR_OK);
}
}  // namespace Notification
}  // namespace OHOS