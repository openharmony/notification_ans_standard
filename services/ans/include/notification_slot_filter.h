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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_ANS_INCLUDE_NOTIFICATION_SLOT_FILTER_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_ANS_INCLUDE_NOTIFICATION_SLOT_FILTER_H

#include "notification_filter.h"

namespace OHOS {
namespace Notification {
class NotificationSlotFilter : public INotificationFilter {
public:
    NotificationSlotFilter() {};
    ~NotificationSlotFilter() {};

    /**
     * @brief Start the filter. Inherited from INotificationFilter.
     */
    void OnStart() override;

    /**
     * @brief Stop the filter. Inherited from INotificationFilter.
     */
    void OnStop() override;

    /**
     * @brief Filters the publish operation. Inherited from INotificationFilter.
     *
     * @param record Indicates the NotificationRecord.
     * @return Returns the filter result.
     */
    ErrCode OnPublish(const std::shared_ptr<NotificationRecord> &record) override;
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_ANS_INCLUDE_NOTIFICATION_SLOT_FILTER_H