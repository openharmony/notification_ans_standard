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

#include "common_event_manager.h"

#include <list>

namespace OHOS {
namespace EventFwk {
namespace {
std::list<std::shared_ptr<CommonEventSubscriber>> subscriberList;
}
bool CommonEventManager::PublishCommonEvent(const CommonEventData &data)
{
    for (auto subscriber : subscriberList) {
        subscriber->OnReceiveEvent(data);
    }
    return true;
}

bool CommonEventManager::PublishCommonEvent(const CommonEventData &data, const CommonEventPublishInfo &publishInfo)
{
    return true;
}

bool CommonEventManager::PublishCommonEvent(const CommonEventData &data, const CommonEventPublishInfo &publishInfo,
    const std::shared_ptr<CommonEventSubscriber> &subscriber)
{
    return true;
}

bool CommonEventManager::SubscribeCommonEvent(const std::shared_ptr<CommonEventSubscriber> &subscriber)
{
    subscriberList.push_back(subscriber);
    return true;
}

bool CommonEventManager::UnSubscribeCommonEvent(const std::shared_ptr<CommonEventSubscriber> &subscriber)
{
    subscriberList.remove(subscriber);
    return true;
}

bool CommonEventManager::GetStickyCommonEvent(const std::string &event, CommonEventData &commonEventData)
{
    return true;
}
}  // namespace EventFwk
}  // namespace OHOS