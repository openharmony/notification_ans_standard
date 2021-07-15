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

#include "permission_filter.h"
#include <functional>
#include <gtest/gtest.h>

#include "ans_inner_errors.h"
#include "notification_preferences.h"
#include "notification_slot.h"
#include "string_ex.h"

using namespace testing::ext;
namespace OHOS {
namespace Notification {

class PermissionFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void PermissionFilterTest::SetUpTestCase()
{}

void PermissionFilterTest::TearDownTestCase()
{}

void PermissionFilterTest::SetUp()
{
    NotificationPreferences::GetInstance().ClearNotificationInRestoreFactorySettings();
}

void PermissionFilterTest::TearDown()
{
    NotificationPreferences::GetInstance().ClearNotificationInRestoreFactorySettings();
}

/**
 * @tc.number    : PermissionFilterTest_00100
 * @tc.name      : AMS_ANS_OnStop_0100
 * @tc.desc      : Test OnStart function
 */
HWTEST_F(PermissionFilterTest, PermissionFilterTest_00100, Function | SmallTest | Level1)
{
    PermissionFilter permissionFilter;
    permissionFilter.OnStart();
}

/**
 * @tc.number    : PermissionFilterTest_00200
 * @tc.name      : AMS_ANS_OnStop_0100
 * @tc.desc      : Test OnStop function
 */
HWTEST_F(PermissionFilterTest, PermissionFilterTest_00200, Function | SmallTest | Level1)
{
    PermissionFilter permissionFilter;
    permissionFilter.OnStop();
}

/**
 * @tc.number    : PermissionFilterTest_00300
 * @tc.name      : AMS_ANS_OnPublish_0100
 * @tc.desc      : Test OnPublish function when the param is wrong
 */
HWTEST_F(PermissionFilterTest, PermissionFilterTest_00300, Function | SmallTest | Level1)
{
    PermissionFilter permissionFilter;
    sptr<NotificationRequest> request = new NotificationRequest();
    auto objptr = new Notification(request);
    EXPECT_EQ(permissionFilter.OnPublish(objptr), (int)ERR_ANS_INVALID_PARAM);
}

/**
 * @tc.number    : PermissionFilterTest_00400
 * @tc.name      : AMS_ANS_OnPublish_0200
 * @tc.desc      : Test OnPublish function
 */
HWTEST_F(PermissionFilterTest, PermissionFilterTest_00400, Function | SmallTest | Level1)
{
    PermissionFilter permissionFilter;
    std::string bundleName = "bundleName";
    sptr<NotificationSlot> slot = new NotificationSlot(NotificationConstant::OTHER);
    std::vector<sptr<NotificationSlot>> slots;
    slots.push_back(slot);
    NotificationPreferences::GetInstance().AddNotificationSlots(bundleName, slots);
    sptr<NotificationRequest> request = new NotificationRequest();
    request->SetOwnerBundleName(bundleName);
    sptr<Notification> notification = new Notification(request);
    EXPECT_EQ(permissionFilter.OnPublish(notification), ERR_OK);
}

/**
 * @tc.number    : PermissionFilterTest_00500
 * @tc.name      : AMS_ANS_OnPublish_0300
 * @tc.desc      : Test OnPublish function when file is null
 */
HWTEST_F(PermissionFilterTest, PermissionFilterTest_00500, Function | SmallTest | Level1)
{
    PermissionFilter permissionFilter;
    std::string bundleName = "bundleName";
    sptr<NotificationSlot> slot = new NotificationSlot(NotificationConstant::OTHER);
    std::vector<sptr<NotificationSlot>> slots;
    slots.push_back(slot);
    NotificationPreferences::GetInstance().AddNotificationSlots(bundleName, slots);
    sptr<NotificationRequest> request = new NotificationRequest();
    request->SetOwnerBundleName("bundleName1");
    sptr<Notification> notification = new Notification(request);
    EXPECT_EQ((int)permissionFilter.OnPublish(notification), (int)ERR_OK);
}

/**
 * @tc.number    : PermissionFilterTest_00600
 * @tc.name      : AMS_ANS_OnPublish_0400
 * @tc.desc      : Test OnPublish function when NotificationsEnabled is false
 */
HWTEST_F(PermissionFilterTest, PermissionFilterTest_00600, Function | SmallTest | Level1)
{
    PermissionFilter permissionFilter;
    std::string bundleName = "bundleName";
    sptr<NotificationSlot> slot = new NotificationSlot(NotificationConstant::OTHER);
    std::vector<sptr<NotificationSlot>> slots;
    slots.push_back(slot);
    NotificationPreferences::GetInstance().AddNotificationSlots(bundleName, slots);
    NotificationPreferences::GetInstance().SetNotificationsEnabledForBundle(bundleName, false);
    sptr<NotificationRequest> request = new NotificationRequest();
    request->SetOwnerBundleName(bundleName);
    sptr<Notification> notification = new Notification(request);
    EXPECT_EQ((int)permissionFilter.OnPublish(notification), (int)ERR_ANS_NOT_ALLOWED);
}
}  // namespace Notification
}  // namespace OHOS