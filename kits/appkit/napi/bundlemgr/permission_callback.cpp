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
#include <uv.h>
#include "napi/native_common.h"
#include "permission_callback.h"
#include "hilog_wrapper.h"

namespace {

constexpr size_t ARGS_SIZE_TWO = 2;
}

PermissionCallback::PermissionCallback(napi_env env, napi_ref callback) : env_(env), callback_(callback)
{}

PermissionCallback::~PermissionCallback()
{
    if (callback_ != nullptr) {
        napi_delete_reference(env_, callback_);
    }
}

void PermissionCallback::OnChanged(const int32_t uid)
{
    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(env_, &loop);
#endif  // NAPI_VERSION >= 2

    uv_work_t *work = new uv_work_t;
    CallbackInfo *callbackInfo = new (std::nothrow) CallbackInfo{
        .env = env_,
        .callback = callback_,
        .uid = uid,
    };

    work->data = (void *)callbackInfo;

    uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            HILOG_INFO("CallOnPermissonChanged, uv_queue_work");
            // JS Thread
            CallbackInfo *event = (CallbackInfo *)work->data;
            napi_value result[2] = {0};

            napi_value callResult = nullptr;
            napi_value eCode = nullptr;
            NAPI_CALL_RETURN_VOID(event->env, napi_create_int32(event->env, 0, &eCode));
            napi_create_object(event->env, &callResult);
            napi_set_named_property(event->env, callResult, "code", eCode);
            result[0] = callResult;
            // create uid
            NAPI_CALL_RETURN_VOID(event->env, napi_create_int32(event->env, event->uid, &result[1]));

            napi_value callback = 0;
            napi_value undefined = 0;
            napi_get_undefined(event->env, &undefined);
            napi_value callbackResult = 0;
            napi_get_reference_value(event->env, event->callback, &callback);
            napi_call_function(event->env, undefined, callback, ARGS_SIZE_TWO, &result[0], &callbackResult);
            delete event;
            delete work;
        });

    HILOG_INFO("OnChanged, end");
}