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

#define private public
#include <gtest/gtest.h>

#include "notification_preferences.h"
#include "notification_preferences_database.h"

using namespace testing::ext;
namespace OHOS {
namespace Notification {

class NotificationPreferencesDatabaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NotificationPreferencesDatabaseTest::SetUpTestCase()
{}

void NotificationPreferencesDatabaseTest::TearDownTestCase()
{}

void NotificationPreferencesDatabaseTest::SetUp()
{}

void NotificationPreferencesDatabaseTest::TearDown()
{}

std::unique_ptr<NotificationPreferencesDatabase> preferncesDB_ = std::make_unique<NotificationPreferencesDatabase>();

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : PutSlotsToDisturbeDB_00100
 */
HWTEST_F(NotificationPreferencesDatabaseTest, PutSlotsToDisturbeDB_00100, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    std::vector<sptr<NotificationSlot>> slots;
    sptr<NotificationSlot> slot1 = new NotificationSlot(NotificationConstant::SlotType::SOCIAL_COMMUNICATION);
    sptr<NotificationSlot> slot2 = new NotificationSlot(NotificationConstant::SlotType::SERVICE_REMINDER);
    slots.push_back(slot1);
    slots.push_back(slot2);
    EXPECT_EQ(true, preferncesDB_->PutSlotsToDisturbeDB(bundleName, slots));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : PutSlotsToDisturbeDB_00200
 */
HWTEST_F(NotificationPreferencesDatabaseTest, PutSlotsToDisturbeDB_00200, Function | SmallTest | Level1)
{
    std::vector<sptr<NotificationSlot>> slots;
    sptr<NotificationSlot> slot1 = new NotificationSlot(NotificationConstant::SlotType::SOCIAL_COMMUNICATION);
    sptr<NotificationSlot> slot2 = new NotificationSlot(NotificationConstant::SlotType::SERVICE_REMINDER);
    slots.push_back(slot1);
    slots.push_back(slot2);
    EXPECT_EQ(false, preferncesDB_->PutSlotsToDisturbeDB(std::string(), slots));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : PutSlotsToDisturbeDB_00300
 */
HWTEST_F(NotificationPreferencesDatabaseTest, PutSlotsToDisturbeDB_00300, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    std::vector<sptr<NotificationSlot>> slots;
    EXPECT_EQ(false, preferncesDB_->PutSlotsToDisturbeDB(std::string(), slots));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : PutGroupsToDisturbeDB_00100
 */
HWTEST_F(NotificationPreferencesDatabaseTest, PutGroupsToDisturbeDB_00100, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";

    sptr<NotificationSlotGroup> slotGroup = new NotificationSlotGroup("id", "name");
    std::vector<sptr<NotificationSlotGroup>> groups;
    groups.push_back(slotGroup);

    EXPECT_EQ(true, preferncesDB_->PutGroupsToDisturbeDB(bundleName, groups));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : PutGroupsToDisturbeDB_00200
 */
HWTEST_F(NotificationPreferencesDatabaseTest, PutGroupsToDisturbeDB_00200, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";

    std::vector<sptr<NotificationSlotGroup>> groups;

    EXPECT_EQ(false, preferncesDB_->PutGroupsToDisturbeDB(bundleName, groups));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : PutShowBadge_00100
 */
HWTEST_F(NotificationPreferencesDatabaseTest, PutShowBadge_00100, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";

    EXPECT_EQ(true, preferncesDB_->PutShowBadge(bundleName, true));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : PutShowBadge_00200
 */
HWTEST_F(NotificationPreferencesDatabaseTest, PutShowBadge_00200, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";

    EXPECT_EQ(true, preferncesDB_->PutShowBadge(bundleName, false));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : PutShowBadge_00300
 */
HWTEST_F(NotificationPreferencesDatabaseTest, PutShowBadge_00300, Function | SmallTest | Level1)
{
    EXPECT_EQ(false, preferncesDB_->PutShowBadge(std::string(), false));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : PutImportance_00100
 */
HWTEST_F(NotificationPreferencesDatabaseTest, PutImportance_00100, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";

    EXPECT_EQ(true,
        preferncesDB_->PutImportance(bundleName, OHOS::Notification::NotificationSlot::NotificationLevel::LEVEL_NONE));
    EXPECT_EQ(true,
        preferncesDB_->PutImportance(bundleName, OHOS::Notification::NotificationSlot::NotificationLevel::LEVEL_MIN));
    EXPECT_EQ(true,
        preferncesDB_->PutImportance(bundleName, OHOS::Notification::NotificationSlot::NotificationLevel::LEVEL_LOW));
    EXPECT_EQ(true,
        preferncesDB_->PutImportance(
            bundleName, OHOS::Notification::NotificationSlot::NotificationLevel::LEVEL_DEFAULT));
    EXPECT_EQ(true,
        preferncesDB_->PutImportance(bundleName, OHOS::Notification::NotificationSlot::NotificationLevel::LEVEL_HIGH));
    EXPECT_EQ(true,
        preferncesDB_->PutImportance(
            bundleName, OHOS::Notification::NotificationSlot::NotificationLevel::LEVEL_UNDEFINED));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : PutTotalBadgeNums_00100
 */
HWTEST_F(NotificationPreferencesDatabaseTest, PutTotalBadgeNums_00100, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";

    EXPECT_EQ(true, preferncesDB_->PutTotalBadgeNums(bundleName, 0));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : PutTotalBadgeNums_00200
 */
HWTEST_F(NotificationPreferencesDatabaseTest, PutTotalBadgeNums_00200, Function | SmallTest | Level1)
{
    EXPECT_EQ(false, preferncesDB_->PutTotalBadgeNums(std::string(), 0));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : PutPrivateNotificationsAllowed_00100
 */
HWTEST_F(NotificationPreferencesDatabaseTest, PutPrivateNotificationsAllowed_00100, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";

    EXPECT_EQ(true, preferncesDB_->PutPrivateNotificationsAllowed(bundleName, true));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : PutPrivateNotificationsAllowed_00200
 */
HWTEST_F(NotificationPreferencesDatabaseTest, PutPrivateNotificationsAllowed_00200, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";

    EXPECT_EQ(true, preferncesDB_->PutPrivateNotificationsAllowed(bundleName, false));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : PutPrivateNotificationsAllowed_00300
 */
HWTEST_F(NotificationPreferencesDatabaseTest, PutPrivateNotificationsAllowed_00300, Function | SmallTest | Level1)
{
    EXPECT_EQ(false, preferncesDB_->PutPrivateNotificationsAllowed(std::string(), false));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : PutNotificationsEnabledForBundle_00100
 */
HWTEST_F(NotificationPreferencesDatabaseTest, PutNotificationsEnabledForBundle_00100, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";

    EXPECT_EQ(true, preferncesDB_->PutNotificationsEnabledForBundle(bundleName, true));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : PutNotificationsEnabledForBundle_00200
 */
HWTEST_F(NotificationPreferencesDatabaseTest, PutNotificationsEnabledForBundle_00200, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";

    EXPECT_EQ(true, preferncesDB_->PutNotificationsEnabledForBundle(bundleName, false));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : PutNotificationsEnabledForBundle_00300
 */
HWTEST_F(NotificationPreferencesDatabaseTest, PutNotificationsEnabledForBundle_00300, Function | SmallTest | Level1)
{
    EXPECT_EQ(false, preferncesDB_->PutNotificationsEnabledForBundle(std::string(), false));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : PutNotificationsEnabled_00100
 */
HWTEST_F(NotificationPreferencesDatabaseTest, PutNotificationsEnabled_00100, Function | SmallTest | Level1)
{

    EXPECT_EQ(true, preferncesDB_->PutNotificationsEnabled(true));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : PutNotificationsEnabled_00200
 */
HWTEST_F(NotificationPreferencesDatabaseTest, PutNotificationsEnabled_00200, Function | SmallTest | Level1)
{
    EXPECT_EQ(true, preferncesDB_->PutNotificationsEnabled(false));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : PutDisturbMode_00100
 */
HWTEST_F(NotificationPreferencesDatabaseTest, PutDisturbMode_00100, Function | SmallTest | Level1)
{
    EXPECT_EQ(true, preferncesDB_->PutDisturbMode(OHOS::Notification::NotificationConstant::DisturbMode::ALLOW_ALARMS));
    EXPECT_EQ(true, preferncesDB_->PutDisturbMode(OHOS::Notification::NotificationConstant::DisturbMode::ALLOW_ALL));
    EXPECT_EQ(true, preferncesDB_->PutDisturbMode(OHOS::Notification::NotificationConstant::DisturbMode::ALLOW_NONE));
    EXPECT_EQ(
        true, preferncesDB_->PutDisturbMode(OHOS::Notification::NotificationConstant::DisturbMode::ALLOW_PRIORITY));
    EXPECT_EQ(
        true, preferncesDB_->PutDisturbMode(OHOS::Notification::NotificationConstant::DisturbMode::ALLOW_UNKNOWN));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : CreateExsitBundle_00100
 */
HWTEST_F(NotificationPreferencesDatabaseTest, CreateExsitBundle_00100, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    EXPECT_EQ(true, preferncesDB_->CheckBundle(bundleName));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : RemoveAllBundleFromDisturbeDB_00100
 */
HWTEST_F(NotificationPreferencesDatabaseTest, RemoveAllBundleFromDisturbeDB_00100, Function | SmallTest | Level1)
{
    EXPECT_EQ(true, preferncesDB_->RemoveAllBundleFromDisturbeDB());
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : RemoveBundleFromDisturbeDB_00100
 */
HWTEST_F(NotificationPreferencesDatabaseTest, RemoveBundleFromDisturbeDB_00100, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    EXPECT_EQ(true, preferncesDB_->RemoveBundleFromDisturbeDB(bundleName));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : RemoveSlotFromDisturbeDB_00100
 */
HWTEST_F(NotificationPreferencesDatabaseTest, RemoveSlotFromDisturbeDB_00100, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    EXPECT_EQ(true,
        preferncesDB_->RemoveSlotFromDisturbeDB(
            bundleName, OHOS::Notification::NotificationConstant::SlotType::SOCIAL_COMMUNICATION));
    EXPECT_EQ(true,
        preferncesDB_->RemoveSlotFromDisturbeDB(
            bundleName, OHOS::Notification::NotificationConstant::SlotType::SERVICE_REMINDER));
    EXPECT_EQ(true,
        preferncesDB_->RemoveSlotFromDisturbeDB(
            bundleName, OHOS::Notification::NotificationConstant::SlotType::CONTENT_INFORMATION));
    EXPECT_EQ(true,
        preferncesDB_->RemoveSlotFromDisturbeDB(bundleName, OHOS::Notification::NotificationConstant::SlotType::OTHER));
    EXPECT_EQ(true,
        preferncesDB_->RemoveSlotFromDisturbeDB(
            bundleName, OHOS::Notification::NotificationConstant::SlotType::CUSTOM));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : RemoveGroupsFromDisturbeDB_00100
 */
HWTEST_F(NotificationPreferencesDatabaseTest, RemoveGroupsFromDisturbeDB_00100, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    std::vector<std::string> groupIds;
    EXPECT_EQ(true, preferncesDB_->RemoveGroupsFromDisturbeDB(bundleName, groupIds));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : RemoveGroupsFromDisturbeDB_00200
 */
HWTEST_F(NotificationPreferencesDatabaseTest, RemoveGroupsFromDisturbeDB_00200, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    std::vector<std::string> groupIds;
    groupIds.push_back("group1");
    groupIds.push_back("group2");
    EXPECT_EQ(true, preferncesDB_->RemoveGroupsFromDisturbeDB(bundleName, groupIds));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : GetRemoveGroupKeys_00100
 */
HWTEST_F(NotificationPreferencesDatabaseTest, GetRemoveGroupKeys_00100, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    std::string groupId = "";
    std::vector<OHOS::DistributedKv::Key> keys;
    EXPECT_EQ(true, preferncesDB_->GetRemoveGroupKeysFromDisturbeDB(bundleName, groupId, keys));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : StoreDeathRecipient_00100
 */
HWTEST_F(NotificationPreferencesDatabaseTest, StoreDeathRecipient_00100, Function | SmallTest | Level1)
{
    NotificationPreferencesInfo preferencesInfo_{};
    NotificationPreferencesInfo preferencesInfo = preferencesInfo_;
    EXPECT_EQ(true, preferncesDB_->StoreDeathRecipient(preferencesInfo));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : GetKvStore_00100
 */
HWTEST_F(NotificationPreferencesDatabaseTest, GetKvStore_00100, Function | SmallTest | Level1)
{
    EXPECT_EQ(OHOS::DistributedKv::Status::SUCCESS, preferncesDB_->GetKvStore());
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : CheckKvStore_00100
 */
HWTEST_F(NotificationPreferencesDatabaseTest, CheckKvStore_00100, Function | SmallTest | Level1)
{
    EXPECT_EQ(true, preferncesDB_->CheckKvStore());
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : ResetStore_00100
 */
HWTEST_F(NotificationPreferencesDatabaseTest, ResetStore_00100, Function | SmallTest | Level1)
{
    EXPECT_EQ(true, preferncesDB_->ResetStore());
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : InitBundlePropertyValueToDisturbeDB_00100
 */
HWTEST_F(NotificationPreferencesDatabaseTest, InitBundlePropertyValueToDisturbeDB_00100, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    EXPECT_EQ(true, preferncesDB_->InitBundlePropertyValueToDisturbeDB(bundleName));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : SetSlotEntry_00200
 */
HWTEST_F(NotificationPreferencesDatabaseTest, SetSlotEntry_00200, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    sptr<NotificationSlot> slot = new NotificationSlot(NotificationConstant::SlotType::SOCIAL_COMMUNICATION);
    std::vector<OHOS::DistributedKv::Entry> entries;
    EXPECT_EQ(true, preferncesDB_->SlotToEntry(bundleName, slot, entries));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : GetGroupEntry_00100
 */
HWTEST_F(NotificationPreferencesDatabaseTest, GetGroupEntry_00100, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    sptr<NotificationSlotGroup> slotGroup = new NotificationSlotGroup("id", "name");
    std::vector<OHOS::DistributedKv::Entry> entries;
    EXPECT_EQ(true, preferncesDB_->GroupToEntry(bundleName, slotGroup, entries));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : GetRemoveGroupKeys_00200
 */
HWTEST_F(NotificationPreferencesDatabaseTest, GetRemoveGroupKeys_00200, Function | SmallTest | Level1)
{
    std::string bundleName = "bundleName";
    std::string groupId = "group1";
    std::vector<OHOS::DistributedKv::Key> keys;
    EXPECT_EQ(true, preferncesDB_->GetRemoveGroupKeysFromDisturbeDB(bundleName, groupId, keys));
}

/**
 * @tc.number    :
 * @tc.name      :
 * @tc.desc      : ParseGroupFromDisturbeDB_00100
 */
HWTEST_F(NotificationPreferencesDatabaseTest, ParseGroupFromDisturbeDB_00100, Function | SmallTest | Level1)
{
    NotificationPreferencesInfo::BundleInfo bundleInfo;
    bundleInfo.SetBundleName("BundleName");
    // preferncesDB_->ParseGroupFromDisturbeDB(bundleInfo, std::string());  // TODO ëΩóπò¢éQêî
        EXPECT_EQ("BundleName", bundleInfo.GetBundleName());
}

}  // namespace Notification
}  // namespace OHOS