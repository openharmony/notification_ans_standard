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

#include "notification_content.h"
#include "ans_log_wrapper.h"

namespace OHOS {
namespace Notification {
NotificationContent::NotificationContent(const std::shared_ptr<NotificationNormalContent> &normalContent)
{
    if (!normalContent) {
        ANS_LOGE("NotificationNormalContent can not be null");
        return;
    }

    contentType_ = NotificationContent::Type::BASIC_TEXT;
    content_ = normalContent;
}

NotificationContent::NotificationContent(const std::shared_ptr<NotificationLongTextContent> &longTextContent)
{
    if (!longTextContent) {
        ANS_LOGE("NotificationLongTextContent can not be null");
        return;
    }

    contentType_ = NotificationContent::Type::LONG_TEXT;
    content_ = longTextContent;
}

NotificationContent::NotificationContent(const std::shared_ptr<NotificationPictureContent> &pictureContent)
{
    if (!pictureContent) {
        ANS_LOGE("NotificationPictureContent can not be null");
        return;
    }

    contentType_ = NotificationContent::Type::PICTURE;
    content_ = pictureContent;
}

NotificationContent::NotificationContent(const std::shared_ptr<NotificationConversationalContent> &conversationContent)
{
    if (!conversationContent) {
        ANS_LOGE("NotificationConversationalContent can not be null");
        return;
    }

    contentType_ = NotificationContent::Type::CONVERSATION;
    content_ = conversationContent;
}

NotificationContent::NotificationContent(const std::shared_ptr<NotificationMultiLineContent> &multiLineContent)
{
    if (!multiLineContent) {
        ANS_LOGE("NotificationMultiLineContent can not be null");
        return;
    }

    contentType_ = NotificationContent::Type::MULTILINE;
    content_ = multiLineContent;
}

NotificationContent::NotificationContent(const std::shared_ptr<NotificationMediaContent> &mediaContent)
{
    if (!mediaContent) {
        ANS_LOGE("NotificationMediaContent can not be null");
        return;
    }

    contentType_ = NotificationContent::Type::MEDIA;
    content_ = mediaContent;
}

NotificationContent::~NotificationContent()
{}

NotificationContent::Type NotificationContent::GetContentType() const
{
    return contentType_;
}

std::shared_ptr<NotificationBasicContent> NotificationContent::GetNotificationContent() const
{
    return content_;
}

std::string NotificationContent::Dump()
{
    std::string contentTypeStr =   (contentType_ == NotificationContent::Type::BASIC_TEXT)   ? "BASIC_TEXT"
                                 : (contentType_ == NotificationContent::Type::CONVERSATION) ? "CONVERSATION"
                                 : (contentType_ == NotificationContent::Type::LONG_TEXT)    ? "LONG_TEXT"
                                 : (contentType_ == NotificationContent::Type::MEDIA)        ? "MEDIA"
                                 : (contentType_ == NotificationContent::Type::MULTILINE)    ? "MULTILINE"
                                 : (contentType_ == NotificationContent::Type::PICTURE)      ? "PICTURE" : "NONE";

    return "NotificationContent{ "
            "contentType = " + contentTypeStr +
            ", content = " + (content_ ? content_->Dump() : "null") +
            " }";
}

bool NotificationContent::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteInt32(static_cast<int32_t>(contentType_))) {
        ANS_LOGE("Failed to write contentType");
        return false;
    }

    auto valid = content_ ? true : false;
    if (!parcel.WriteBool(valid)) {
        ANS_LOGE("Failed to write the flag which indicate whether content is null");
        return false;
    }

    if (valid) {
        if (!parcel.WriteParcelable(content_.get())) {
            ANS_LOGE("Failed to write content");
            return false;
        }
    }

    return true;
}

NotificationContent *NotificationContent::Unmarshalling(Parcel &parcel)
{
    auto pContent = new (std::nothrow) NotificationContent();
    if ((pContent != nullptr) && !pContent->ReadFromParcel(parcel)) {
        delete pContent;
        pContent = nullptr;
    }

    return pContent;
}

bool NotificationContent::ReadFromParcel(Parcel &parcel)
{
    contentType_ = static_cast<NotificationContent::Type>(parcel.ReadInt32());

    auto valid = parcel.ReadBool();
    if (!valid) {
        return true;
    }

    switch (contentType_) {
        case NotificationContent::Type::BASIC_TEXT:
            content_ = std::static_pointer_cast<NotificationBasicContent>(
                std::shared_ptr<NotificationNormalContent>(parcel.ReadParcelable<NotificationNormalContent>()));
            break;
        case NotificationContent::Type::CONVERSATION:
            content_ =
                std::static_pointer_cast<NotificationBasicContent>(std::shared_ptr<NotificationConversationalContent>(
                    parcel.ReadParcelable<NotificationConversationalContent>()));
            break;
        case NotificationContent::Type::LONG_TEXT:
            content_ = std::static_pointer_cast<NotificationBasicContent>(
                std::shared_ptr<NotificationLongTextContent>(parcel.ReadParcelable<NotificationLongTextContent>()));
            break;
        case NotificationContent::Type::MEDIA:
            content_ = std::static_pointer_cast<NotificationBasicContent>(
                std::shared_ptr<NotificationMediaContent>(parcel.ReadParcelable<NotificationMediaContent>()));
            break;
        case NotificationContent::Type::MULTILINE:
            content_ = std::static_pointer_cast<NotificationBasicContent>(
                std::shared_ptr<NotificationMultiLineContent>(parcel.ReadParcelable<NotificationMultiLineContent>()));
            break;
        case NotificationContent::Type::PICTURE:
            content_ = std::static_pointer_cast<NotificationBasicContent>(
                std::shared_ptr<NotificationPictureContent>(parcel.ReadParcelable<NotificationPictureContent>()));
            break;
        default:
            ANS_LOGE("Invalid contentType");
            return false;
    }
    if (!content_) {
        ANS_LOGE("Failed to read content");
        return false;
    }

    return true;
}
}  // namespace Notification
}  // namespace OHOS
