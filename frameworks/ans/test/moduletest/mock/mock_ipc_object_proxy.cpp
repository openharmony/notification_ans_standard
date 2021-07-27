/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "ipc_object_proxy.h"

#include "dbinder_error_code.h"
#include "ipc_types.h"
#include "ipc_debug.h"
#include "ipc_thread_skeleton.h"
#include "log_tags.h"
#include "securec.h"
#include "ans_manager_interface.h"
#include "ans_manager_stub.h"
#ifndef CONFIG_IPC_SINGLE
#endif

namespace OHOS {
#ifdef CONFIG_IPC_SINGLE
using namespace IPC_SINGLE;
#endif

static constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_ID_IPC, "IPCObjectProxy" };
IPCObjectProxy::IPCObjectProxy(int handle, std::u16string descriptor, int proto)
    : IRemoteObject(std::move(descriptor)), handle_(handle), proto_(proto), isFinishInit_(false), isRemoteDead_(false)
{}

IPCObjectProxy::~IPCObjectProxy()
{
    ZLOGW(LABEL, "handle = %{public}u destroyed", handle_);
}

int32_t IPCObjectProxy::GetObjectRefCount()
{
    MessageParcel dummy, reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    if (SendRequestInner(false, SYNCHRONIZE_REFERENCE, dummy, reply, option) == ERR_NONE) {
        return reply.ReadInt32();
    }
    return 0;
}

int IPCObjectProxy::Dump(int fd, const std::vector<std::u16string> &args)
{
    MessageParcel data, reply;
    MessageOption option { MessageOption::TF_SYNC };
    data.WriteFileDescriptor(fd);
    data.WriteString16Vector(args);
    return SendRequestInner(false, DUMP_TRANSACTION, data, reply, option);
}

int IPCObjectProxy::SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    return SendRequestInner(false, code, data, reply, option);
}

int IPCObjectProxy::SendLocalRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    return SendRequestInner(true, code, data, reply, option);
}

int IPCObjectProxy::SendRequestInner(bool isLocal, uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    if (IsObjectDead()) {
        return ERR_DEAD_OBJECT;
    }

    IRemoteInvoker *invoker = nullptr;
    if (isLocal) {
        invoker = IPCThreadSkeleton::GetDefaultInvoker();
    } else {
        invoker = IPCThreadSkeleton::GetRemoteInvoker(proto_);
    }
    if (invoker == nullptr) {
        ZLOGE(LABEL, "%s: null invoker, type = %d", __func__, proto_);
        return ERR_NULL_OBJECT;
    }

    int status = invoker->SendRequest(handle_, code, data, reply, option);
    if (status == ERR_DEAD_OBJECT) {
        MarkObjectDied();
    }
    Notification::AnsManagerStub ansManagerStub;
    ansManagerStub.OnRemoteRequest(code,data,reply,option);
    return status;
}

std::u16string IPCObjectProxy::GetInterfaceDescriptor()
{
    std::lock_guard<std::mutex> lockGuard(initMutex_);
    if (!remoteDescriptor_.empty()) {
        return remoteDescriptor_;
    }
    if (handle_ == 0) {
        ZLOGI(LABEL, "handle == 0, do nothing");
        return std::u16string();
    }

    MessageParcel data, reply;
    MessageOption option;

    uint32_t err = SendRequestInner(false, INTERFACE_TRANSACTION, data, reply, option);
    if (err != ERR_NONE) {
        ZLOGE(LABEL, "INTERFACE_TRANSACTION transact return error = %{public}u", err);
        return std::u16string();
    }
    remoteDescriptor_ = reply.ReadString16();

    return remoteDescriptor_;
}

std::string IPCObjectProxy::GetPidAndUidInfo()
{
    MessageParcel data, reply;
    MessageOption option;

    uint32_t err = SendRequestInner(false, GET_UIDPID_INFO, data, reply, option);
    if (err != ERR_NONE) {
        ZLOGE(LABEL, "GetPidAndUidInfo SendRequestInner return error = %{public}u", err);
        return std::string("");
    }
    return reply.ReadString();
}

std::string IPCObjectProxy::GetDataBusName()
{
    MessageParcel data, reply;
    MessageOption option;

    uint32_t err = SendRequestInner(false, GRANT_DATABUS_NAME, data, reply, option);
    if (err != ERR_NONE) {
        ZLOGE(LABEL, "GetDataBusName transact return error = %{public}u", err);
        return std::string("");
    }

    if (reply.ReadUint32() != IRemoteObject::IF_PROT_DATABUS) {
        ZLOGE(LABEL, "GetDataBusName normal binder");
        return std::string("");
    }

    return reply.ReadString();
}

void IPCObjectProxy::OnFirstStrongRef(const void *objectId)
{
    return WaitForInit();
}

void IPCObjectProxy::WaitForInit()
{
#ifndef CONFIG_IPC_SINGLE
    int type = 0;
#endif

    {
        bool acquire = true;
        std::lock_guard<std::mutex> lockGuard(initMutex_);
        if (IsObjectDead()) {
            ZLOGI(LABEL, "check a dead proxy, init again");
            isRemoteDead_ = false;
            isFinishInit_ = false;
            acquire = false;
        }

        // check again is this object been initialized
        if (isFinishInit_) {
            return;
        }
        IRemoteInvoker *invoker = IPCThreadSkeleton::GetDefaultInvoker();
        if (invoker != nullptr && acquire == true) {
            invoker->AcquireHandle(handle_);
        }
#ifndef CONFIG_IPC_SINGLE
        type = UpdateProto();
#endif
        isFinishInit_ = true;
    }
#ifndef CONFIG_IPC_SINGLE
    if (type == IRemoteObject::IF_PROT_DATABUS) {
        IncRefToRemote();
    }
#endif
}

void IPCObjectProxy::OnLastStrongRef(const void *objectId)
{
}


/* mutex_ should be called before set or get isRemoteDead_ status */
void IPCObjectProxy::MarkObjectDied()
{
    isRemoteDead_ = true;
}

bool IPCObjectProxy::IsObjectDead() const
{
    return isRemoteDead_;
}

bool IPCObjectProxy::AddDeathRecipient(const sptr<DeathRecipient> &recipient)
{
    return true;
}

bool IPCObjectProxy::RemoveDeathRecipient(const sptr<DeathRecipient> &recipient)
{
    return true;
}

void IPCObjectProxy::SendObituary()
{
}

int IPCObjectProxy::GetProto() const
{
    return proto_;
}

int32_t IPCObjectProxy::NoticeServiceDie()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    data.WriteInt32(IRemoteObject::DeathRecipient::NOTICE_DEATH_RECIPIENT);

    int status = SendLocalRequest(DBINDER_OBITUARY_TRANSACTION, data, reply, option);
    if (status != ERR_NONE || reply.ReadInt32() != ERR_NONE) {
        ZLOGE(LABEL, "%s: send local request fail, status = %d", __func__, status);
        return IPC_PROXY_TRANSACTION_ERR;
    }

    return ERR_NONE;
}

int IPCObjectProxy::InvokeListenThread(MessageParcel &data, MessageParcel &reply)
{
    MessageOption option;
    return SendRequestInner(false, INVOKE_LISTEN_THREAD, data, reply, option);
}

#ifndef CONFIG_IPC_SINGLE
int IPCObjectProxy::UpdateProto()
{
    int type = GetSessionFromDBinderService();
    SetProto(type);
    return type;
}

void IPCObjectProxy::IncRefToRemote()
{
    MessageParcel data, reply;
    MessageOption option;

    int32_t err = SendRequestInner(false, DBINDER_INCREFS_TRANSACTION, data, reply, option);
    if (err != ERR_NONE) {
        ZLOGE(LABEL, "DBINDER_INCREFS_TRANSACTION transact return error = %{public}d", err);
        // do nothing
    }
}


void IPCObjectProxy::ReleaseProto()
{
    switch (GetProto()) {
        case IRemoteObject::IF_PROT_BINDER: {
            ZLOGW(LABEL, "it is normal binder, try to delete handle to index");
            ReleaseBinderProto();
            break;
        }
        case IRemoteObject::IF_PROT_DATABUS: {
            ReleaseDatabusProto();
            break;
        }
        default: {
            ZLOGE(LABEL, "ReleaseProto Invalid Type");
            break;
        }
    }

    return;
}

void IPCObjectProxy::SetProto(int proto)
{
    proto_ = proto;
}

int IPCObjectProxy::GetSessionFromDBinderService()
{
    MessageParcel data, reply;
    MessageOption option;
    uint32_t type = IRemoteObject::IF_PROT_BINDER;

    if (CheckHaveSession(type)) {
        ZLOGE(LABEL, "GetSessionFromDBinderService type = %u", type);
        return type;
    }

    uint32_t err = SendRequestInner(true, GET_PROTO_INFO, data, reply, option);
    if (err != ERR_NONE) {
        ZLOGI(LABEL, "GET_PROTO_INFO transact return error = %{public}u", err);
        return IRemoteObject::IF_PROT_BINDER;
    }

    switch (reply.ReadUint32()) {
        case IRemoteObject::IF_PROT_BINDER: {
            ZLOGW(LABEL, "it is normal binder, not dbinder");
            break;
        }
        case IRemoteObject::IF_PROT_DATABUS: {
            if (UpdateDatabusClientSession(handle_, reply)) {
                ZLOGW(LABEL, "it is dbinder, not binder");
                return IRemoteObject::IF_PROT_DATABUS;
            }
            break;
        }
        default: {
            ZLOGE(LABEL, "GetSessionFromDBinderService Invalid Type");
            break;
        }
    }

    return IRemoteObject::IF_PROT_BINDER;
}

bool IPCObjectProxy::AddDbinderDeathRecipient()
{
    return true;
}

bool IPCObjectProxy::RemoveDbinderDeathRecipient()
{
    return true;
}

bool IPCObjectProxy::CheckHaveSession(uint32_t &type)
{
    return true;
}

bool IPCObjectProxy::UpdateDatabusClientSession(int handle, MessageParcel &reply)
{
    return true;
}

void IPCObjectProxy::ReleaseDatabusProto()
{
    return;
}

void IPCObjectProxy::ReleaseBinderProto()
{
}
#endif
} // namespace OHOS
