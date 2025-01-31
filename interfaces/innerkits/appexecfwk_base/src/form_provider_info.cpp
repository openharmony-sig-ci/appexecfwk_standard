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

#include "form_provider_info.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
bool FormProviderInfo::ReadFromParcel(Parcel &parcel)
{
    formId_ = parcel.ReadInt64();
    auto bindingData = parcel.ReadParcelable<FormProviderData>();
    jsBindingData_ = *bindingData;
    return true;
}

FormProviderInfo *FormProviderInfo::Unmarshalling(Parcel &parcel)
{
    FormProviderInfo *formProviderInfo = new (std::nothrow) FormProviderInfo();
    if (formProviderInfo && !formProviderInfo->ReadFromParcel(parcel)) {
        delete formProviderInfo;
        formProviderInfo = nullptr;
    }
    return formProviderInfo;
}

bool FormProviderInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteInt64(formId_)) {
        return false;
    }
    if (!parcel.WriteParcelable(&jsBindingData_)) {
        return false;
    }
   
    return true;
}
void FormProviderInfo::SetFormDataString(std::string &dataString)
{
    jsBindingData_.SetDataString(dataString);
}
/**
 * @brief Merge new data to FormProviderData.
 * @param addJsonData data to merge to FormProviderData
 */   
void FormProviderInfo::MergeData(nlohmann::json &addJsonData)
{
    jsBindingData_.MergeData(addJsonData);
}
}  // namespace AppExecFwk
}  // namespace OHOS
