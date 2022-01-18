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

#include "blob.h"

#include <securec.h>

namespace OHOS {
namespace DistributedKv {
Blob::Blob()
{}

Blob::Blob(const Blob &blob)
{
    blob_ = blob.Data();
}

Blob::Blob(Blob &&blob)
{
    blob_.swap(blob.blob_);
}

Blob &Blob::operator=(const Blob &blob)
{
    // Self-assignment detection
    if (&blob == this) {
        return *this;
    }

    blob_ = blob.Data();

    return *this;
}

Blob &Blob::operator=(Blob &&blob)
{
    // Self-assignment detection
    if (&blob == this) {
        return *this;
    }

    blob_.swap(blob.blob_);

    return *this;
}

Blob::Blob(const char *str, size_t n) : blob_()
{
    if (str != nullptr) {
        blob_ = std::vector<uint8_t>(str, str + n);
    }
}

Blob::Blob(const std::string &str) : blob_(str.begin(), str.end())
{}

Blob::Blob(const char *str) : blob_()
{
    if (str != nullptr) {
        blob_ = std::vector<uint8_t>(str, str + strlen(str));
    }
}

Blob::Blob(const std::vector<uint8_t> &bytes) : blob_(bytes)
{}

Blob::Blob(std::vector<uint8_t> &&bytes) : blob_(std::move(bytes))
{}

const std::vector<uint8_t> &Blob::Data() const
{
    return blob_;
}

std::string Blob::ToString() const
{
    std::string str(blob_.begin(), blob_.end());
    return str;
}

bool Blob::Marshalling(Parcel &parcel) const
{
    return parcel.WriteUInt8Vector(this->blob_);
}

Blob *Blob::Unmarshalling(Parcel &parcel)
{
    std::vector<uint8_t> blobData;
    if (!parcel.ReadUInt8Vector(&blobData)) {
        return nullptr;
    }
    return new Blob(blobData);
}
}  // namespace DistributedKv
}  // namespace OHOS