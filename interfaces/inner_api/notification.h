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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_H

#include "notification_request.h"
#include "parcel.h"
#include "uri.h"

namespace OHOS {
namespace Notification {
class Notification final : public Parcelable {
public:
    /**
     * @brief A constructor used to create a Notification instance by existing NotificationRequest object.
     *
     * @param request Indicates the existing NotificationRequest object.
     */
    Notification(const sptr<NotificationRequest> &request);

    /**
     * @brief A constructor used to create a Notification instance by existing NotificationRequest object.
     *
     * @param deviceId Indicates the device id.
     * @param request Indicates the existing NotificationRequest object.
     */
    Notification(const std::string &deviceId, const sptr<NotificationRequest> &request);

    /**
     * @brief A constructor used to create a Notification instance by copying parameters from an existing one.
     *
     * @param other Indicates the Notification object.
     */
    Notification(const Notification &other);

    ~Notification();

    /**
     * @brief Obtains whether to enable the notification light when a notification is received on the device, provided
     * that this device has a notification light.
     *
     * @return Returns true if led light color is set.
     */
    bool EnableLight() const;

    /**
     * @brief Obtains the sound enabled or not, set by ANS.
     *
     * @return Returns true if sound is set.
     */
    bool EnableSound() const;

    /**
     * @brief Obtains the vibrate enabled or not, set by ANS.
     *
     * @return Returns true if vibrate style is set.
     */
    bool EnableVibrate() const;

    /**
     * @brief Obtains the bundle's name which publish this notification.
     *
     * @return Returns the bundle's name.
     */
    std::string GetBundleName() const;

    /**
     * @brief Obtains the bundle's name which create this notification.
     *
     * @return Returns the creator bundle name.
     */
    std::string GetCreateBundle() const;

    /**
     * @brief Obtains the label of this notification.
     *
     * @return Returns the label.
     */
    std::string GetLabel() const;

    /**
     * @brief Obtains the color of the notification light in a NotificationSlot object
     *
     * @return Returns the color of the notification light.
     */
    int32_t GetLedLightColor() const;

    /**
     * @brief Sets the notification display effect, including whether to display this notification on the lock screen,
     * and how it will be presented if displayed.
     *
     * @return Returns the display effect of this notification on the lock screen.
     */
    NotificationConstant::VisiblenessType GetLockscreenVisibleness() const;

    /**
     * @brief The ID passed to setGroup(), or the override, or null.
     *
     * @return Returns the string of group.
     */
    std::string GetGroup() const;

    /**
     * @brief Obtains the id of the notification.
     *
     * @return Returns the id supplied to NotificationManager::Notify(int, NotificationRequest).
     */
    int32_t GetId() const;

    /**
     * @brief A key for this notification record.
     *
     * @return Returns a unique instance key.
     */
    std::string GetKey() const;

    /**
     * @brief Obtains the notification request set by ANS.
     *
     * @return Returns NotificationRequest object.
     */
    NotificationRequest GetNotificationRequest() const;

    /**
     * @brief Obtains the time notification was posted.
     *
     * @return Returns the time notificationRequest was posted.
     */
    int64_t GetPostTime() const;

    /**
     * @brief Obtains the sound uri.
     *
     * @return Returns the sound set by ANS.
     */
    Uri GetSound() const;

    /**
     * @brief Obtains the UID of the notification creator.
     *
     * @return Returns the UID of the notification creator.
     */
    int32_t GetUid() const;

    /**
     * @brief Obtains the PID of the notification creator.
     *
     * @return Returns the PID of the notification creator.
     */
    pid_t GetPid() const;

    /**
     * @brief Checks whether this notification is unremovable.
     * @return Returns true if this notification is unremovable; returns false otherwise.
     */
    bool IsUnremovable() const;

    /**
     * @brief Obtains the vibration style for this notifications.
     *
     * @return Returns the vibration style.
     */
    std::vector<int64_t> GetVibrationStyle() const;

    /**
     * @brief This notification is part of a group or not.
     *
     * @return Returns true if this notification is part of a group.
     */
    bool IsGroup() const;

    /**
     * @brief Checks whether this notification is displayed as a floating icon on top of the screen.
     *
     * @return Returns true if this notification is displayed as a floating icon; returns false otherwise.
     */
    bool IsFloatingIcon() const;

    /**
     * @brief Obtains the remind type of a notification.
     * @return Returns the remind type of a notification.
     */
    NotificationConstant::RemindType GetRemindType() const;

    /**
     * @brief Whether to support remove allowed.
     * @return Returns the current remove allowed status.
     */
    bool IsRemoveAllowed() const;

    /**
     * @brief Gets the notification source.
     * @return Returns the notification slot type.
     */
    NotificationConstant::SourceType GetSourceType() const;

    /**
     * @brief Gets the device id of the notification source.
     *
     * @return Returns the device id.
     */
    std::string GetDeviceId() const;

    /**
     * @brief Obtains the UserId of the notification creator.
     *
     * @return Returns the UserId of the notification creator.
     */
    int32_t GetUserId() const;

    /**
     * @brief Dumps a string representation of the object.
     *
     * @return Returns a string representation of the object.
     */
    std::string Dump() const;

    /**
     * @brief Marshals a Notification object into a Parcel.
     *
     * @param parcel Indicates the Parcel object for marshalling.
     * @return Returns true if the marshalling is successful; returns false otherwise.
     */
    bool Marshalling(Parcel &parcel) const;

    /**
     * @brief Unmarshals a Notification object from a Parcel.
     *
     * @param Indicates the Parcel object for unmarshalling.
     * @return Returns true if the unmarshalling is successful; returns false otherwise.
     */
    static Notification *Unmarshalling(Parcel &parcel);

private:
    Notification();
    void SetEnableSound(const bool &enable);
    void SetEnableLight(const bool &enable);
    void SetEnableVibration(const bool &enable);
    void SetLedLightColor(const int32_t &color);
    void SetLockScreenVisbleness(const NotificationConstant::VisiblenessType &visbleness);
    void SetPostTime(const int64_t &time);
    void SetSound(const Uri &sound);
    void SetVibrationStyle(const std::vector<int64_t> &style);
    void SetRemindType(const NotificationConstant::RemindType &reminType);
    std::string GenerateNotificationKey(
        const std::string &deviceId, int32_t userId, int32_t uid, const std::string &label, int32_t id);
    void SetRemoveAllowed(bool removeAllowed);
    void SetSourceType(NotificationConstant::SourceType sourceType);
    bool ReadFromParcel(Parcel &parcel);
    void ReadFromParcelBool(Parcel &parcel);
    void ReadFromParcelString(Parcel &parcel);
    void ReadFromParcelInt32(Parcel &parcel);
    void ReadFromParcelInt64(Parcel &parcel);
    void ReadFromParcelParcelable(Parcel &parcel);
    bool MarshallingBool(Parcel &parcel) const;
    bool MarshallingString(Parcel &parcel) const;
    bool MarshallingInt32(Parcel &parcel) const;
    bool MarshallingInt64(Parcel &parcel) const;
    bool MarshallingParcelable(Parcel &parcel) const;

private:
    bool enableSound_ {false};
    bool enableLight_ {false};
    bool enableVibration_ {false};
    bool isRemoveAllowed_ {true};
    std::string key_ {""};
    std::string deviceId_ {""};
    int32_t ledLightColor_ {0};
    NotificationConstant::VisiblenessType lockscreenVisibleness_ {NotificationConstant::VisiblenessType::NO_OVERRIDE};
    NotificationConstant::RemindType remindType_ {NotificationConstant::RemindType::NONE};
    NotificationConstant::SourceType sourceType_ {NotificationConstant::SourceType::TYPE_NORMAL};
    sptr<NotificationRequest> request_ {nullptr};
    int64_t postTime_ {0};
    std::shared_ptr<Uri> sound_ {nullptr};
    std::vector<int64_t> vibrationStyle_ {};

    friend class AdvancedNotificationService;
    friend class NotificationSlotFilter;
    friend class DisturbFilter;
};
}  // namespace Notification
}  // namespace OHOS
#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_H