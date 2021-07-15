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

#include "advanced_notification_service.h"
#include "notification_preferences.h"
#include <functional>
#include <gtest/gtest.h>
#define private public
#include "notification_subscriber.h"

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
}

void AdvancedNotificationServiceTest::TearDown()
{
    NotificationPreferences::GetInstance().ClearNotificationInRestoreFactorySettings();
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

void TestAddSlots()
{
    std::vector<sptr<NotificationSlot>> slots;
    sptr<NotificationSlot> slotOther = new NotificationSlot(NotificationConstant::OTHER);
    slots.push_back(slotOther);
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
 * @tc.number    : AdvancedNotificationServiceTest_00100
 * @tc.name      : AMS_ANS_Publish_0100
 * @tc.desc      : Test the branch if notification not exists
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_00100, Function | SmallTest | Level1)
{
    sptr<NotificationRequest> req = new NotificationRequest();
    req->SetSlotType(NotificationConstant::OTHER);
    TestAddSlots();
    EXPECT_EQ(g_advancedNotificationService->Publish(std::string(), req), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_00200
 * @tc.name      : AMS_ANS_Publish_0200
 * @tc.desc      : Test the branch if notification is exists
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_00200, Function | SmallTest | Level1)
{
    int32_t notificationId = 1;
    sptr<NotificationRequest> req = new NotificationRequest(notificationId);
    req->SetSlotType(NotificationConstant::OTHER);
    req->SetAlertOneTime(true);
    TestAddSlots();
    g_advancedNotificationService->Publish(std::string(), req);
    EXPECT_EQ(g_advancedNotificationService->Publish(std::string(), req), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_00300
 * @tc.name      : AMS_ANS_Publish_0300
 * @tc.desc      : Publish notification when no slot is set 
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_00300, Function | SmallTest | Level1)
{
    int32_t notificationId = 1;
    sptr<NotificationRequest> req = new NotificationRequest(NULL, notificationId);
    req->SetSlotType(NotificationConstant::OTHER);
    EXPECT_EQ(g_advancedNotificationService->Publish(std::string(), req), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_00400
 * @tc.name      : AMS_ANS_Publish_0400
 * @tc.desc      : Test Publish function when no slotType
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_00400, Function | SmallTest | Level1)
{
    int32_t notificationId = 1;
    sptr<NotificationRequest> req = new NotificationRequest(notificationId);
    req->SetSlotType(NotificationConstant::CUSTOM);
    TestAddSlots();
    EXPECT_EQ((int)g_advancedNotificationService->Publish(std::string(), req), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_00500
 * @tc.name      : AMS_ANS_Publish_0500
 * @tc.desc      : publish function when NotificationsEnabled is false
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_00500, Function | SmallTest | Level1)
{
    int32_t notificationId = 1;
    sptr<NotificationRequest> req = new NotificationRequest(notificationId);
    req->SetSlotType(NotificationConstant::OTHER);
    TestAddSlots();
    g_advancedNotificationService->SetNotificationsEnabledForBundle("bundleName", false);
    EXPECT_EQ(g_advancedNotificationService->Publish(std::string(), req), (int)ERR_ANS_NOT_ALLOWED);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_00600
 * @tc.name      : AMS_ANS_Cancel_0100
 * @tc.desc      : public two notification to cancel one of them 
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_00600, Function | SmallTest | Level1)
{
    int32_t notificationId = 2;
    std::string label = "testLabel";
    sptr<NotificationRequest> req = new NotificationRequest(NULL, notificationId);
    req->SetSlotType(NotificationConstant::OTHER);
    req->SetLabel(label);
    TestAddSlots();
    g_advancedNotificationService->Publish(label, req);
    g_advancedNotificationService->Publish(label, req);
    EXPECT_EQ(g_advancedNotificationService->Cancel(notificationId, label), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_00700
 * @tc.name      : AMS_ANS_Cancel_0200
 * @tc.desc      : Test Cancel function when notification no exists
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_00700, Function | SmallTest | Level1)
{
    int32_t notificationId = 0;
    std::string label = "testLabel";
    EXPECT_EQ((int)g_advancedNotificationService->Cancel(notificationId, label), (int)ERR_ANS_NOTIFICATION_NOT_EXISTS);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_00800
 * @tc.name      : AMS_ANS_CancelAll_0100
 * @tc.desc      : Test CancelAll function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_00800, Function | SmallTest | Level1)
{
    EXPECT_EQ(g_advancedNotificationService->CancelAll(), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_00900
 * @tc.name      : AMS_ANS_Cancel_0100
 * @tc.desc      : Test Cancel function when unremovable is true
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_00900, Function | SmallTest | Level1)
{
    int32_t notificationId = 2;
    std::string label = "testLabel";
    sptr<NotificationRequest> req = new NotificationRequest(notificationId);
    req->SetSlotType(NotificationConstant::OTHER);
    req->SetLabel(label);
    req->SetUnremovable(true);
    TestAddSlots();
    g_advancedNotificationService->Publish(label, req);
    g_advancedNotificationService->Publish(label, req);
    EXPECT_EQ(g_advancedNotificationService->Cancel(notificationId, label), (int)ERR_ANS_NOTIFICATION_IS_UNREMOVABLE);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_01000
 * @tc.name      : AMS_ANS_AddSlots_0100
 * @tc.desc      : Test AddSlots function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_01000, Function | SmallTest | Level1)
{
    std::vector<sptr<NotificationSlot>> slots;
    sptr<NotificationSlot> slot0 = new NotificationSlot(NotificationConstant::OTHER);
    sptr<NotificationSlot> slot1 = new NotificationSlot(NotificationConstant::OTHER);
    slots.push_back(slot0);
    slots.push_back(slot1);
    EXPECT_EQ(g_advancedNotificationService->AddSlots(slots), ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_01100
 * @tc.name      : AMS_ANS_RemoveSlotByType_0100
 * @tc.desc      : Test RemoveSlotByType function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_01100, Function | SmallTest | Level1)
{
    TestAddSlots();
    EXPECT_EQ(g_advancedNotificationService->RemoveSlotByType(NotificationConstant::OTHER), ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_01200
 * @tc.name      : AMS_ANS_RemoveSlotByType_0200
 * @tc.desc      : Test RemoveSlotByType function when no type
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_01200, Function | SmallTest | Level1)
{
    TestAddSlots();
    EXPECT_EQ((int)g_advancedNotificationService->RemoveSlotByType(NotificationConstant::CUSTOM),
        (int)ERR_ANS_PREFERENCES_NOTIFICATION_SLOT_ID_INVALID);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_01300
 * @tc.name      : AMS_ANS_AddSlotGroups_0100
 * @tc.desc      : Test AddSlotGroups function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_01300, Function | SmallTest | Level1)
{
    std::vector<sptr<NotificationSlotGroup>> groups;
    sptr<NotificationSlotGroup> group0 = new NotificationSlotGroup("id0", "name0");
    sptr<NotificationSlotGroup> group1 = new NotificationSlotGroup("id1", "name1");
    groups.push_back(group0);
    groups.push_back(group1);
    EXPECT_EQ(g_advancedNotificationService->AddSlotGroups(groups), ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_01400
 * @tc.name      : AMS_ANS_RemoveSlotGroups_0100
 * @tc.desc      : Test RemoveSlotGroups function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_01400, Function | SmallTest | Level1)
{
    TestAddSlotGroups();
    std::vector<std::string> groupIds;
    groupIds.push_back("id0");
    EXPECT_EQ(g_advancedNotificationService->RemoveSlotGroups(groupIds), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_01500
 * @tc.name      : AMS_ANS_GetSlot_0100
 * @tc.desc      : Test GetSlot function for data
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_01500, Function | SmallTest | Level1)
{
    std::vector<sptr<NotificationSlot>> slots;
    sptr<NotificationSlot> slot = new NotificationSlot(NotificationConstant::OTHER);
    sptr<NotificationSlot> slot0 = new NotificationSlot(NotificationConstant::OTHER);
    slots.push_back(slot0);
    g_advancedNotificationService->AddSlots(slots);
    EXPECT_EQ((int)g_advancedNotificationService->GetSlot("id0", slot), ERR_OK);
    EXPECT_EQ(slot->GetName(), slot0->GetName());
    EXPECT_EQ(slot->GetId(), slot0->GetId());
    EXPECT_EQ(slot->GetLevel(), slot0->GetLevel());
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
    TestAddSlots();
    EXPECT_EQ((int)g_advancedNotificationService->SetNotificationBadgeNum(2), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_01900
 * @tc.name      : AMS_ANS_GetBundleImportance_0100
 * @tc.desc      : Test GetBundleImportance function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_01900, Function | SmallTest | Level1)
{
    TestAddSlots();
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
    TestAddSlots();
    EXPECT_EQ((int)g_advancedNotificationService->SetPrivateNotificationsAllowed(true), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_02100
 * @tc.name      : AMS_ANS_GetPrivateNotificationsAllowed_0100
 * @tc.desc      : Test GetPrivateNotificationsAllowed function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_02100, Function | SmallTest | Level1)
{
    TestAddSlots();
    EXPECT_EQ((int)g_advancedNotificationService->SetPrivateNotificationsAllowed(true), (int)ERR_OK);
    bool allow;
    EXPECT_EQ((int)g_advancedNotificationService->GetPrivateNotificationsAllowed(allow), (int)ERR_OK);
    EXPECT_TRUE(allow);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_02200
 * @tc.name      : AMS_ANS_UpdateSlots_0100
 * @tc.desc      : Test UpdateSlots function when no slot
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_02200, Function | SmallTest | Level1)
{
    std::string bundle = "bundleName";
    std::vector<sptr<NotificationSlot>> slots;
    sptr<NotificationSlot> slot0 = new NotificationSlot(NotificationConstant::OTHER);
    slots.push_back(slot0);
    EXPECT_EQ((int)g_advancedNotificationService->UpdateSlots(bundle, slots), (int)ERR_ANS_PREFERENCES_FILE_IS_NULL);
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
    TestAddSlots();
    sptr<NotificationSlot> slot0 = new NotificationSlot(NotificationConstant::OTHER);
    slots.push_back(slot0);
    EXPECT_EQ((int)g_advancedNotificationService->UpdateSlots(bundle, slots), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_02400
 * @tc.name      : AMS_ANS_UpdateSlotGroups_0100
 * @tc.desc      : Test UpdateSlotGroups function when no group
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_02400, Function | SmallTest | Level1)
{
    std::string bundle = "bundleName";
    std::vector<sptr<NotificationSlotGroup>> groups;
    sptr<NotificationSlotGroup> group2 = new NotificationSlotGroup("id2", "name2");
    groups.push_back(group2);
    EXPECT_EQ(
        (int)g_advancedNotificationService->UpdateSlotGroups(bundle, groups), (int)ERR_ANS_PREFERENCES_FILE_IS_NULL);
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
    EXPECT_EQ((int)g_advancedNotificationService->GetShowBadgeEnabledForBundle(bundle, allow),
        (int)ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST);
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
    TestAddSlots();
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
    TestAddSlots();
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
    req->SetSlotType(NotificationConstant::OTHER);
    TestAddSlots();
    g_advancedNotificationService->Publish(std::string(), req);
    EXPECT_EQ(g_advancedNotificationService->Publish(std::string(), req), (int)ERR_OK);
    sptr<Notification> notification = new Notification(req);
    std::vector<sptr<Notification>> notifications;
    std::string key1 = notification->GetKey();
    std::string key2 = "key2";
    std::vector<std::string> keys;
    keys.push_back(key1);
    keys.push_back(key2);
    EXPECT_EQ(g_advancedNotificationService->GetSpecialActiveNotifications(keys, notifications), ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_04200
 * @tc.name      : AMS_ANS_Subscribe_0100
 * @tc.desc      : Test Subscribe function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_04200, Function | SmallTest | Level1)
{
    auto subscriber = new TestAnsSubscriber();
    sptr<NotificationSubscribeInfo> info = new NotificationSubscribeInfo();
    EXPECT_EQ((int)g_advancedNotificationService->Subscribe(subscriber->GetImpl(), info), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_04300
 * @tc.name      : AMS_ANS_Unsubscribe_0100
 * @tc.desc      : Test Unsubscribe function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_04300, Function | SmallTest | Level1)
{
    auto subscriber = new TestAnsSubscriber();
    sptr<NotificationSubscribeInfo> info = new NotificationSubscribeInfo();
    EXPECT_EQ((int)g_advancedNotificationService->Unsubscribe(subscriber->GetImpl(), info), (int)ERR_OK);
    ;
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_04400
 * @tc.name      : AMS_ANS_SetNotificationsEnabledForBundle_0200
 * @tc.desc      : Test SetNotificationsEnabledForBundle function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_04400, Function | SmallTest | Level1)
{
    std::string bundle = "bundleName";
    TestAddSlots();
    EXPECT_EQ((int)g_advancedNotificationService->SetNotificationsEnabledForBundle(bundle, true), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_04500
 * @tc.name      : AMS_ANS_GetShowBadgeEnabledForBundle_0200
 * @tc.desc      : Test GetShowBadgeEnabledForBundle function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_04500, Function | SmallTest | Level1)
{
    std::string bundle = "bundleName";
    TestAddSlots();
    EXPECT_EQ((int)g_advancedNotificationService->SetShowBadgeEnabledForBundle(bundle, true), (int)ERR_OK);
    bool allow;
    EXPECT_EQ((int)g_advancedNotificationService->GetShowBadgeEnabledForBundle(bundle, allow), (int)ERR_OK);
    EXPECT_TRUE(allow);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_04600
 * @tc.name      : AMS_ANS_GetSlotByType_0100
 * @tc.desc      : Test GetSlotByType function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_04600, Function | SmallTest | Level1)
{
    std::string bundle = "bundleName";
    TestAddSlots();
    sptr<NotificationSlot> slot;
    EXPECT_EQ((int)g_advancedNotificationService->GetSlotByType(NotificationConstant::OTHER, slot), (int)ERR_OK);
    EXPECT_EQ(slot->GetId(), "OTHER");
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_04700
 * @tc.name      : AMS_ANS_GetSlotByType_0200
 * @tc.desc      : Test the CreateSlot branch of the GetSlotByType function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_04700, Function | SmallTest | Level1)
{
    std::string bundle = "bundleName";
    sptr<NotificationSlot> slot;
    g_advancedNotificationService->GetSlotByType(NotificationConstant::SOCIAL_COMMUNICATION, slot);
    g_advancedNotificationService->GetSlotByType(NotificationConstant::SERVICE_REMINDER, slot);
    g_advancedNotificationService->GetSlotByType(NotificationConstant::CONTENT_INFORMATION, slot);
    g_advancedNotificationService->GetSlotByType(NotificationConstant::OTHER, slot);
    EXPECT_EQ((int)g_advancedNotificationService->GetSlotByType(NotificationConstant::CUSTOM, slot), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_04800
 * @tc.name      : AMS_ANS_GetSlotGroups_0100
 * @tc.desc      : Test GetSlotGroups function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_04800, Function | SmallTest | Level1)
{
    std::vector<sptr<NotificationSlotGroup>> groups;
    sptr<NotificationSlotGroup> group2 = new NotificationSlotGroup("id2", "name2");
    group2->SetDescription("Description");
    groups.push_back(group2);
    g_advancedNotificationService->AddSlotGroups(groups);
    std::vector<sptr<NotificationSlotGroup>> group;
    EXPECT_EQ(g_advancedNotificationService->GetSlotGroups(group), ERR_OK);
    EXPECT_EQ(groups.size(), group.size());
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_04900
 * @tc.name      : AMS_ANS_GetAllActiveNotifications_0100
 * @tc.desc      : Test GetAllActiveNotifications function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_04900, Function | SmallTest | Level1)
{
    std::vector<sptr<Notification>> notifications;
    EXPECT_EQ(g_advancedNotificationService->GetAllActiveNotifications(notifications), ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_05000
 * @tc.name      : AMS_ANS_SetNotificationsEnabledForAllBundles_0100
 * @tc.desc      : Test SetNotificationsEnabledForAllBundles function for else
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_05000, Function | SmallTest | Level1)
{
    std::vector<sptr<Notification>> notifications;
    EXPECT_EQ((int)g_advancedNotificationService->SetNotificationsEnabledForAllBundles("test", true), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_05100
 * @tc.name      : AMS_ANS_SetNotificationsEnabledForAllBundles_0200
 * @tc.desc      : Test SetNotificationsEnabledForAllBundles function for if
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_05100, Function | SmallTest | Level1)
{
    TestAddSlots();
    std::vector<sptr<Notification>> notifications;
    EXPECT_EQ(
        (int)g_advancedNotificationService->SetNotificationsEnabledForAllBundles(std::string(), true), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_05200
 * @tc.name      : AMS_ANS_SetNotificationsEnabledForSpecialBundle_0100
 * @tc.desc      : Test SetNotificationsEnabledForSpecialBundle function for if
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_05200, Function | SmallTest | Level1)
{
    TestAddSlots();
    std::vector<sptr<Notification>> notifications;
    EXPECT_EQ(
        (int)g_advancedNotificationService->SetNotificationsEnabledForSpecialBundle(std::string(), "bundleName", true),
        (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_05300
 * @tc.name      : AMS_ANS_SetNotificationsEnabledForSpecialBundle_0200
 * @tc.desc      : Test SetNotificationsEnabledForSpecialBundle function for else
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_05300, Function | SmallTest | Level1)
{
    std::vector<sptr<Notification>> notifications;
    EXPECT_EQ((int)g_advancedNotificationService->SetNotificationsEnabledForSpecialBundle("test", "bundleName", true),
        (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_05400
 * @tc.name      : AMS_ANS_IsAllowedNotify_0100
 * @tc.desc      : Test IsAllowedNotify function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_05400, Function | SmallTest | Level1)
{
    bool allowed = true;
    EXPECT_EQ((int)g_advancedNotificationService->IsAllowedNotify(allowed), (int)ERR_ANS_PREFERENCES_FILE_IS_NULL);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_05500
 * @tc.name      : AMS_ANS_IsAllowedNotify_0200
 * @tc.desc      : Test IsAllowedNotify function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_05500, Function | SmallTest | Level1)
{
    TestAddSlots();
    bool allowed = true;
    EXPECT_EQ((int)g_advancedNotificationService->IsAllowedNotify(allowed), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_05600
 * @tc.name      : AMS_ANS_IsSpecialBundleAllowedNotify_0100
 * @tc.desc      : Test IsSpecialBundleAllowedNotify function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_05600, Function | SmallTest | Level1)
{
    TestAddSlots();
    std::vector<sptr<Notification>> notifications;
    bool allowed = true;
    std::string bundle = "bundleName";
    EXPECT_EQ((int)g_advancedNotificationService->IsSpecialBundleAllowedNotify(bundle, allowed), (int)ERR_OK);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_05700
 * @tc.name      : AMS_ANS_IsSpecialBundleAllowedNotify_0200
 * @tc.desc      : Test IsSpecialBundleAllowedNotify function
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_05700, Function | SmallTest | Level1)
{
    std::vector<sptr<Notification>> notifications;
    bool allowed = true;
    std::string bundle = "bundleName";
    EXPECT_EQ((int)g_advancedNotificationService->IsSpecialBundleAllowedNotify(bundle, allowed),
        (int)ERR_ANS_PREFERENCES_FILE_IS_NULL);
}

/**
 * @tc.number    : AdvancedNotificationServiceTest_05800
 * @tc.name      : AMS_ANS_GetSlotsByBundle_0200
 * @tc.desc      : Test GetSlotsByBundle function when no bundle
 */
HWTEST_F(AdvancedNotificationServiceTest, AdvancedNotificationServiceTest_05800, Function | SmallTest | Level1)
{
    std::string bundle = "bundleName";
    std::vector<sptr<NotificationSlot>> slots;
    EXPECT_EQ(
        (int)g_advancedNotificationService->GetSlotsByBundle(bundle, slots), (int)ERR_ANS_PREFERENCES_FILE_IS_NULL);
}
}  // namespace Notification
}  // namespace OHOS