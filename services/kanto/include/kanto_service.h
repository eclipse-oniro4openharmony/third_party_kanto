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
#ifndef KANTO_SERVICE_H_
#define KANTO_SERVICE_H_

#include "kanto_service_stub.h"
#include "system_ability.h"

namespace OHOS {
class KantoService : public SystemAbility, public KantoServiceStub {
    DECLARE_SYSTEM_ABILITY(KantoService);

public:
    KantoService(int32_t saId, bool runOnCreate);
    ~KantoService() = default;

    void OnStart() override;
    void OnStop() override;
    int32_t StartServices() override;

protected:
    DISALLOW_COPY_AND_MOVE(KantoService);
};

}

#endif // KANTO_SERVICE_H_
