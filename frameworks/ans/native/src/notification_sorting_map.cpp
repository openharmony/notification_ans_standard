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

#include "notification_sorting_map.h"

#include "parcel.h"

namespace OHOS {
namespace Notification {
NotificationSortingMap::NotificationSortingMap()
{}

NotificationSortingMap::~NotificationSortingMap()
{}

NotificationSortingMap::NotificationSortingMap(const std::vector<NotificationSorting> &sortingList)
{
    for (auto item : sortingList) {
        sortedKey_.push_back(item.GetKey());
        sortings_[item.GetKey()] = item;
    }
}

void NotificationSortingMap::SetKey(const std::string &key)
{
    auto it = std::find(sortedKey_.cbegin(), sortedKey_.cend(), key);
    if (it == sortedKey_.cend()) {
        sortedKey_.emplace_back(key);
    }
}

bool NotificationSortingMap::GetNotificationSorting(const std::string &key, NotificationSorting &sorting) const
{
    for (auto item : sortings_) {
        if (item.first == key) {
            sorting = item.second;
            return true;
        }
    }

    return false;
}

void NotificationSortingMap::SetNotificationSorting(const std::vector<NotificationSorting> &sortingList)
{
    for (auto item : sortingList) {
        sortedKey_.push_back(item.GetKey());
        sortings_[item.GetKey()] = item;
    }
}

bool NotificationSortingMap::Marshalling(Parcel &parcel) const
{
    bool ret = true;
    if (!parcel.WriteUint64(sortings_.size())) {
        ANS_LOGE("Can't write sorting size");
        return false;
    }

    size_t count = 0;
    for (auto &sorting : sortings_) {
        if (!parcel.WriteParcelable(&sorting.second)) {
            ANS_LOGE("Can't write sorting");
            ret = false;
        }
        count++;

        if (count == MAX_ACTIVE_NUM) {
            break;
        }
    }

    return ret;
}

NotificationSortingMap *NotificationSortingMap::Unmarshalling(Parcel &parcel)
{
    std::vector<NotificationSorting> sortings;
    // read sorting num
    uint64_t size = 0;
    parcel.ReadUint64(size);
    size = (size <= MAX_ACTIVE_NUM) ? size : MAX_ACTIVE_NUM;

    for (uint64_t i = 0; i < size; i++) {
        // read sorting
        NotificationSorting *sorting = parcel.ReadParcelable<NotificationSorting>();
        if (sorting != nullptr) {
            sortings.push_back(*sorting);
        }
    }

    NotificationSortingMap *sortingMap = new (std::nothrow) NotificationSortingMap(sortings);
    return sortingMap;
}

std::string NotificationSortingMap::Dump() const
{
    std::string keys = "";
    for (auto key : sortedKey_) {
        keys += key;
        keys += ", ";
    }
    return "NotificationSortingMap{ "
            "sortedkey = [" + keys + "]"
            " }";
}
}  // namespace Notification
}  // namespace OHOS
