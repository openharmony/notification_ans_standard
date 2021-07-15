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

#include "notification_preferences.h"
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace Notification {

class NotificationPreferencesTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NotificationPreferencesTest::SetUpTestCase()
{}

void NotificationPreferencesTest::TearDownTestCase()
{}

void NotificationPreferencesTest::SetUp()
{
    NotificationPreferences::GetInstance().ClearNotificationInRestoreFactorySettings();
}

void NotificationPreferencesTest::TearDown()
{
    NotificationPreferences::GetInstance().ClearNotificationInRestoreFactorySettings();
}

void TestAddNotificationSlot()
{
    std::string bundleName = "bundleName";
    sptr<NotificationSlot> spSlot = new NotificationSlot(NotificationConstant::OTHER);
    std::vector<sptr<NotificationSlot>> spSlots;
    spSlots.push_back(spSlot);
    NotificationPreferences::GetInstance().AddNotificationSlots(bundleName, spSlots);
}

void TestAddNotificationSlotGroup()
{
    std::string bundleName = "bundleName";
    sptr<NotificationSlotGroup> spGroup = new NotificationSlotGroup("id", "name");
    std::vector<sptr<NotificationSlotGroup>> groups;
    groups.push_back(spGroup);
    NotificationPreferences::GetInstance().AddNotificationSlotGroups(bundleName, groups);
}

/**
 * @tc.number    : NotificationPreferencesTest_00100
 * @tc.name      : AMS_ANS_AddNotificationSlots_0100
 * @tc.desc      : Test AddNotificationSlots function
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_00100, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    sptr<NotificationSlot> spSlot = new NotificationSlot(NotificationConstant::OTHER);
    std::vector<sptr<NotificationSlot>> spSlots;
    spSlots.push_back(spSlot);
    EXPECT_EQ((int)NotificationPreferences::GetInstance().AddNotificationSlots(bundleName, spSlots), (int)ERR_OK);
}

/**
 * @tc.number    : NotificationPreferencesTest_00200
 * @tc.name      : AMS_ANS_AddNotificationSlots_0200
 * @tc.desc      : Test AddNotificationSlots function when add two identical data
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_00200, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName1";
    sptr<NotificationSlot> spSlot = new NotificationSlot(NotificationConstant::OTHER);
    spSlot->SetSlotGroup("test");
    std::vector<sptr<NotificationSlot>> spSlots;
    spSlots.push_back(spSlot);
    NotificationPreferences::GetInstance().AddNotificationSlots(bundleName, spSlots);
    EXPECT_EQ((int)NotificationPreferences::GetInstance().AddNotificationSlots(bundleName, spSlots), (int)ERR_OK);
}

/**
 * @tc.number    : NotificationPreferencesTest_00300
 * @tc.name      : AMS_ANS_AddNotificationSlots_0300
 * @tc.desc      : Test AddNotificationSlots function when the parameter is wrong
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_00300, Function | SmallTest | Level1)
{
    std::vector<sptr<NotificationSlot>> spSlots;
    EXPECT_EQ((int)NotificationPreferences::GetInstance().AddNotificationSlots(std::string(), spSlots),
        (int)ERR_ANS_INVALID_PARAM);
}

/**
 * @tc.number    : NotificationPreferencesTest_00400
 * @tc.name      : AMS_ANS_AddNotificationSlots_0100
 * @tc.desc      : Test AddNotificationSlotGroups function
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_00400, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    sptr<NotificationSlotGroup> spSlotGroup = new NotificationSlotGroup("id", "name");
    std::vector<sptr<NotificationSlotGroup>> spSlotGroups;
    spSlotGroups.push_back(spSlotGroup);
    EXPECT_EQ(
        (int)NotificationPreferences::GetInstance().AddNotificationSlotGroups(bundleName, spSlotGroups), (int)ERR_OK);
}

/**
 * @tc.number    : NotificationPreferencesTest_00500
 * @tc.name      : AMS_ANS_AddNotificationSlotGroups_0200
 * @tc.desc      : Test AddNotificationSlotGroups function when the parameter is wrong
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_00500, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    sptr<NotificationSlotGroup> spSlotGroup = new NotificationSlotGroup("", "name");
    std::vector<sptr<NotificationSlotGroup>> spSlotGroups;
    spSlotGroups.push_back(spSlotGroup);
    EXPECT_EQ((int)NotificationPreferences::GetInstance().AddNotificationSlotGroups(bundleName, spSlotGroups),
        (int)ERR_ANS_PREFERENCES_NOTIFICATION_SLOTGROUP_ID_INVALID);
}

/**
 * @tc.number    : NotificationPreferencesTest_00600
 * @tc.name      : AMS_ANS_AddNotificationSlotGroups_0300
 * @tc.desc      : Test AddNotificationSlotGroups function when bundleName is not set
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_00600, Function | SmallTest | Level1)
{
    std::vector<sptr<NotificationSlotGroup>> spSlotGroups;
    EXPECT_EQ((int)NotificationPreferences::GetInstance().AddNotificationSlotGroups(std::string(), spSlotGroups),
        (int)ERR_ANS_INVALID_PARAM);
}

/**
 * @tc.number    : NotificationPreferencesTest_00700
 * @tc.name      : AMS_ANS_RemoveNotificationSlot_0100
 * @tc.desc      : Test RemoveNotificationSlot function
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_00700, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    sptr<NotificationSlot> spSlot = new NotificationSlot(NotificationConstant::OTHER);
    std::vector<sptr<NotificationSlot>> spSlots;
    spSlots.push_back(spSlot);
    NotificationPreferences::GetInstance().AddNotificationSlots(bundleName, spSlots);
    std::vector<std::string> slotIds;
    slotIds.push_back(spSlot->GetId());
    EXPECT_EQ(
        (int)NotificationPreferences::GetInstance().RemoveNotificationSlot(bundleName, NotificationConstant::OTHER),
        (int)ERR_OK);
}

/**
 * @tc.number    : NotificationPreferencesTest_00800
 * @tc.name      : AMS_ANS_RemoveNotificationSlot_0200
 * @tc.desc      : Test RemoveNotificationSlot function when bundleName is not set
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_00800, Function | SmallTest | Level1)
{
    std::string bundleName = "";
    std::vector<std::string> slotIds;
    EXPECT_EQ(
        (int)NotificationPreferences::GetInstance().RemoveNotificationSlot(bundleName, NotificationConstant::OTHER),
        (int)ERR_ANS_INVALID_PARAM);
}

/**
 * @tc.number    : NotificationPreferencesTest_00900
 * @tc.name      : AMS_ANS_RemoveNotificationSlot_0300
 * @tc.desc      : Test RemoveNotificationSlot function when slot not exist
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_00900, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    std::string slotId = "id";
    std::vector<std::string> slotIds;
    slotIds.push_back(slotId);
    TestAddNotificationSlot();
    NotificationPreferences::GetInstance().RemoveNotificationSlot(bundleName, NotificationConstant::OTHER);
    EXPECT_EQ(
        (int)NotificationPreferences::GetInstance().RemoveNotificationSlot(bundleName, NotificationConstant::OTHER),
        (int)ERR_ANS_PREFERENCES_NOTIFICATION_SLOT_NOT_EXIST);
}

/**
 * @tc.number    : NotificationPreferencesTest_01000
 * @tc.name      : AMS_ANS_RemoveNotificationSlot_0400
 * @tc.desc      : Test RemoveNotificationSlot function when file is null
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_01000, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    sptr<NotificationSlot> spSlot = new NotificationSlot(NotificationConstant::OTHER);
    std::vector<sptr<NotificationSlot>> spSlots;
    spSlots.push_back(spSlot);
    std::vector<std::string> slotIds;
    slotIds.push_back(spSlot->GetId());
    EXPECT_EQ(
        (int)NotificationPreferences::GetInstance().RemoveNotificationSlot(bundleName, NotificationConstant::OTHER),
        (int)ERR_ANS_PREFERENCES_FILE_IS_NULL);
}

/**
 * @tc.number    : NotificationPreferencesTest_01100
 * @tc.name      : AMS_ANS_RemoveNotificationSlotGroups_0100
 * @tc.desc      : Test RemoveNotificationSlotGroups function when bundle not exist
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_01100, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName1";
    sptr<NotificationSlotGroup> spSlotGroup = new NotificationSlotGroup("id1", "name");
    std::vector<sptr<NotificationSlotGroup>> spSlotGroups;
    spSlotGroups.push_back(spSlotGroup);
    NotificationPreferences::GetInstance().AddNotificationSlotGroups("bundleName", spSlotGroups);
    std::string groupId = "id1";
    std::vector<std::string> groupIds;
    groupIds.push_back(groupId);
    EXPECT_EQ((int)NotificationPreferences::GetInstance().RemoveNotificationSlotGroups(bundleName, groupIds),
        (int)ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST);
}

/**
 * @tc.number    : NotificationPreferencesTest_01200
 * @tc.name      : AMS_ANS_RemoveNotificationSlotGroups_0200
 * @tc.desc      : Test RemoveNotificationSlotGroups function the parameter is wrong
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_01200, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    std::vector<std::string> groupIds;
    EXPECT_EQ((int)NotificationPreferences::GetInstance().RemoveNotificationSlotGroups(bundleName, groupIds),
        (int)ERR_ANS_INVALID_PARAM);
}

/**
 * @tc.number    : NotificationPreferencesTest_01300
 * @tc.name      : AMS_ANS_RemoveNotificationSlotGroups_0300
 * @tc.desc      : Test RemoveNotificationSlotGroups function
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_01300, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    std::string groupId = "id";
    std::vector<std::string> groupIds;
    groupIds.push_back(groupId);
    TestAddNotificationSlotGroup();
    EXPECT_EQ(
        (int)NotificationPreferences::GetInstance().RemoveNotificationSlotGroups(bundleName, groupIds), (int)ERR_OK);
}

/**
 * @tc.number    : NotificationPreferencesTest_01400
 * @tc.name      : AMS_ANS_RemoveNotificationSlotGroups_0400
 * @tc.desc      : Test RemoveNotificationSlotGroups function when slotid invalid
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_01400, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    std::string groupId = "";
    std::vector<std::string> groupIds;
    groupIds.push_back(groupId);
    TestAddNotificationSlotGroup();
    EXPECT_EQ((int)NotificationPreferences::GetInstance().RemoveNotificationSlotGroups(bundleName, groupIds),
        (int)ERR_ANS_PREFERENCES_NOTIFICATION_SLOT_ID_INVALID);
}

/**
 * @tc.number    : NotificationPreferencesTest_01500
 * @tc.name      : AMS_ANS_UpdateNotificationSlotGroups_0100
 * @tc.desc      : Test UpdateNotificationSlotGroups function
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_01500, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    sptr<NotificationSlotGroup> spSlotGroup = new NotificationSlotGroup("id1", "name");
    std::vector<sptr<NotificationSlotGroup>> spSlotGroups;
    spSlotGroups.push_back(spSlotGroup);
    NotificationPreferences::GetInstance().AddNotificationSlotGroups(bundleName, spSlotGroups);
    EXPECT_EQ((int)NotificationPreferences::GetInstance().UpdateNotificationSlotGroups(bundleName, spSlotGroups),
        (int)ERR_OK);
}

/**
 * @tc.number    : NotificationPreferencesTest_01600
 * @tc.name      : AMS_ANS_GetNotificationSlot_0100
 * @tc.desc      : Test GetNotificationSlot function
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_01600, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    sptr<NotificationSlot> notificationSlot;
    sptr<NotificationSlot> spSlot = new NotificationSlot(NotificationConstant::OTHER);
    std::vector<sptr<NotificationSlot>> spSlots;
    spSlots.push_back(spSlot);
    NotificationPreferences::GetInstance().AddNotificationSlots(bundleName, spSlots);
    EXPECT_EQ((int)NotificationPreferences::GetInstance().GetNotificationSlot(
                  bundleName, NotificationConstant::OTHER, notificationSlot),
        (int)ERR_OK);
    EXPECT_EQ(spSlot->GetName(), notificationSlot->GetName());
}

/**
 * @tc.number    : NotificationPreferencesTest_01700
 * @tc.name      : AMS_ANS_GetNotificationAllSlots_0100
 * @tc.desc      : Test GetNotificationAllSlots function
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_01700, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    sptr<NotificationSlot> spSlot = new NotificationSlot(NotificationConstant::OTHER);
    std::vector<sptr<NotificationSlot>> spSlots;
    spSlots.push_back(spSlot);
    NotificationPreferences::GetInstance().AddNotificationSlots(bundleName, spSlots);
    std::vector<sptr<NotificationSlot>> slotsResult;
    EXPECT_EQ(
        (int)NotificationPreferences::GetInstance().GetNotificationAllSlots(bundleName, slotsResult), (int)ERR_OK);
    EXPECT_EQ((int)slotsResult.size(), 1);
}

/**
 * @tc.number    : NotificationPreferencesTest_01800
 * @tc.name      : AMS_ANS_GetNotificationSlotGroup_0100
 * @tc.desc      : Test GetNotificationSlotGroup function
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_01800, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    sptr<NotificationSlotGroup> spGroup = new NotificationSlotGroup("id", "name");
    spGroup->SetDescription("Description");
    std::vector<sptr<NotificationSlotGroup>> groups;
    groups.push_back(spGroup);
    NotificationPreferences::GetInstance().AddNotificationSlotGroups(bundleName, groups);
    sptr<NotificationSlotGroup> groupResult;
    EXPECT_EQ(
        (int)NotificationPreferences::GetInstance().GetNotificationSlotGroup(bundleName, spGroup->GetId(), groupResult),
        (int)ERR_OK);
    EXPECT_EQ(groupResult->GetDescription(), spGroup->GetDescription());
}

/**
 * @tc.number    : NotificationPreferencesTest_01900
 * @tc.name      : AMS_ANS_GetNotificationAllSlotGroups_0100
 * @tc.desc      : Test GetNotificationAllSlotGroups function
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_01900, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    TestAddNotificationSlotGroup();
    std::vector<sptr<NotificationSlotGroup>> groups;
    EXPECT_EQ(
        (int)NotificationPreferences::GetInstance().GetNotificationAllSlotGroups(bundleName, groups), (int)ERR_OK);
    EXPECT_EQ((int)groups.size(), 1);
}

/**
 * @tc.number    : NotificationPreferencesTest_02000
 * @tc.name      : AMS_ANS_GetNotificationAllSlotInSlotGroup_0100
 * @tc.desc      : Test GetNotificationAllSlotInSlotGroup function
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_02000, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    std::string groupId = "groupId";
    sptr<NotificationSlotGroup> spGroup = new NotificationSlotGroup("groupId", "name");
    std::vector<sptr<NotificationSlotGroup>> groups;
    groups.push_back(spGroup);
    NotificationPreferences::GetInstance().AddNotificationSlotGroups(bundleName, groups);
    sptr<NotificationSlot> spSlot = new NotificationSlot(NotificationConstant::OTHER);
    spSlot->SetSlotGroup(groupId);
    std::vector<sptr<NotificationSlot>> spSlots;
    spSlots.push_back(spSlot);
    NotificationPreferences::GetInstance().AddNotificationSlots(bundleName, spSlots);
    std::vector<sptr<NotificationSlot>> slotsReuslt;
    EXPECT_EQ(
        (int)NotificationPreferences::GetInstance().GetNotificationAllSlotInSlotGroup(bundleName, groupId, slotsReuslt),
        (int)ERR_OK);
    EXPECT_EQ((int)slotsReuslt.size(), 1);
}

/**
 * @tc.number    : NotificationPreferencesTest_02100
 * @tc.name      : AMS_ANS_IsShowBadge_0100
 * @tc.desc      : Test IsShowBadge function
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_02100, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    bool enable;
    TestAddNotificationSlot();
    NotificationPreferences::GetInstance().SetShowBadge(bundleName, true);
    EXPECT_EQ((int)NotificationPreferences::GetInstance().IsShowBadge(bundleName, enable), (int)ERR_OK);
    EXPECT_TRUE(enable);
}

/**
 * @tc.number    : NotificationPreferencesTest_02200
 * @tc.name      : AMS_ANS_SetShowBadge_0100
 * @tc.desc      : Test SetShowBadge function
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_02200, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    TestAddNotificationSlot();
    EXPECT_EQ((int)NotificationPreferences::GetInstance().SetShowBadge(bundleName, false), (int)ERR_OK);
}

/**
 * @tc.number    : NotificationPreferencesTest_02300
 * @tc.name      : AMS_ANS_RemoveNotificationSlotGroups_0200
 * @tc.desc      : Test RemoveNotificationSlotGroups function when bundle not exist
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_02300, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    std::string groupId = "id1";
    std::vector<std::string> groupIds;
    groupIds.push_back(groupId);
    EXPECT_EQ((int)NotificationPreferences::GetInstance().RemoveNotificationSlotGroups(bundleName, groupIds),
        (int)ERR_ANS_PREFERENCES_FILE_IS_NULL);
}

/**
 * @tc.number    : NotificationPreferencesTest_02400
 * @tc.name      : AMS_ANS_IsShowBadge_0200
 * @tc.desc      : Test IsShowBadge function when the parameter is wrong
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_02400, Function | SmallTest | Level1)
{
    std::string bundleName = "";
    bool enable;
    EXPECT_EQ((int)NotificationPreferences::GetInstance().IsShowBadge(bundleName, enable), (int)ERR_ANS_INVALID_PARAM);
}

/**
 * @tc.number    : NotificationPreferencesTest_02600
 * @tc.name      : AMS_ANS_GetImportance_0200
 * @tc.desc      : Test GetImportance function
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_02600, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    TestAddNotificationSlot();
    const int importance = 1;
    NotificationPreferences::GetInstance().SetImportance(bundleName, importance);
    int importanceResult;
    EXPECT_EQ((int)NotificationPreferences::GetInstance().GetImportance(bundleName, importanceResult), (int)ERR_OK);
    EXPECT_EQ(importanceResult, importance);
}

/**
 * @tc.number    : NotificationPreferencesTest_02700
 * @tc.name      : AMS_ANS_GetTotalBadgeNums_0100
 * @tc.desc      : Test GetTotalBadgeNums function
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_02700, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    int num = 1;
    TestAddNotificationSlot();
    NotificationPreferences::GetInstance().SetTotalBadgeNums(bundleName, num);
    int totalBadgeNum;
    EXPECT_EQ((int)NotificationPreferences::GetInstance().GetTotalBadgeNums(bundleName, totalBadgeNum), (int)ERR_OK);
    EXPECT_EQ(totalBadgeNum, num);
}

/**
 * @tc.number    : NotificationPreferencesTest_02800
 * @tc.name      : AMS_ANS_GetPrivateNotificationsAllowed_0100
 * @tc.desc      : Test GetPrivateNotificationsAllowed function
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_02800, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    TestAddNotificationSlot();
    NotificationPreferences::GetInstance().SetPrivateNotificationsAllowed(bundleName, true);
    bool allow;
    NotificationPreferences::GetInstance().GetPrivateNotificationsAllowed(bundleName, allow);
    EXPECT_EQ(allow, true);
}

/**
 * @tc.number    : NotificationPreferencesTest_02900
 * @tc.name      : AMS_ANS_GetNotificationsEnabledForBundle_0100
 * @tc.desc      : Test GetNotificationsEnabledForBundle function
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_02900, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    TestAddNotificationSlot();
    NotificationPreferences::GetInstance().SetNotificationsEnabledForBundle(bundleName, false);
    bool enabled;
    NotificationPreferences::GetInstance().GetNotificationsEnabledForBundle(bundleName, enabled);
    EXPECT_FALSE(enabled);
}

/**
 * @tc.number    : NotificationPreferencesTest_03000
 * @tc.name      : AMS_ANS_UpdateNotificationSlotGroups_0200
 * @tc.desc      : Test UpdateNotificationSlotGroups function when file is null
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_03000, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    sptr<NotificationSlotGroup> spGroup = new NotificationSlotGroup("groupId", "name");
    std::vector<sptr<NotificationSlotGroup>> groups;
    groups.push_back(spGroup);
    EXPECT_EQ((int)NotificationPreferences::GetInstance().UpdateNotificationSlotGroups(bundleName, groups),
        (int)ERR_ANS_PREFERENCES_FILE_IS_NULL);
}

/**
 * @tc.number    : NotificationPreferencesTest_03100
 * @tc.name      : AMS_ANS_RemoveNotificationForBundle_0100
 * @tc.desc      : Test RemoveNotificationForBundle function when bundleName is not set
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_03100, Function | SmallTest | Level1)
{
    std::string bundleName = "";
    EXPECT_EQ((int)NotificationPreferences::GetInstance().RemoveNotificationForBundle(bundleName),
        (int)ERR_ANS_INVALID_PARAM);
}

/**
 * @tc.number    : NotificationPreferencesTest_03200
 * @tc.name      : AMS_ANS_ClearNotificationInRestoreFactorySettings_0100
 * @tc.desc      : Test ClearNotificationInRestoreFactorySettings function when file is null
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_03200, Function | SmallTest | Level1)
{
    EXPECT_EQ((int)NotificationPreferences::GetInstance().ClearNotificationInRestoreFactorySettings(),
        (int)ERR_ANS_PREFERENCES_FILE_IS_NULL);
}

/**
 * @tc.number    : NotificationPreferencesTest_03300
 * @tc.name      : AMS_ANS_RemoveNotificationSlotGroups_0200
 * @tc.desc      : Test RemoveNotificationSlotGroups function
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_03300, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    TestAddNotificationSlotGroup();
    std::string groupId = "id";
    std::vector<std::string> groupIds;
    groupIds.push_back(groupId);
    EXPECT_EQ(
        (int)NotificationPreferences::GetInstance().RemoveNotificationSlotGroups(bundleName, groupIds), (int)ERR_OK);
}

/**
 * @tc.number    : NotificationPreferencesTest_03400
 * @tc.name      : AMS_ANS_UpdateNotificationSlots_0200
 * @tc.desc      : Test UpdateNotificationSlots function when bundleName not exist
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_03400, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName1";
    TestAddNotificationSlot();
    sptr<NotificationSlot> spSlot = new NotificationSlot(NotificationConstant::OTHER);
    std::vector<sptr<NotificationSlot>> spSlots;
    spSlots.push_back(spSlot);
    EXPECT_EQ((int)NotificationPreferences::GetInstance().UpdateNotificationSlots(bundleName, spSlots),
        (int)ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST);
}

/**
 * @tc.number    : NotificationPreferencesTest_03500
 * @tc.name      : AMS_ANS_GetNotificationSlot_0200
 * @tc.desc      : Test UpdateNotificationSlots function
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_03500, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    TestAddNotificationSlot();
    sptr<NotificationSlot> spSlot = new NotificationSlot(NotificationConstant::OTHER);
    spSlot->SetLevel(NotificationSlot::NotificationLevel::LEVEL_HIGH);
    std::vector<sptr<NotificationSlot>> spSlots;
    spSlots.push_back(spSlot);
    EXPECT_EQ((int)NotificationPreferences::GetInstance().UpdateNotificationSlots(bundleName, spSlots), (int)ERR_OK);
    sptr<NotificationSlot> updateSpSlot;
    NotificationPreferences::GetInstance().GetNotificationSlot(bundleName, NotificationConstant::OTHER, updateSpSlot);
    EXPECT_EQ(updateSpSlot->GetLevel(), spSlot->GetLevel());
}

/**
 * @tc.number    : NotificationPreferencesTest_03600
 * @tc.name      : AMS_ANS_UpdateNotificationSlots_0300
 * @tc.desc      : Test UpdateNotificationSlots function when the parameter is wrong
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_03600, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    TestAddNotificationSlot();
    std::vector<sptr<NotificationSlot>> spSlots;
    EXPECT_EQ((int)NotificationPreferences::GetInstance().UpdateNotificationSlots(bundleName, spSlots),
        (int)ERR_ANS_INVALID_PARAM);
}

/**
 * @tc.number    : NotificationPreferencesTest_03700
 * @tc.name      : AMS_ANS_UpdateNotificationSlotGroups_0300
 * @tc.desc      : Test UpdateNotificationSlotGroups function when groupid invalid
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_03700, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    TestAddNotificationSlotGroup();
    sptr<NotificationSlotGroup> spGroup = new NotificationSlotGroup("", "name");
    std::vector<sptr<NotificationSlotGroup>> groups;
    groups.push_back(spGroup);
    EXPECT_EQ((int)NotificationPreferences::GetInstance().UpdateNotificationSlotGroups(bundleName, groups),
        (int)ERR_ANS_PREFERENCES_NOTIFICATION_SLOTGROUP_ID_INVALID);
}

/**
 * @tc.number    : NotificationPreferencesTest_03800
 * @tc.name      : AMS_ANS_SetImportance_0200
 * @tc.desc      : Test SetImportance function when the parameter is wrong
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_03800, Function | SmallTest | Level1)
{
    std::string bundleName = "";
    int importance = 1;
    EXPECT_EQ(
        (int)NotificationPreferences::GetInstance().SetImportance(bundleName, importance), (int)ERR_ANS_INVALID_PARAM);
}

/**
 * @tc.number    : NotificationPreferencesTest_03900
 * @tc.name      : AMS_ANS_SetImportance_0300
 * @tc.desc      : Test SetImportance function
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_03900, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    int importance = 1;
    TestAddNotificationSlot();
    EXPECT_EQ((int)NotificationPreferences::GetInstance().SetImportance(bundleName, importance), (int)ERR_OK);
}

/**
 * @tc.number    : NotificationPreferencesTest_04000
 * @tc.name      : AMS_ANS_GetImportance_0300
 * @tc.desc      : Test GetImportance function when file is null
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_04000, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    int importanceResult;
    EXPECT_EQ((int)NotificationPreferences::GetInstance().GetImportance(bundleName, importanceResult),
        (int)ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST);
}

/**
 * @tc.number    : NotificationPreferencesTest_04100
 * @tc.name      : AMS_ANS_SetTotalBadgeNums_0200
 * @tc.desc      : Test SetTotalBadgeNums function when the parameter is wrong
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_04100, Function | SmallTest | Level1)
{
    std::string bundleName = "";
    int num = 1;
    EXPECT_EQ(
        (int)NotificationPreferences::GetInstance().SetTotalBadgeNums(bundleName, num), (int)ERR_ANS_INVALID_PARAM);
}

/**
 * @tc.number    : NotificationPreferencesTest_04200
 * @tc.name      : AMS_ANS_GetTotalBadgeNums_0300
 * @tc.desc      : Test GetTotalBadgeNums function when file is null
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_04200, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    int totalBadgeNum;
    EXPECT_EQ((int)NotificationPreferences::GetInstance().GetTotalBadgeNums(bundleName, totalBadgeNum),
        (int)ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST);
}

/**
 * @tc.number    : NotificationPreferencesTest_04300
 * @tc.name      : AMS_ANS_SetPrivateNotificationsAllowed_0200
 * @tc.desc      : Test SetPrivateNotificationsAllowed function when the parameter is wrong
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_04300, Function | SmallTest | Level1)
{
    std::string bundleName = "";
    EXPECT_EQ((int)NotificationPreferences::GetInstance().SetPrivateNotificationsAllowed(bundleName, false),
        (int)ERR_ANS_INVALID_PARAM);
}

/**
 * @tc.number    : NotificationPreferencesTest_04400
 * @tc.name      : AMS_ANS_GetPrivateNotificationsAllowed_0200
 * @tc.desc      : Test GetPrivateNotificationsAllowed function when file is null
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_04400, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    bool allow;
    EXPECT_EQ((int)NotificationPreferences::GetInstance().GetPrivateNotificationsAllowed(bundleName, allow),
        (int)ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST);
}

/**
 * @tc.number    : NotificationPreferencesTest_04500
 * @tc.name      : AMS_ANS_SetNotificationsEnabledForBundle_0200
 * @tc.desc      : Test SetNotificationsEnabledForBundle function when bundleName not exist
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_04500, Function | SmallTest | Level1)
{
    std::string bundleName = "";
    EXPECT_EQ((int)NotificationPreferences::GetInstance().SetNotificationsEnabledForBundle(bundleName, false),
        (int)ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST);
}

/**
 * @tc.number    : NotificationPreferencesTest_04600
 * @tc.name      : AMS_ANS_SetNotificationsEnabledForBundle_0300
 * @tc.desc      : Test GetNotificationsEnabledForBundle function when file is null
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_04600, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    bool enabled;
    EXPECT_EQ((int)NotificationPreferences::GetInstance().GetNotificationsEnabledForBundle(bundleName, enabled),
        (int)ERR_ANS_PREFERENCES_NOTIFICATION_BUNDLE_NOT_EXIST);
}

/**
 * @tc.number    : NotificationPreferencesTest_04700
 * @tc.name      : AMS_ANS_RemoveNotificationForBundle_0200
 * @tc.desc      : Test RemoveNotificationForBundle function
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_04700, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    TestAddNotificationSlot();
    EXPECT_EQ((int)NotificationPreferences::GetInstance().RemoveNotificationForBundle(bundleName), (int)ERR_OK);
}

/**
 * @tc.number    : NotificationPreferencesTest_04800
 * @tc.name      : AMS_ANS_AddNotificationSlotGroups_0400
 * @tc.desc      : Test AddNotificationSlotGroups function when group size more than the max
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_04800, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    sptr<NotificationSlotGroup> spGroup0 = new NotificationSlotGroup("id1", "name1");
    sptr<NotificationSlotGroup> spGroup1 = new NotificationSlotGroup("id2", "name2");
    sptr<NotificationSlotGroup> spGroup2 = new NotificationSlotGroup("id3", "name3");
    sptr<NotificationSlotGroup> spGroup3 = new NotificationSlotGroup("id4", "name4");
    sptr<NotificationSlotGroup> spGroup4 = new NotificationSlotGroup("id5", "name5");
    std::vector<sptr<NotificationSlotGroup>> groups;
    groups.push_back(spGroup0);
    groups.push_back(spGroup1);
    groups.push_back(spGroup2);
    groups.push_back(spGroup3);
    groups.push_back(spGroup4);
    EXPECT_EQ((int)NotificationPreferences::GetInstance().AddNotificationSlotGroups(bundleName, groups),
        (int)ERR_ANS_PREFERENCES_NOTIFICATION_SLOTGROUP_EXCEED_MAX_NUM);
}

/**
 * @tc.number    : NotificationPreferencesTest_04900
 * @tc.name      : AMS_ANS_UpdateNotificationSlots_0400
 * @tc.desc      : Test UpdateNotificationSlots function when file is null
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_04900, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    sptr<NotificationSlot> spSlot = new NotificationSlot(NotificationConstant::OTHER);
    std::vector<sptr<NotificationSlot>> spSlots;
    spSlots.push_back(spSlot);
    EXPECT_EQ((int)NotificationPreferences::GetInstance().UpdateNotificationSlots(bundleName, spSlots),
        (int)ERR_ANS_PREFERENCES_FILE_IS_NULL);
}

/**
 * @tc.number    : NotificationPreferencesTest_05000
 * @tc.name      : AMS_ANS_SetDisturbMode_0100
 * @tc.desc      : Test SetDisturbMode function
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_05000, Function | SmallTest | Level1)
{
    TestAddNotificationSlot();
    EXPECT_EQ((int)NotificationPreferences::GetInstance().SetDisturbMode(NotificationConstant::DisturbMode::ALLOW_ALL),
        (int)ERR_OK);
}

/**
 * @tc.number    : NotificationPreferencesTest_05100
 * @tc.name      : AMS_ANS_GetDisturbMode_0100
 * @tc.desc      : Test GetDisturbMode function when file is null
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_05100, Function | SmallTest | Level1)
{
    NotificationConstant::DisturbMode mode;
    EXPECT_EQ((int)NotificationPreferences::GetInstance().GetDisturbMode(mode), (int)ERR_ANS_PREFERENCES_FILE_IS_NULL);
}

/**
 * @tc.number    : NotificationPreferencesTest_05200
 * @tc.name      : AMS_ANS_GetDisturbMode_0200
 * @tc.desc      : Test GetDisturbMode function
 */
HWTEST_F(NotificationPreferencesTest, NotificationPreferencesTest_05200, Function | SmallTest | Level1)
{
    TestAddNotificationSlot();
    NotificationConstant::DisturbMode mode;
    EXPECT_EQ((int)NotificationPreferences::GetInstance().GetDisturbMode(mode), (int)ERR_OK);
}
}  // namespace Notification
}  // namespace OHOS
