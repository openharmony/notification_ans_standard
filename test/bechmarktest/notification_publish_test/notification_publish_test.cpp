/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <benchmark/benchmark.h>
#include <string>
#include <vector>

#include "notification_action_button.h"
#include "notification_request.h"
#include "notification_content.h"
#include "notification_helper.h"
#include "want_agent_helper.h"
#include "want_agent_info.h"

using namespace OHOS;
using namespace OHOS::Notification;
using namespace OHOS::AbilityRuntime;

namespace {
class BenchmarkNotificationPublish : public benchmark::Fixture {
public:
    BenchmarkNotificationPublish()
    {
        Iterations(iterations);
        Repetitions(repetitions);
        ReportAggregatesOnly();

        InitTextRequest();
        InitWantAgentRequest();
        InitButtonRequest();
    }

    virtual ~BenchmarkNotificationPublish() override = default;

    void SetUp(const ::benchmark::State &state) override
    {}

    void TearDown(const ::benchmark::State &state) override
    {}

protected:
    void InitTextRequest();
    void InitWantAgentRequest();
    void InitButtonRequest();

    const int32_t repetitions = 1;
    const int32_t iterations = 100;

    NotificationRequest req_;
    NotificationRequest reqWantAgent_;
    NotificationRequest reqButton_;
};

void BenchmarkNotificationPublish::InitTextRequest()
{
    std::shared_ptr<NotificationMediaContent> mediaContent = std::make_shared<NotificationMediaContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(mediaContent);
    req_.SetContent(content);
    req_.SetSlotType(NotificationConstant::OTHER);
    std::vector<std::string> style;
    style.push_back("style");
    req_.SetNotificationUserInputHistory(style);
    req_.SetOwnerBundleName("bundleName");
    req_.SetLabel("Text");
}

void BenchmarkNotificationPublish::InitWantAgentRequest()
{
    std::shared_ptr<NotificationMediaContent> mediaContent = std::make_shared<NotificationMediaContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(mediaContent);
    AbilityRuntime::WantAgent::WantAgentInfo paramsInfo;
    std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> wantAgent =
        AbilityRuntime::WantAgent::WantAgentHelper::GetWantAgent(paramsInfo);
    reqWantAgent_.SetContent(content);
    reqWantAgent_.SetSlotType(NotificationConstant::OTHER);
    reqWantAgent_.SetWantAgent(wantAgent);
    reqWantAgent_.SetOwnerBundleName("bundleName");
    reqWantAgent_.SetLabel("Text");
}

void BenchmarkNotificationPublish::InitButtonRequest()
{
    std::shared_ptr<NotificationMediaContent> mediaContent = std::make_shared<NotificationMediaContent>();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(mediaContent);
    AbilityRuntime::WantAgent::WantAgentInfo paramsInfo;
    std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> wantAgent =
        AbilityRuntime::WantAgent::WantAgentHelper::GetWantAgent(paramsInfo);
    std::shared_ptr<NotificationActionButton> actionButton =
        NotificationActionButton::Create(nullptr, "title", wantAgent);
    std::shared_ptr<NotificationUserInput> onlyUserInput = NotificationUserInput::Create("onlyUserInputKey");
    AAFwk::WantParams additional;
    actionButton->AddAdditionalData(additional);
    actionButton->SetSemanticActionButton(NotificationConstant::NONE_ACTION_BUTTON);
    actionButton->SetAutoCreatedReplies(true);
    actionButton->AddMimeTypeOnlyUserInput(onlyUserInput);
    actionButton->SetContextDependent(true);
    reqButton_.SetContent(content);
    reqButton_.SetSlotType(NotificationConstant::OTHER);
    reqButton_.AddActionButton(actionButton);
    reqButton_.SetOwnerBundleName("bundleName");
    reqButton_.SetLabel("Text");
}

/**
* @tc.name: PublishNotificationTestCase001
* @tc.desc: publish text notifcation
* @tc.type: FUNC
* @tc.require:
*/
BENCHMARK_F(BenchmarkNotificationPublish, PublishNotificationTestCase001)(benchmark::State &state)
{
    int id = 0;
    while (state.KeepRunning()) {
        req_.SetNotificationId(id++);
        NotificationHelper::PublishNotification(req_);
    }
}

/**
* @tc.name: CancelNotificationTestCase
* @tc.desc: cancel notification
* @tc.type: FUNC
* @tc.require:
*/
BENCHMARK_F(BenchmarkNotificationPublish, CancelNotificationTestCase)(benchmark::State &state)
{
    int id = 100;
    while (state.KeepRunning()) {
        id--;
        NotificationHelper::CancelNotification(id);
    }
}

/**
* @tc.name: PublishNotificationTestCase002
* @tc.desc: publish wantAgent notifcation
* @tc.type: FUNC
* @tc.require:
*/
BENCHMARK_F(BenchmarkNotificationPublish, PublishNotificationTestCase002)(benchmark::State &state)
{
    int id = 0;
    while (state.KeepRunning()) {
        reqWantAgent_.SetNotificationId(id++);
        NotificationHelper::PublishNotification(reqWantAgent_);
    }
}

BENCHMARK_REGISTER_F(BenchmarkNotificationPublish, CancelNotificationTestCase)->Iterations(100)->
    Repetitions(1)->ReportAggregatesOnly();

/**
* @tc.name: PublishNotificationTestCase003
* @tc.desc: publish wantAgent notifcation
* @tc.type: FUNC
* @tc.require:
*/
BENCHMARK_F(BenchmarkNotificationPublish, PublishNotificationTestCase003)(benchmark::State &state)
{
    int id = 0;
    while (state.KeepRunning()) {
        reqButton_.SetNotificationId(id++);
        NotificationHelper::PublishNotification(reqButton_);
    }
}

BENCHMARK_REGISTER_F(BenchmarkNotificationPublish, CancelNotificationTestCase)->Iterations(100)->
    Repetitions(1)->ReportAggregatesOnly();

/**
* @tc.name: RemoveNotificationTestCase
* @tc.desc: remove notification
* @tc.type: FUNC
* @tc.require:
*/
BENCHMARK_F(BenchmarkNotificationPublish, RemoveNotificationTestCase)(benchmark::State &state)
{
    int id = 100;
    while (state.KeepRunning()) {
        id--;
        NotificationHelper::RemoveNotifications();
    }
}
}

// Run the benchmark
BENCHMARK_MAIN();