
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

#include "appexecfwk_errors.h"
#include "app_log_wrapper.h"
#include "form_acquire_connection.h"
#include "form_constants.h"
#include "form_supply_callback.h"
#include "form_task_mgr.h"
#include "form_util.h"
#include "ipc_types.h"
#include "message_parcel.h"
#include "want.h"

namespace OHOS {
namespace AppExecFwk {
FormAcquireConnection::FormAcquireConnection(const int64_t formId, const FormItemInfo &info, 
const WantParams &wantParams)
    :formId_(formId),
    info_(info), 
    wantParams_(wantParams)
{
}
/**
 * @brief OnAbilityConnectDone, AbilityMs notify caller ability the result of connect.
 * @param element service ability's ElementName.
 * @param remoteObject the session proxy of service ability.
 * @param resultCode ERR_OK on success, others on failure.
 */
void FormAcquireConnection::OnAbilityConnectDone(
    const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    APP_LOGI("%{public}s called.", __func__);
    if (resultCode != ERR_OK) {
        //APP_LOGE("%{public}s, abilityName:%{public}s, formId:%{public}lld, resultCode:%{public}d", 
        //   __func__, element.GetAbilityName().c_str(), formId_, resultCode);
        return;
    }
    FormSupplyCallback::GetInstance()->AddConnection(this);
    Want want;
    want.SetParams(wantParams_);
    FormUtil::CreateFormWant(info_.GetFormName(), info_.GetSpecificationId(), info_.IsTemporaryForm(), want);
    if (want.GetBoolParam(Constants::RECREATE_FORM_KEY, false)) {
        want.SetParam(Constants::ACQUIRE_TYPE, Constants::ACQUIRE_TYPE_RECREATE_FORM);
    } else {
        want.SetParam(Constants::ACQUIRE_TYPE, Constants::ACQUIRE_TYPE_CREATE_FORM);
    }
    want.SetParam(Constants::FORM_CONNECT_ID, this->GetConnectId());
    FormTaskMgr::GetInstance().PostAcquireTask(formId_, want, remoteObject);
}
}  // namespace AppExecFwk
}  // namespace OHOS