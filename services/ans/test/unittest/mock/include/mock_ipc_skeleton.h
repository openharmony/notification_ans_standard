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

#ifndef OHOS_IPC_IPC_SKELETON_H
#define OHOS_IPC_IPC_SKELETON_H

#include "accesstoken_kit.h"
#include "iremote_object.h"

namespace OHOS {
class IPCSkeleton {
public:
    IPCSkeleton() = default;
    ~IPCSkeleton() = default;

    /**
     * @brief Set the max number of work thread.
     *          Default max is 4, only if you need a customize value.
     *
     * @param maxThreadNum Indicates the max number of work thread
     * @return Indicates the result.
     */
    static bool SetMaxWorkThreadNum(int maxThreadNum);

    /**
     * @brief Join current thread into work loop.
     */
    static void JoinWorkThread();

    /**
     * @brief Remove current thread from work loop.
     */
    static void StopWorkThread();

    /**
     * @brief Get calling pid.
     *
     * @return Indicates the calling pid.
     */
    static pid_t GetCallingPid();

    /**
     * @brief Get calling uid.
     *
     * @return Indicates the calling uid.
     */
    static pid_t GetCallingUid();

    /**
     * @brief Get the ID of local device.
     *
     * @return Indicates the ID of local device.
     */
    static std::string GetLocalDeviceID();

    static Security::AccessToken::AccessTokenID GetCallingTokenID();

    /**
     * @brief Get the ID of calling device.
     *
     * @return Indicates the ID of calling device.
     */
    static std::string GetCallingDeviceID();

    /**
     * @brief Whether it is locally calling.
     *
     * @return Return ture for calling from local, otherwise false.
     */
    static bool IsLocalCalling();

    /**
     * @brief Get the instance.
     *
     * @return Indicates the instance.
     */
    static IPCSkeleton &GetInstance();

    /**
     * @brief Get the object of context.
     *
     * @return Indicates the context cobject.
     */
    static sptr<IRemoteObject> GetContextObject();

    /**
     * @brief Set the object of context.
     *
     * @param object Indicates the context cobject.
     * @return Indicates the result.
     */
    static bool SetContextObject(sptr<IRemoteObject> &object);

    /**
     * @brief Flush the commands.
     *
     * @param object Indicates the object.
     * @return Indicates the result.
     */
    static int FlushCommands(IRemoteObject *object);

    /**
     * @brief Reset calling identity.
     *
     * @return Indicates the result.
     */
    static std::string ResetCallingIdentity();

    /**
     * @brief Set calling identity.
     *
     * @param identity Indicates the identity.
     * @return Indicates the result.
     */
    static bool SetCallingIdentity(std::string &identity);

    /**
     * @brief Set calling uid.
     *
     * @param uid Indicates the uid.
     */
    static void SetCallingUid(pid_t uid);

    static void SetCallingTokenID(Security::AccessToken::AccessTokenID callerToken);
};
}  // namespace OHOS
#endif  // OHOS_IPC_IPC_SKELETON_H
