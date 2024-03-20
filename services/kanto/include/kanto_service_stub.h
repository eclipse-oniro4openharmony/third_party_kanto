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
#ifndef KANTO_SERVICE_STUB_H_
#define KANTO_SERVICE_STUB_H_

#include <map>
#include "if_kanto_service.h"
#include "iremote_stub.h"
#include "kanto_service_ipc_interface_code.h"

namespace OHOS {
class KantoServiceStub : public IRemoteStub<IKantoService> {
public:
    KantoServiceStub();
    virtual ~KantoServiceStub() = default;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    int32_t StartServicesInner(MessageParcel &data, MessageParcel &reply);
    void InitMemberFuncMap();

    using KantoServiceStubFunc =
            int32_t (KantoServiceStub::*)(MessageParcel &data, MessageParcel &reply);
            
    std::map<KantoServiceIpcInterfaceCode, KantoServiceStubFunc > memberFuncMap_;
};

} //namespace OHOS

#endif // KANTO_SERVICE_STUB_H_
