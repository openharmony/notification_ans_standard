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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_TEMPLATE_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_TEMPLATE_H

#include <memory>
#include <string>
#include "parcel.h"
#include "want_params.h"

namespace OHOS {
namespace Notification {
class NotificationTemplate : public Parcelable {
public:
    /**
     * Default constructor used to create an empty NotificationTemplate instance.
     */
    NotificationTemplate() = default;

    /**
     * Default deconstructor used to deconstruct.
     */
    ~NotificationTemplate() = default;

    /**
     * Sets the template name to be included in a template notification.
     * @param name template name.
     */
    void SetTemplateName(const std::string &name);

    /**
     * Obtains the template name to be included in a template notification.
     * @return template name.
     */
    std::string GetTemplateName() const;

    /**
     * Sets the template data to be included in a template notification.
     * @param data template data.
     */
    void SetTemplateData(const std::shared_ptr<AAFwk::WantParams> &data);

    /**
     * Obtains the template data to be included in a template notification.
     * @return template data.
     */
    std::shared_ptr<AAFwk::WantParams> GetTemplateData() const;

    /**
     * Returns a string representation of the object.
     * @return a string representation of the object.
     */
    std::string Dump();

    /**
     * Marshal a object into a Parcel.
     * @param parcel the object into the parcel
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * Unmarshal object from a Parcel.
     * @return the NotificationTemplate
     */
    static NotificationTemplate *Unmarshalling(Parcel &parcel);

private:
    /**
     * Read a NotificationTemplate object from a Parcel.
     * @param parcel the parcel
     */
    bool ReadFromParcel(Parcel &parcel);

private:
    std::string templateName_;
    std::shared_ptr<AAFwk::WantParams> templateData_ {};
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_TEMPLATE_H

