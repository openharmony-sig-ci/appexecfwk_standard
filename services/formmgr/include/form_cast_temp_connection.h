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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_FORMMGR_INCLUDE_CAST_TEMP_CONNECTION_H
#define FOUNDATION_APPEXECFWK_SERVICES_FORMMGR_INCLUDE_CAST_TEMP_CONNECTION_H

#include "event_handler.h"
#include "form_ability_connection.h"

namespace OHOS {
namespace AppExecFwk {
/**
 * @class FormCastTempConnection
 * Cast temp form Connection Stub.
 */
class FormCastTempConnection : public FormAbilityConnection {
public:
    FormCastTempConnection(const int64_t formId);
    virtual ~FormCastTempConnection() = default;

    /**
     * @brief OnAbilityConnectDone, AbilityMs notify caller ability the result of connect.
     * @param element service ability's ElementName.
     * @param remoteObject the session proxy of service ability.
     * @param resultCode ERR_OK on success, others on failure.
     */
    void OnAbilityConnectDone(
        const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode) override;

private:
    int64_t formId_;
    DISALLOW_COPY_AND_MOVE(FormCastTempConnection);
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif // FOUNDATION_APPEXECFWK_SERVICES_FORMMGR_INCLUDE_CAST_TEMP_CONNECTION_H