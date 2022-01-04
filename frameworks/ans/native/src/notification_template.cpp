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

#include "notification_template.h"
#include "ans_log_wrapper.h"

namespace OHOS {
namespace Notification {
void NotificationTemplate::SetTemplateName(const std::string &name)
{
    templateName_ = name;
}

std::string NotificationTemplate::GetTemplateName() const
{
    return templateName_;
}

void NotificationTemplate::SetTemplateData(const std::shared_ptr<AAFwk::WantParams> &data)
{
    templateData_ = data;
}

std::shared_ptr<AAFwk::WantParams> NotificationTemplate::GetTemplateData() const
{
    return templateData_;
}

std::string NotificationTemplate::Dump()
{
    return "templateName = " + templateName_ +
        ", templateData = " + (templateData_ ? "not null" : "null");
}

bool NotificationTemplate::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(templateName_)) {
        ANS_LOGE("Failed to write text");
        return false;
    }

    bool valid = templateData_ ? true : false;
    if (!parcel.WriteBool(valid)) {
        ANS_LOGE("Failed to write the flag which indicate whether templateData is null");
        return false;
    }

    return true;
}

NotificationTemplate *NotificationTemplate::Unmarshalling(Parcel &parcel)
{
    auto templ = new NotificationTemplate();
    if ((templ != nullptr) && !templ->ReadFromParcel(parcel)) {
        delete templ;
        templ = nullptr;
    }

    return templ;
}

bool NotificationTemplate::ReadFromParcel(Parcel &parcel)
{
    if (!parcel.ReadString(templateName_)) {
        ANS_LOGE("Failed to read template name");
        return false;
    }

    bool valid = parcel.ReadBool();
    if (valid) {
        templateData_ = std::shared_ptr<AAFwk::WantParams>(parcel.ReadParcelable<AAFwk::WantParams>());
        if (!templateData_) {
            ANS_LOGE("Failed to read template data");
            return false;
        }
    }

    return true;
}
}  // namespace Notification
}  // namespace OHOS
