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
#ifndef NOTIFICATION_FUZZTESTMANAGER_H
#define NOTIFICATION_FUZZTESTMANAGER_H

#include <map>
#include <unordered_map>

#include "nlohmann/json.hpp"
namespace OHOS {
namespace Notification {
class NotificationFuzzTestManager {
public:
    using Ptr = std::shared_ptr<NotificationFuzzTestManager>;

    /**
     * @brief Default deconstructor used to deconstruct.
     */
    ~NotificationFuzzTestManager()
    {}

    /**
     * @brief Obtains the instance of the fuzz test.
     *
     * @return Returns the instance of the fuzz test.
     */
    static Ptr GetInstance()
    {
        if (instance == nullptr) {
            NotificationFuzzTestManager* pObj = new NotificationFuzzTestManager();
            instance = std::shared_ptr<NotificationFuzzTestManager>(pObj);
        }
        return instance;
    }

    /**
     * @brief Starts the fuzz test.
     */
    void StartFuzzTest();

private:
    void SetJsonFunction(std::string functionName);
    void SetCycle(uint16_t cycle);
    NotificationFuzzTestManager();
    NotificationFuzzTestManager(const NotificationFuzzTestManager &) = delete;
    NotificationFuzzTestManager &operator=(const NotificationFuzzTestManager &) = delete;
    static Ptr instance;
    uint16_t cycle_ = 0;
    std::unordered_map<std::string, int> remainderMap_ {};
    std::unordered_map<std::string, std::function<void()>> callFunctionMap_ {};

    const int COLOR_R = 100;
    const int COLOR_G = 100;
    const int COLOR_B = 100;
    void RegisterNotificationHelper();
    void RegisterNotificationSorting();
    void RegisterNotificationSortingMap();
    void RegisterNotificationSubscribeInfo();
    void RegisterNotificationSubscriber();
    void RegisterWantAgentHelper();
    void RegisterLauncherService();
    void RegisterNapiBundlemgr();
    void RegisterIAbilityContinuation();
    void RegisterRevocable();
    void RegisterTaskDispatcher();
    void RegisterAbility();
    void RegisterAbilityContext();
    void RegisterContext();
    void RegisterAbilityLifecycleCallbacks();
    void RegisterIAbilityManager();
    friend class TestAnsSubscriber;
};
}  // namespace EventFwk
}  // namespace OHOS

#endif