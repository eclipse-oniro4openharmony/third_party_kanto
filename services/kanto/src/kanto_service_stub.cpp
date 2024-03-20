/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "kanto_service_stub.h"

#include "kanto_service.h"
#include "kanto_service_ipc_interface_code.h"
#include "kanto_hilog.h"

namespace OHOS {

KantoServiceStub::KantoServiceStub()
{
    InitMemberFuncMap();
}

void KantoServiceStub::InitMemberFuncMap()
{
    memberFuncMap_[KantoServiceIpcInterfaceCode::KANTO_SERVICE_START_SERVICES] = &KantoServiceStub::StartServicesInner;
}

int32_t KantoServiceStub::OnRemoteRequest(uint32_t code,
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    KANTO_HILOGI(KANTO_MODULE_SERVICE, "KantoServiceStub::OnReceived, code = %u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        KANTO_HILOGE(KANTO_MODULE_SERVICE, "KANTO_SERVICE_NOT_INIT ReadInterfaceToken failed!");
        return -1;
    }

    auto itFunc = memberFuncMap_.find(static_cast<KantoServiceIpcInterfaceCode>(code));
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data, reply);
        }
    }
    KANTO_HILOGI(KANTO_MODULE_SERVICE, "KantoServiceStub:: default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t KantoServiceStub::StartServicesInner(MessageParcel &data, MessageParcel &reply)
{
    KANTO_HILOGI(KANTO_MODULE_SERVICE, "StartServicesInner called");
    int32_t retReply = StartServices(); 
    if (!reply.WriteInt32(retReply)) {
        KANTO_HILOGE(KANTO_MODULE_SERVICE, "StartServicesInner write reply failed!");
        return -1;
    }
    return 0;
}

} // namespace OHOS
