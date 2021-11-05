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

#include "ans_slotgroup_test.h"
#include "notification_slot_group.h"

namespace OHOS {
namespace AppExecFwk {
void AnsSlotGroupSystemTest::OnStart(const Want &want)
{
    APP_LOGI("====>Ans SlotGroup test start:====>");
    Notification::NotificationSlotGroup slotGroupTest("id_test", "name_test");
    if (slotGroupTest.GetId().empty()) {
        ANS_LOGI("====>Notification slot group id is invalid====>");
    } else {
        ANS_LOGI("====>Notification slot group id is====> %{public}s", slotGroupTest.GetId().c_str());
    }
    ErrCode errAddGroupTest = Notification::NotificationHelper::AddNotificationSlotGroup(slotGroupTest);
    APP_LOGI("====>add a SlotGroup with id name id_test, ErrCode is====> %{public}d", errAddGroupTest);

    Notification::NotificationSlot slotFirst(Notification::NotificationConstant::SlotType::SOCIAL_COMMUNICATION);
    Notification::NotificationSlot slotSecond(Notification::NotificationConstant::SlotType::SERVICE_REMINDER);
    Notification::NotificationSlot slotThird(Notification::NotificationConstant::SlotType::CONTENT_INFORMATION);
    Notification::NotificationSlot slotFourth(Notification::NotificationConstant::SlotType::OTHER);
    Notification::NotificationSlot slotFifth(Notification::NotificationConstant::SlotType::CUSTOM);

    slotFirst.SetSlotGroup("id_test");
    slotSecond.SetSlotGroup("id_test");
    slotThird.SetSlotGroup("id_test");
    slotFourth.SetSlotGroup("id_test");
    slotFifth.SetSlotGroup("id_test");

    ErrCode errAddSlotFirst = Notification::NotificationHelper::AddNotificationSlot(slotFirst);
    ErrCode errAddSlotSecond = Notification::NotificationHelper::AddNotificationSlot(slotSecond);
    ErrCode errAddSlotThird = Notification::NotificationHelper::AddNotificationSlot(slotThird);
    ErrCode errAddSlotFourth = Notification::NotificationHelper::AddNotificationSlot(slotFourth);
    ErrCode errAddSlotFifth = Notification::NotificationHelper::AddNotificationSlot(slotFifth);

    APP_LOGI("====>add SOCIAL_COMMUNICATION type slot, ErrCode is====> %{public}d", errAddSlotFirst);
    APP_LOGI("====>add SERVICE_REMINDER type slot, ErrCode is====> %{public}d", errAddSlotSecond);
    APP_LOGI("====>add CONTENT_INFORMATION type slot, ErrCode is====> %{public}d", errAddSlotThird);
    APP_LOGI("====>add OTHER type slot, ErrCode is====> %{public}d", errAddSlotFourth);
    APP_LOGI("====>add CUSTOM type slot, ErrCode is====> %{public}d", errAddSlotFifth);

    sptr<Notification::NotificationSlotGroup> group;
    ErrCode errcodeGet = Notification::NotificationHelper::GetNotificationSlotGroup("id_test", group);
    APP_LOGI("====>get SlotGroup after adding SlotGroup and slots, ErrCode is====> %{public}d", errcodeGet);
    if (group != nullptr) {
        std::string dumpGroupStr = group->Dump();
        APP_LOGI("====>obtained group dump====> %{public}s", dumpGroupStr.c_str());
        APP_LOGI("====>the number of slots in the obtained group====> %{public}d", group->GetSlots().size());

        ErrCode errcodeRem = Notification::NotificationHelper::RemoveNotificationSlotGroup("id_test");
        APP_LOGI("====>remove the group whose id is id_test, ErrCode is====> %{public}d", errcodeRem);
        sptr<Notification::NotificationSlotGroup> groupTwo;
        ErrCode errcodeGetTwo = Notification::NotificationHelper::GetNotificationSlotGroup("id_test", groupTwo);
        APP_LOGI("====>get SlotGroup after removing SlotGroup, ErrCode is====> %{public}d", errcodeGetTwo);

        ErrCode errAddGroupTestSecond = Notification::NotificationHelper::AddNotificationSlotGroup(slotGroupTest);
        APP_LOGI("====>add SlotGroup again with id name id_test, ErrCode is====> %{public}d", errAddGroupTestSecond);
        sptr<Notification::NotificationSlotGroup> groupSecond;
        ErrCode errcodeGetSecond = Notification::NotificationHelper::GetNotificationSlotGroup("id_test", groupSecond);
        APP_LOGI("====>get SlotGroup again after adding SlotGroup again, ErrCode is: %{public}d", errcodeGetSecond);
        if (groupSecond != nullptr) {
            dumpGroupStr = groupSecond->Dump();
            APP_LOGI("====>the second time obtained group dump====> %{public}s", dumpGroupStr.c_str());
            APP_LOGI("====>the number of slots in second obtained group: %{public}d", groupSecond->GetSlots().size());

            Notification::NotificationSlotGroup slotGroupFirst("id_first", "name_first");
            ErrCode errAddGroupFirst = Notification::NotificationHelper::AddNotificationSlotGroup(slotGroupFirst);
            APP_LOGI("====>add SlotGroup with id name id_first, ErrCode is====> %{public}d", errAddGroupFirst);

            Notification::NotificationSlotGroup slotGroupSecond("id_second", "name_second");
            ErrCode errAddGroupSecond = Notification::NotificationHelper::AddNotificationSlotGroup(slotGroupSecond);
            APP_LOGI("====>add SlotGroup with id name id_second, ErrCode is====> %{public}d", errAddGroupSecond);

            Notification::NotificationSlotGroup slotGroupThird("id_third", "name_third");
            ErrCode errAddGroupThird = Notification::NotificationHelper::AddNotificationSlotGroup(slotGroupThird);
            APP_LOGI("====>add SlotGroup with id name id_third, ErrCode is====> %{public}d", errAddGroupThird);

            Notification::NotificationSlotGroup slotGroupFourth("id_fourth", "name_fourth");
            ErrCode errAddGroupFourth = Notification::NotificationHelper::AddNotificationSlotGroup(slotGroupFourth);
            APP_LOGI("====>add SlotGroup with id name id_fourth, ErrCode is====> %{public}d", errAddGroupFourth);

            Notification::NotificationSlotGroup slotGroupFifth("id_fifth", "name_fifth");
            ErrCode errAddGroupFifth = Notification::NotificationHelper::AddNotificationSlotGroup(slotGroupFifth);
            APP_LOGI("====>add SlotGroup with id name id_fifth, ErrCode is====> %{public}d", errAddGroupFifth);
        } else {
            ANS_LOGI("====>fail to get group after adding SlotGroup for the second time====>");
        }
    } else {
        ANS_LOGI("====>fail to get group after adding SlotGroup and slots for the first time====>");
    }

    Ability::OnStart(want);
}

void AnsSlotGroupSystemTest::OnNewWant(const Want &want)
{
    GetWantInfo(want);
    Ability::OnNewWant(want);
}

void AnsSlotGroupSystemTest::OnForeground(const Want &want)
{
    GetWantInfo(want);
    Ability::OnForeground(want);
}

void AnsSlotGroupSystemTest::OnStop()
{
    Ability::OnStop();
}

void AnsSlotGroupSystemTest::OnActive()
{
    Ability::OnActive();
    if (std::string::npos != shouldReturn.find(GetAbilityName())) {
        TerminateAbility();
    }
    Clear();
}

void AnsSlotGroupSystemTest::OnInactive()
{
    Ability::OnInactive();
}

void AnsSlotGroupSystemTest::OnBackground()
{
    Ability::OnBackground();
}

void AnsSlotGroupSystemTest::Clear()
{
    shouldReturn = "";
    targetBundle = "";
    targetAbility = "";
}

void AnsSlotGroupSystemTest::GetWantInfo(const Want &want)
{
    Want mWant(want);
    shouldReturn = mWant.GetStringParam("shouldReturn");
    targetBundle = mWant.GetStringParam("targetBundle");
    targetAbility = mWant.GetStringParam("targetAbility");
}

REGISTER_AA(AnsSlotGroupSystemTest)
}  // namespace AppExecFwk
}  // namespace OHOS