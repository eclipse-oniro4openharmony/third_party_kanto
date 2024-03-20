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

#ifndef KANTO_HILOG_WRAPPER_H
#define KANTO_HILOG_WRAPPER_H

#include "hilog/log.h"

namespace OHOS {
// param of log interface, such as KANTO_HILOGF.
enum KantoSubModule {
    KANTO_MODULE_INNERKIT = 0,
    KANTO_MODULE_CLIENT,
    KANTO_MODULE_SERVICE,
    KANTO_MODULE_JAVAKIT, // java kit, defined to avoid repeated use of domain.
    KANTO_MODULE_JNI,
    KANTO_MODULE_COMMON,
    KANTO_MODULE_JS_NAPI,
    KANTO_MODULE_BUTT,
};

static constexpr unsigned int BASE_KANTO_DOMAIN_ID = 0xD001D04;

enum KantoDomainId {
    KANTO_INNERKIT_DOMAIN = BASE_KANTO_DOMAIN_ID + KANTO_MODULE_INNERKIT,
    KANTO_CLIENT_DOMAIN,
    KANTO_SERVICE_DOMAIN,
    KANTO_JAVAKIT_DOMAIN,
    KANTO_JNI_DOMAIN,
    KANTO_COMMON_DOMAIN,
    KANTO_JS_NAPI,
    KANTO_BUTT,
};

static constexpr OHOS::HiviewDFX::HiLogLabel KANTO_MODULE_LABEL[KANTO_MODULE_BUTT] = {
    { LOG_CORE, KANTO_INNERKIT_DOMAIN, "KantoInnerKit" },
    { LOG_CORE, KANTO_CLIENT_DOMAIN, "KantoClient" },
    { LOG_CORE, KANTO_SERVICE_DOMAIN, "KantoService" },
    { LOG_CORE, KANTO_JAVAKIT_DOMAIN, "KantoJavaKit" },
    { LOG_CORE, KANTO_JNI_DOMAIN, "KantoJni" },
    { LOG_CORE, KANTO_COMMON_DOMAIN, "KantoCommon" },
    { LOG_CORE, KANTO_JS_NAPI, "KantoJSNAPI" },
};

#define R_FILENAME_ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#define R_FORMATED(fmt, ...) "[%{public}s] %{public}s# " fmt, R_FILENAME_, __FUNCTION__, ##__VA_ARGS__

// In order to improve performance, do not check the module range.
// Besides, make sure module is less than KANTO_MODULE_BUTT.
#define KANTO_HILOGF(module, ...) \
    (void)OHOS::HiviewDFX::HiLog::Fatal(KANTO_MODULE_LABEL[module], R_FORMATED(__VA_ARGS__))
#define KANTO_HILOGE(module, ...) \
    (void)OHOS::HiviewDFX::HiLog::Error(KANTO_MODULE_LABEL[module], R_FORMATED(__VA_ARGS__))
#define KANTO_HILOGW(module, ...) (void)OHOS::HiviewDFX::HiLog::Warn(KANTO_MODULE_LABEL[module], R_FORMATED(__VA_ARGS__))
#define KANTO_HILOGI(module, ...) (void)OHOS::HiviewDFX::HiLog::Info(KANTO_MODULE_LABEL[module], R_FORMATED(__VA_ARGS__))
#define KANTO_HILOGD(module, ...) \
    (void)OHOS::HiviewDFX::HiLog::Debug(KANTO_MODULE_LABEL[module], R_FORMATED(__VA_ARGS__))

#define CHECK_AND_RETURN_RET_LOG(module, cond, ret, ...)  \
    do {                                                  \
        if (!(cond)) {                                    \
            KANTO_HILOGE(module, R_FORMATED(__VA_ARGS__)); \
            return ret;                                   \
        }                                                 \
    } while (0)

#define CHECK_AND_RETURN_LOG(module, cond, ...)           \
    do {                                                  \
        if (!(cond)) {                                    \
            KANTO_HILOGE(module, R_FORMATED(__VA_ARGS__)); \
            return;                                       \
        }                                                 \
    } while (0)

} // namespace OHOS
#endif // KANTO_HILOG_WRAPPER_H
