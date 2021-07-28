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

#include <functional>
#include <gtest/gtest.h>

#define private public
#include "advanced_notification_service.h"
#include "mock_ipc_skeleton.h"
#include "notification_preferences.h"
#include "notification_subscriber.h"

#include "ans_inner_errors.h"

using namespace testing::ext;
namespace OHOS {
namespace Notification {

class AdvancedNotificationServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
sptr<AdvancedNotificationService> g_advancedNotificationService;
void AdvancedNotificationServiceTest::SetUpTestCase()
{
    g_advancedNotificationService = AdvancedNotificationService::GetInstance();
}

void AdvancedNotificationServiceTest::TearDownTestCase()
{
    sptr<AdvancedNotificationService> g_advancedNotificationService = nullptr;
}

void AdvancedNotificationServiceTest::SetUp()
{
    NotificationPreferences::GetInstance().ClearNotificationInRestoreFactorySettings();
    IPCSkeleton::SetCallingUid(1);
    g_advancedNotificationService->CancelAll();
}

void AdvancedNotificationServiceTest::TearDown()
{
    NotificationPreferences::GetInstance().ClearNotificationInRestoreFactorySettings();
    sleep(1);
}

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

void TestAddSlots(NotificationConstant::SlotType type)
{
    std::vector<sptr<NotificationSlot>> slots;
    sptr<NotificationSlot> slot = new NotificationSlot(type);
    slots.push_back(slot);
    g_advancedNotificationService->AddSlots(slots);
}

void TestAddSlotGroups()
{
    std::vector<sptr<NotificationSlotGroup>> groups;
    sptr<NotificationSlotGroup> group = new NotificationSlotGroup("id0", "name0");
    groups.push_back(group);
    g_advancedNotificationService->AddSlotGroups(groups);
}
/**
 * @tc.number    : AMS_ANS_Publish_00100
 * @tc.name      : ANSPublish00100
 * @tc.desc      : When there is no such notification in the notification queue,
 * the test publish interface can successfully publish a normal text type notification.
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_00100, Function | SmallTest | Level1)
{
    sptr<NotificationRequest> req = new NotificationRequest();
    req->SetSlotType(NotificationConstant::SlotType::OTHER);
    req->SetLabel("req's label");
    std::string label = "publish's label";
    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
    normalContent->SetText("normalContent's text");
    normalContent->SetTitle("normalContent's title");
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(normalContent);
    req->SetContent(content);
    EXPECT_EQ(g_advancedNotificationService->Publish(label, req), (int)ERR_OK);
}

/**
 * @tc.number    : AMS_ANS_Publish_00200
 * @tc.name      : ANSPublish00200
 * @tc.desc      : When the notification is in the notification queue,
 * the test publish interface can successfully publish a notification of normal text type.
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_00200, Function | SmallTest | Level1)
{
    sptr<NotificationRequest> req = new NotificationRequest();
    req->SetSlotType(NotificationConstant::SlotType::OTHER);
    req->SetLabel("req's label");
    std::string label = "publish's label";
    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
    normalContent->SetText("normalContent's text");
    normalContent->SetTitle("normalContent's title");
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(normalContent);
    req->SetContent(content);
    g_advancedNotificationService->Publish(label, req);
    EXPECT_EQ(g_advancedNotificationService->Publish(label, req), (int)ERR_OK);
}

/**
 * @tc.number    : AMS_ANS_Publish_00300
 * @tc.name      : ANSPublish00300
 * @tc.desc      : When slotType is CUSTOM and not systemApp, the notification publish fails,
 * and the notification publish interface returns ERR_ANS_NON_SYSTEM_APP.
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_00300, Function | SmallTest | Level1)
{
    IPCSkeleton::SetCallingUid(750);
    sptr<NotificationRequest> req = new NotificationRequest();
    req->SetSlotType(NotificationConstant::SlotType::CUSTOM);
    req->SetLabel("req's label");
    std::string label = "publish's label";
    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
    normalContent->SetText("normalContent's text");
    normalContent->SetTitle("normalContent's title");
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(normalContent);
    req->SetContent(content);
    EXPECT_EQ(g_advancedNotificationService->Publish(label, req), (int)ERR_OK);
}

/**
 * @tc.number    : AMS_ANS_Publish_00400
 * @tc.name      : ANSPublish00400
 * @tc.desc      : When the obtained bundleName is empty, the notification publish interface returns
 * ERR_ANS_INVALID_PARAM.
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_00400, Function | SmallTest | Level1)
{
    IPCSkeleton::SetCallingUid(1250);
    sptr<NotificationRequest> req = new NotificationRequest();
    req->SetSlotType(NotificationConstant::SlotType::CONTENT_INFORMATION);
    req->SetLabel("req's label");
    std::string label = "publish's label";
    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
    normalContent->SetText("normalContent's text");
    normalContent->SetTitle("normalContent's title");
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(normalContent);
    req->SetContent(content);
    EXPECT_EQ(g_advancedNotificationService->Publish(label, req), (int)ERR_OK);
}

/**
 * @tc.number    : AMS_ANS_Publish_00500
 * @tc.name      : ANSPublish00500
 * @tc.desc      : When the obtained bundleName does not have a corresponding slot in the database,
 * create the corresponding slot and publish a notification.
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_00500, Function | SmallTest | Level1)
{
    sptr<NotificationRequest> req = new NotificationRequest();
    req->SetSlotType(NotificationConstant::SlotType::CONTENT_INFORMATION);
    req->SetLabel("req's label");
    std::string label = "publish's label";
    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
    normalContent->SetText("normalContent's text");
    normalContent->SetTitle("normalContent's title");
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(normalContent);
    req->SetContent(content);
    EXPECT_EQ(g_advancedNotificationService->Publish(label, req), (int)ERR_OK);
}

/**
 * @tc.number    : AMS_ANS_Publish_00600
 * @tc.name      : ANSPublish00600
 * @tc.desc      : When the obtained bundleName have a corresponding slot in the database,
 * the test publish interface can successfully publish a notification of normal text type.
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_00600, Function | SmallTest | Level1)
{
    sptr<NotificationRequest> req = new NotificationRequest();
    req->SetSlotType(NotificationConstant::SlotType::OTHER);
    req->SetLabel("req's label");
    std::string label = "publish's label";
    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
    normalContent->SetText("normalContent's text");
    normalContent->SetTitle("normalContent's title");
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(normalContent);
    req->SetContent(content);
    TestAddSlots(NotificationConstant::SlotType::OTHER);
    EXPECT_EQ(g_advancedNotificationService->Publish(label, req), (int)ERR_OK);
}

/**
 * @tc.number    : AMS_ANS_Publish_00700
 * @tc.name      : ANSPublish00700
 * @tc.desc      : When the obtained bundleName have a corresponding slot in the database,
 * create the corresponding slot and publish a notification.
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_00700, Function | SmallTest | Level1)
{
    sptr<NotificationRequest> req = new NotificationRequest();
    req->SetSlotType(NotificationConstant::SlotType::CONTENT_INFORMATION);
    req->SetLabel("req's label");
    std::string label = "publish's label";
    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
    normalContent->SetText("normalContent's text");
    normalContent->SetTitle("normalContent's title");
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(normalContent);
    req->SetContent(content);
    TestAddSlots(NotificationConstant::SlotType::OTHER);
    EXPECT_EQ(g_advancedNotificationService->Publish(label, req), (int)ERR_OK);
}

/**
 * @tc.number    : AMS_ANS_Publish_00800
 * @tc.name      : ANSPublish00800
 * @tc.desc      : Create a slot of type SOCIAL_COMMUNICATION and successfully publish a notification
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_00800, Function | SmallTest | Level1)
{
    sptr<NotificationRequest> req = new NotificationRequest();
    req->SetSlotType(NotificationConstant::SlotType::SOCIAL_COMMUNICATION);
    req->SetLabel("req's label");
    std::string label = "publish's label";
    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
    normalContent->SetText("normalContent's text");
    normalContent->SetTitle("normalContent's title");
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(normalContent);
    req->SetContent(content);
    EXPECT_EQ(g_advancedNotificationService->Publish(label, req), (int)ERR_OK);
}

/**
 * @tc.number    : AMS_ANS_Publish_00900
 * @tc.name      : ANSPublish00900
 * @tc.desc      : Create a slot of type SERVICE_REMINDER and successfully publish a notification
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_00900, Function | SmallTest | Level1)
{
    sptr<NotificationRequest> req = new NotificationRequest();
    req->SetSlotType(NotificationConstant::SlotType::SERVICE_REMINDER);
    req->SetLabel("req's label");
    std::string label = "publish's label";
    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
    normalContent->SetText("normalContent's text");
    normalContent->SetTitle("normalContent's title");
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(normalContent);
    req->SetContent(content);
    EXPECT_EQ(g_advancedNotificationService->Publish(label, req), (int)ERR_OK);
}

/**
 * @tc.number    : AMS_ANS_Publish_01000
 * @tc.name      : ANSPublish01000
 * @tc.desc      : Create a slot of type CONTENT_INFORMATION and successfully publish a notification
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_01000, Function | SmallTest | Level1)
{
    sptr<NotificationRequest> req = new NotificationRequest();
    req->SetSlotType(NotificationConstant::SlotType::CONTENT_INFORMATION);
    req->SetLabel("req's label");
    std::string label = "publish's label";
    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
    normalContent->SetText("normalContent's text");
    normalContent->SetTitle("normalContent's title");
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(normalContent);
    req->SetContent(content);
    EXPECT_EQ(g_advancedNotificationService->Publish(label, req), (int)ERR_OK);
}

/**
 * @tc.number    : AMS_ANS_Publish_01100
 * @tc.name      : ANSPublish01100
 * @tc.desc      : Create a slot of type OTHER and successfully publish a notification
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_01100, Function | SmallTest | Level1)
{
    sptr<NotificationRequest> req = new NotificationRequest();
    req->SetSlotType(NotificationConstant::SlotType::OTHER);
    req->SetLabel("req's label");
    std::string label = "publish's label";
    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
    normalContent->SetText("normalContent's text");
    normalContent->SetTitle("normalContent's title");
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(normalContent);
    req->SetContent(content);
    EXPECT_EQ(g_advancedNotificationService->Publish(label, req), (int)ERR_OK);
}

/**
 * @tc.number    : AMS_ANS_Publish_01200
 * @tc.name      : ANSPublish01200
 * @tc.desc      : Create a slot of type CUSTOM and successfully publish a notification
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_01200, Function | SmallTest | Level1)
{
    sptr<NotificationRequest> req = new NotificationRequest();
    req->SetSlotType(NotificationConstant::SlotType::CUSTOM);
    req->SetLabel("req's label");
    std::string label = "publish's label";
    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
    normalContent->SetText("normalContent's text");
    normalContent->SetTitle("normalContent's title");
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(normalContent);
    req->SetContent(content);
    EXPECT_EQ(g_advancedNotificationService->Publish(label, req), (int)ERR_OK);
}

/**
 * @tc.number    : AMS_ANS_Publish_01300
 * @tc.name      : ANSPublish01300
 * @tc.desc      : When a bundle is not allowed to publish a notification, the notification publishing interface returns
 * ERR_ANS_NOT_ALLOWED
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_01300, Function | SmallTest | Level1)
{
    sptr<NotificationRequest> req = new NotificationRequest();
    req->SetSlotType(NotificationConstant::SlotType::SOCIAL_COMMUNICATION);
    req->SetLabel("req's label");
    std::string label = "publish's label";
    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
    normalContent->SetText("normalContent's text");
    normalContent->SetTitle("normalContent's title");
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(normalContent);
    req->SetContent(content);
    g_advancedNotificationService->SetNotificationsEnabledForBundle("bundleName", false);
    EXPECT_EQ(g_advancedNotificationService->Publish(label, req), (int)ERR_ANS_NOT_ALLOWED);
}

/**
 * @tc.number    : AMS_ANS_Publish_01400
 * @tc.name      : ANSPublish01400
 * @tc.desc      : When IsAlertOneTime, reset ColorEnabled, Sound, VibrationStyle, and publish the notification
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_01400, Function | SmallTest | Level1)
{
    TestAddSlotGroups();
    std::vector<std::string> groupIds;
    groupIds.push_back("id0");
    EXPECT_EQ(g_advancedNotificationService->RemoveSlotGroups(groupIds), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_01600
 * @tc.name      : AMS_ANS_GetSlot_0200
 * @tc.desc      : Test GetSlots function when add two identical data
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_01600, Function | SmallTest | Level1)
{
    std::vector<sptr<NotificationSlot>> slots;
    sptr<NotificationSlot> slot0 = new NotificationSlot(NotificationConstant::OTHER);
    sptr<NotificationSlot> slot1 = new NotificationSlot(NotificationConstant::OTHER);
    slots.push_back(slot0);
    slots.push_back(slot1);
    std::vector<sptr<NotificationSlot>> slotsResult;
    g_advancedNotificationService->AddSlots(slots);
    g_advancedNotificationService->GetSlots(slotsResult);
    EXPECT_EQ((int)slots.size(), 2);
    EXPECT_EQ((int)slotsResult.size(), 1);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_01700
 * @tc.name      : AMS_ANS_GetSlotGroup_0100
 * @tc.desc      : Test GetSlotGroup function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_01700, Function | SmallTest | Level1)
{
    std::vector<sptr<NotificationSlotGroup>> groups;
    sptr<NotificationSlotGroup> group2 = new NotificationSlotGroup("id2", "name2");
    group2->SetDescription("Description");
    groups.push_back(group2);
    g_advancedNotificationService->AddSlotGroups(groups);
    sptr<NotificationSlotGroup> group = new NotificationSlotGroup();
    EXPECT_EQ(g_advancedNotificationService->GetSlotGroup("id2", group), ERR_OK);
    EXPECT_EQ(group2->GetId(), group->GetId());
    EXPECT_EQ(group2->GetName(), group->GetName());
    EXPECT_EQ(group2->GetDescription(), group->GetDescription());
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_01800
 * @tc.name      : AMS_ANS_SetNotificationBadgeNum_0100
 * @tc.desc      : Test SetNotificationBadgeNum function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_01800, Function | SmallTest | Level1)
{
    TestAddSlots(NotificationConstant::SlotType::OTHER);
    EXPECT_EQ((int)g_advancedNotificationService->SetNotificationBadgeNum(2), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_01900
 * @tc.name      : AMS_ANS_GetBundleImportance_0100
 * @tc.desc      : Test GetBundleImportance function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_01900, Function | SmallTest | Level1)
{
    TestAddSlots(NotificationConstant::SlotType::OTHER);
    int importance;
    EXPECT_EQ((int)g_advancedNotificationService->GetBundleImportance(importance), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_02000
 * @tc.name      : AMS_ANS_SetPrivateNotificationsAllowed_0100
 * @tc.desc      : Test SetPrivateNotificationsAllowed function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_02000, Function | SmallTest | Level1)
{
    TestAddSlots(NotificationConstant::SlotType::OTHER);
    EXPECT_EQ((int)g_advancedNotificationService->SetPrivateNotificationsAllowed(true), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_02100
 * @tc.name      : AMS_ANS_GetPrivateNotificationsAllowed_0100
 * @tc.desc      : Test GetPrivateNotificationsAllowed function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_02100, Function | SmallTest | Level1)
{
    TestAddSlots(NotificationConstant::SlotType::OTHER);
    EXPECT_EQ((int)g_advancedNotificationService->SetPrivateNotificationsAllowed(true), (int)ERR_OK);
    bool allow;
    EXPECT_EQ((int)g_advancedNotificationService->GetPrivateNotificationsAllowed(allow), (int)ERR_OK);
    EXPECT_TRUE(allow);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_02300
 * @tc.name      : AMS_ANS_UpdateSlots_0200
 * @tc.desc      : Test UpdateSlots function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_02300, Function | SmallTest | Level1)
{
    std::string bundle = "bundleName";
    std::vector<sptr<NotificationSlot>> slots;
    TestAddSlots(NotificationConstant::SlotType::OTHER);
    sptr<NotificationSlot> slot0 = new NotificationSlot(NotificationConstant::OTHER);
    slots.push_back(slot0);
    EXPECT_EQ((int)g_advancedNotificationService->UpdateSlots(bundle, slots), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_02500
 * @tc.name      : AMS_ANS_UpdateSlotGroups_0200
 * @tc.desc      : Test UpdateSlotGroups function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_02500, Function | SmallTest | Level1)
{
    std::string bundle = "bundleName";
    std::vector<sptr<NotificationSlotGroup>> groups;
    sptr<NotificationSlotGroup> group2 = new NotificationSlotGroup("id0", "name2");
    groups.push_back(group2);
    TestAddSlotGroups();
    EXPECT_EQ((int)g_advancedNotificationService->UpdateSlotGroups(bundle, groups), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_02600
 * @tc.name      : AMS_ANS_SetNotificationsEnabledForBundle_0100
 * @tc.desc      : Test SetNotificationsEnabledForBundle function when no bundle
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_02600, Function | SmallTest | Level1)
{
    std::string bundle = "";
    EXPECT_EQ((int)g_advancedNotificationService->SetNotificationsEnabledForBundle(bundle, true),
        (int)ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_02700
 * @tc.name      : AMS_ANS_SetShowBadgeEnabledForBundle_0100
 * @tc.desc      : Test the SetShowBadgeEnabledForBundle function when the parameter is wrong
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_02700, Function | SmallTest | Level1)
{
    std::string bundle = "";
    EXPECT_EQ(g_advancedNotificationService->SetShowBadgeEnabledForBundle(bundle, true), (int)ERR_ANS_INVALID_PARAM);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_02800
 * @tc.name      : AMS_ANS_GetShowBadgeEnabledForBundle_0100
 * @tc.desc      : Test GetShowBadgeEnabledForBundle function when no bundle
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_02800, Function | SmallTest | Level1)
{
    std::string bundle = "bundleName";
    bool allow;
    EXPECT_EQ((int)g_advancedNotificationService->GetShowBadgeEnabledForBundle(bundle, allow), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_02900
 * @tc.name      : AMS_ANS_GetActiveNotifications_0100
 * @tc.desc      : Test GetActiveNotifications function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_02900, Function | SmallTest | Level1)
{
    std::vector<sptr<NotificationRequest>> notifications;
    EXPECT_EQ((int)g_advancedNotificationService->GetActiveNotifications(notifications), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_03000
 * @tc.name      : AMS_ANS_SetNotificationAgent_0100
 * @tc.desc      : Test SetNotificationAgent function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_03000, Function | SmallTest | Level1)
{
    std::string agent = "agent";
    g_advancedNotificationService->SetNotificationAgent(agent);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_03100
 * @tc.name      : AMS_ANS_GetNotificationAgent_0100
 * @tc.desc      : Test GetNotificationAgent function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_03100, Function | SmallTest | Level1)
{
    std::string agent;
    g_advancedNotificationService->GetNotificationAgent(agent);
}

/**
 * @tc.number    : NotificationPreferencesTest_03200
 * @tc.name      : AMS_ANS_CanPublishAsBundle_0100
 * @tc.desc      : Test CanPublishAsBundle function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_03200, Function | SmallTest | Level1)
{
    std::string representativeBundle = "representativeBundle";
    bool canPublish = true;
    g_advancedNotificationService->CanPublishAsBundle(representativeBundle, canPublish);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_03300
 * @tc.name      : AMS_ANS_PublishAsBundle_0100
 * @tc.desc      : Test PublishAsBundle function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_03300, Function | SmallTest | Level1)
{
    std::string representativeBundle = "representativeBundle";
    sptr<NotificationRequest> notification = new NotificationRequest();
    g_advancedNotificationService->PublishAsBundle(notification, representativeBundle);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_03400
 * @tc.name      : AMS_ANS_SetDisturbMode_0100
 * @tc.desc      : Test SetDisturbMode function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_03400, Function | SmallTest | Level1)
{
    EXPECT_EQ((int)g_advancedNotificationService->SetDisturbMode(NotificationConstant::DisturbMode::ALLOW_PRIORITY),
        (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_03500
 * @tc.name      : AMS_ANS_GetDisturbMode_0100
 * @tc.desc      : Test GetDisturbMode function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_03500, Function | SmallTest | Level1)
{
    TestAddSlots(NotificationConstant::SlotType::OTHER);
    NotificationConstant::DisturbMode mode;
    EXPECT_EQ((int)g_advancedNotificationService->GetDisturbMode(mode), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_03600
 * @tc.name      : AMS_ANS_HasNotificationPolicyAccessPermission_0100
 * @tc.desc      : Test HasNotificationPolicyAccessPermission function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_03600, Function | SmallTest | Level1)
{
    bool granted;
    g_advancedNotificationService->HasNotificationPolicyAccessPermission(granted);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_03700
 * @tc.name      : AMS_ANS_Delete_0100
 * @tc.desc      : Test Delete function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_03700, Function | SmallTest | Level1)
{
    const std::string key = "key";
    EXPECT_EQ((int)g_advancedNotificationService->Delete(key), (int)ERR_ANS_NOTIFICATION_NOT_EXISTS);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_03800
 * @tc.name      : AMS_ANS_DeleteByBundle_0100
 * @tc.desc      : Test DeleteByBundle function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_03800, Function | SmallTest | Level1)
{
    const std::string bundle = "bundleName";
    EXPECT_EQ(g_advancedNotificationService->DeleteByBundle(bundle), ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_03900
 * @tc.name      : AMS_ANS_DeleteAll_0100
 * @tc.desc      : Test DeleteAll function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_03900, Function | SmallTest | Level1)
{
    EXPECT_EQ(g_advancedNotificationService->DeleteAll(), ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_04000
 * @tc.name      : AMS_ANS_GetSlotsByBundle_0100
 * @tc.desc      : Test GetSlotsByBundle function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_04000, Function | SmallTest | Level1)
{
    TestAddSlots(NotificationConstant::SlotType::OTHER);
    std::string bundle = "bundleName";
    std::vector<sptr<NotificationSlot>> slots;
    EXPECT_EQ(g_advancedNotificationService->GetSlotsByBundle(bundle, slots), ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_04100
 * @tc.name      : AMS_ANS_GetSpecialActiveNotifications_0100
 * @tc.desc      : Test GetSpecialActiveNotifications function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_04100, Function | SmallTest | Level1)
{
    sptr<NotificationRequest> req = new NotificationRequest();
    req->SetSlotType(NotificationConstant::SlotType::SOCIAL_COMMUNICATION);
    req->SetLabel("req's label");
    req->SetAlertOneTime(true);
    std::string label = "publish's label";
    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
    normalContent->SetText("normalContent's text");
    normalContent->SetTitle("normalContent's title");
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(normalContent);
    req->SetContent(content);
    g_advancedNotificationService->SetNotificationsEnabledForBundle("bundleName", true);
    g_advancedNotificationService->Publish(label, req);
    EXPECT_EQ(g_advancedNotificationService->Publish(label, req), (int)ERR_OK);
}

}  // namespace Notification
}  // namespace OHOS