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

#include "app_log_wrapper.h"
#include "app_scheduler_interface.h"
#include "errors.h"
#include "form_host_stub.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "iremote_object.h"

namespace OHOS {
namespace AppExecFwk {
FormHostStub::FormHostStub()
{
    memberFuncMap_[static_cast<uint32_t>(IFormHost::Message::FORM_HOST_ON_ACQUIRED)] =
        &FormHostStub::HandleAcquired;
    memberFuncMap_[static_cast<uint32_t>(IFormHost::Message::FORM_HOST_ON_UPDATE)] =
        &FormHostStub::HandleOnUpdate;
    memberFuncMap_[static_cast<uint32_t>(IFormHost::Message::FORM_HOST_ON_UNINSTALL)] =
        &FormHostStub::HandleOnUninstall; 
}

FormHostStub::~FormHostStub()
{
    memberFuncMap_.clear();
}
/**
 * @brief handle remote request.
 * @param data input param.
 * @param reply output param.
 * @param option message option.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormHostStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    APP_LOGI("FormHostStub::OnReceived, code = %{public}d, flags= %{public}d.", code, option.GetFlags());
    std::u16string descriptor = FormHostStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        APP_LOGE("%{public}s failed, local descriptor is not equal to remote", __func__);
        return ERR_INVALID_STATE;
    }

    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data, reply);
        }
    }

    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}
/**
 * @brief handle OnAcquired event.
 * @param data input param.
 * @param reply output param.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormHostStub::HandleAcquired(MessageParcel &data, MessageParcel &reply)
{
    std::unique_ptr<FormJsInfo> formInfo(data.ReadParcelable<FormJsInfo>());
    if (!formInfo) {
        APP_LOGE("%{public}s, failed to ReadParcelable<FormJsInfo>", __func__);
        return ERR_NULL_OBJECT;
    }
    OnAcquired(*formInfo);
    reply.WriteInt32(ERR_OK);
    return ERR_OK;
}
/**
 * @brief handle OnUpdate event.
 * @param data input param.
 * @param reply output param.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormHostStub::HandleOnUpdate(MessageParcel &data, MessageParcel &reply)
{
    std::unique_ptr<FormJsInfo> formInfo(data.ReadParcelable<FormJsInfo>());
    if (!formInfo) {
        APP_LOGE("%{public}s, failed to ReadParcelable<FormJsInfo>", __func__);
        return ERR_NULL_OBJECT;
    }
    OnUpdate(*formInfo);
    reply.WriteInt32(ERR_OK);
    return ERR_OK;
}

/**
 * @brief handle OnUnInstall event.
 * @param data input param.
 * @param reply output param.
 * @return Returns ERR_OK on success, others on failure.
 */
int FormHostStub::HandleOnUninstall(MessageParcel &data, MessageParcel &reply)
{
    std::vector<int64_t> formIds;
    bool ret = data.ReadInt64Vector(&formIds);
    if (ret) {
        OnUninstall(formIds);
        reply.WriteInt32(ERR_OK);
        return ERR_OK;
    }
    return ERR_INVALID_DATA;
}
}  // namespace AppExecFwk
}  // namespace OHOS