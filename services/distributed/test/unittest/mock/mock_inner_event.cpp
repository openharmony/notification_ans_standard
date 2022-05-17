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

#include "inner_event.h"

namespace OHOS {
namespace AppExecFwk {
InnerEvent::Pointer InnerEvent::Get(const Callback &callback, const std::string &name)
{
    InnerEvent::Pointer pointer = InnerEvent::Pointer(new InnerEvent, [](InnerEvent *event) {});
    pointer->taskCallback_ = callback;
    pointer->taskName_ = name;
    return pointer;
}
}  // namespace AppExecFwk
}  // namespace OHOS