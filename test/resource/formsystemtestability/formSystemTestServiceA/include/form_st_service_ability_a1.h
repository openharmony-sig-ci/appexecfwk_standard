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

#ifndef FORM_ST_SERVICE_ABILITY_A1_
#define FORM_ST_SERVICE_ABILITY_A1_
#include <string>
#include <map>
#include "ability_connect_callback_stub.h"
#include "ability_connect_callback_proxy.h"
#include "ability_loader.h"
#include "common_event.h"
#include "common_event_manager.h"

#include "form_provider_info.h"

namespace OHOS {
namespace AppExecFwk {
using AbilityConnectionStub = OHOS::AAFwk::AbilityConnectionStub;
using AbilityConnectionProxy = OHOS::AAFwk::AbilityConnectionProxy;
const std::string APP_A1_RESP_EVENT_NAME = "resp_com_ohos_formst_service_app_a1";
const std::string APP_A1_REQ_EVENT_NAME = "req_com_ohos_formst_service_app_a1";

class FormStServiceAbilityA1 : public Ability {
public:
    ~FormStServiceAbilityA1();

protected:
    virtual void OnStart(const Want &want) override;
    virtual void OnStop() override;
    virtual void OnActive() override;
    virtual void OnInactive() override;
    virtual void OnBackground() override;
    virtual void OnNewWant(const Want &want) override;
    virtual void OnCommand(const AAFwk::Want &want, bool restart, int startId) override;
    virtual sptr<IRemoteObject> OnConnect(const Want &want) override;
    virtual void OnDisconnect(const Want &want) override;
    virtual FormProviderInfo OnCreateForm(const Want &want) override;

private:
    void Clear();
    void GetWantInfo(const Want &want);
    std::vector<std::string> Split(std::string str, const std::string &token);
    bool SubscribeEvent();
    static bool PublishEvent(const std::string &eventName, const int &code, const std::string &data);
    void StartOtherAbility();
    void ConnectOtherAbility();
    void DisConnectOtherAbility();
    void StopSelfAbility();

    std::string shouldReturn_ = {};
    std::string targetBundle_ = {};
    std::string targetAbility_ = {};
    std::string nextTargetBundle_ = {};
    std::string nextTargetAbility_ = {};
    std::string targetBundleConn_ = {};
    std::string targetAbilityConn_ = {};
    std::string nextTargetBundleConn_ = {};
    std::string nextTargetAbilityConn_ = {};
    std::string zombie_ = {};

    typedef void (FormStServiceAbilityA1::*func)();
    static std::map<std::string, func> funcMap_;
    class AbilityConnectCallback;
    sptr<AbilityConnectCallback> stub_ = {};
    sptr<AbilityConnectionProxy> connCallback_ = {};
    class AppEventSubscriber;
    std::shared_ptr<AppEventSubscriber> subscriber_ = {};

    class AbilityConnectCallback : public AbilityConnectionStub {
    public:
        sptr<IRemoteObject> AsObject() override
        {
            return nullptr;
        }
        /**
         * OnAbilityConnectDone, AbilityMs notify caller ability the result of connect.
         *
         * @param element,.service ability's ElementName.
         * @param remoteObject,.the session proxy of service ability.
         * @param resultCode, ERR_OK on success, others on failure.
         */
        void OnAbilityConnectDone(
            const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode) override
        {
            if (resultCode == 0) {
                onAbilityConnectDoneCount++;
                PublishEvent(APP_A1_RESP_EVENT_NAME, onAbilityConnectDoneCount, "OnAbilityConnectDone");
            }
        }

        /**
         * OnAbilityDisconnectDone, AbilityMs notify caller ability the result of disconnect.
         *
         * @param element,.service ability's ElementName.
         * @param resultCode, ERR_OK on success, others on failure.
         */
        void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) override
        {
            if (resultCode == 0) {
                onAbilityConnectDoneCount--;
                PublishEvent(APP_A1_RESP_EVENT_NAME, onAbilityConnectDoneCount, "OnAbilityDisconnectDone");
            }
        }

        static int onAbilityConnectDoneCount;
    };
    class AppEventSubscriber : public EventFwk::CommonEventSubscriber {
    public:
        AppEventSubscriber(const EventFwk::CommonEventSubscribeInfo &sp) : CommonEventSubscriber(sp){};
        ~AppEventSubscriber() = default;
        virtual void OnReceiveEvent(const EventFwk::CommonEventData &data) override;

        FormStServiceAbilityA1 *mainAbility_ = nullptr;
    };
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FORM_ST_SERVICE_ABILITY_A1_
