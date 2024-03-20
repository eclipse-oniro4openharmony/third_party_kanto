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
#ifndef INTERFACES_KANTO_SERVICE_H_
#define INTERFACES_KANTO_SERVICE_H_

#include "iremote_broker.h"
#include "iremote_object.h"
#include "iremote_proxy.h"

namespace OHOS {
class IKantoService : public IRemoteBroker {
public:
    virtual ~IKantoService() = default;

    virtual int32_t StartServices() = 0;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IKantoService");
};
} // namespace OHOS

#endif // !defined(INTERFACES_KANTO_SERVICE_H_ )
