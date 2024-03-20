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
#include "kanto_service.h"
#include "kanto_hilog.h"
#include "system_ability_definition.h"
#include <unistd.h>
#include <system_error>

namespace OHOS {
REGISTER_SYSTEM_ABILITY_BY_ID(KantoService, KANTO_SERVICE_SA_ID, true);

KantoService::KantoService(int32_t saId, bool runOnCreate) : SystemAbility(saId, runOnCreate)
{
}

void KantoService::OnStart()
{
    SystemAbility::OnStart(); // Initialize the system ability
    StartServices(); // Start the required services
}

void KantoService::OnStop()
{
    SystemAbility::OnStop(); // Cleanup on stop
}

int32_t KantoService::StartServices()
{
    KANTO_HILOGI(KANTO_MODULE_SERVICE, "starting kanto!");
    return 0;
}

} // namespace OHOS
