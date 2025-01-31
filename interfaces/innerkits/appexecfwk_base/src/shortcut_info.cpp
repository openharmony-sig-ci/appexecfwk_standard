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
#include "shortcut_info.h"

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "json_serializer.h"
#include "nlohmann/json.hpp"
#include "string_ex.h"
#include "parcel_macro.h"
#include "app_log_wrapper.h"
#include "bundle_constants.h"

namespace OHOS {
namespace AppExecFwk {

namespace {

const std::string JSON_KEY_BUNDLE_ID = "id";
const std::string JSON_KEY_BUNDLE_NAME = "bundleName";
const std::string JSON_KEY_BUNDLE_HOST_ABILITY = "hostAbility";
const std::string JSON_KEY_BUNDLE_ICON = "icon";
const std::string JSON_KEY_BUNDLE_LABEL = "label";
const std::string JSON_KEY_BUNDLE_DISABLE_MESSAGE = "disableMessage";
const std::string JSON_KEY_BUNDLE_IS_STATIC = "isStatic";
const std::string JSON_KEY_BUNDLE_IS_HOME_SHORTCUT = "isHomeShortcut";
const std::string JSON_KEY_BUNDLE_IS_ENABLES = "isEnables";
const std::string JSON_KEY_BUNDLE_INTENTS = "intents";
const std::string JSON_KEY_BUNDLE_TARGET_BUNDLE = "targetBundle";
const std::string JSON_KEY_BUNDLE_TARGET_CLASS = "targetClass";

}  // namespace

bool ShortcutInfo::ReadFromParcel(Parcel &parcel)
{
    id = Str16ToStr8(parcel.ReadString16());
    bundleName = Str16ToStr8(parcel.ReadString16());
    hostAbility = Str16ToStr8(parcel.ReadString16());
    icon = Str16ToStr8(parcel.ReadString16());
    label = Str16ToStr8(parcel.ReadString16());
    disableMessage = Str16ToStr8(parcel.ReadString16());
    isStatic = parcel.ReadBool();
    isHomeShortcut = parcel.ReadBool();
    isEnables = parcel.ReadBool();
    int32_t intentsSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, intentsSize);
    for (auto i = 0; i < intentsSize; i++) {
        ShortcutIntent shortcutIntent;
        std::string targetBundleName = Str16ToStr8(parcel.ReadString16());
        std::string targetClassName = Str16ToStr8(parcel.ReadString16());
        shortcutIntent.targetBundle = targetBundleName;
        shortcutIntent.targetClass = targetClassName;
        intents.emplace_back(shortcutIntent);
    }
    return true;
}

ShortcutInfo *ShortcutInfo::Unmarshalling(Parcel &parcel)
{
    ShortcutInfo *info = new ShortcutInfo();
    if (!info->ReadFromParcel(parcel)) {
        APP_LOGW("read from parcel failed");
        delete info;
        info = nullptr;
    }
    return info;
}

bool ShortcutInfo::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(id));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(bundleName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(hostAbility));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(icon));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(label));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(disableMessage));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isStatic);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isHomeShortcut);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isEnables);

    const auto intentsSize = static_cast<int32_t>(intents.size());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, intentsSize);
    for (auto i = 0; i < intentsSize; i++) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(intents[i].targetBundle));
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(intents[i].targetClass));
    }
    return true;
}

void to_json(nlohmann::json &jsonObject, const ShortcutIntent &shortcutIntent)
{
    jsonObject = nlohmann::json{{JSON_KEY_BUNDLE_TARGET_BUNDLE, shortcutIntent.targetBundle},
        {JSON_KEY_BUNDLE_TARGET_CLASS, shortcutIntent.targetClass}};
}

void to_json(nlohmann::json &jsonObject, const ShortcutInfo &shortcutInfo)
{
    jsonObject = nlohmann::json{{JSON_KEY_BUNDLE_ID, shortcutInfo.id},
        {JSON_KEY_BUNDLE_NAME, shortcutInfo.bundleName},
        {JSON_KEY_BUNDLE_HOST_ABILITY, shortcutInfo.hostAbility},
        {JSON_KEY_BUNDLE_ICON, shortcutInfo.icon},
        {JSON_KEY_BUNDLE_LABEL, shortcutInfo.label},
        {JSON_KEY_BUNDLE_DISABLE_MESSAGE, shortcutInfo.disableMessage},
        {JSON_KEY_BUNDLE_IS_STATIC, shortcutInfo.isStatic},
        {JSON_KEY_BUNDLE_IS_HOME_SHORTCUT, shortcutInfo.isHomeShortcut},
        {JSON_KEY_BUNDLE_IS_ENABLES, shortcutInfo.isEnables},
        {JSON_KEY_BUNDLE_INTENTS, shortcutInfo.intents}};
}

void from_json(const nlohmann::json &jsonObject, ShortcutIntent &shortcutIntent)
{
    shortcutIntent.targetBundle = jsonObject.at(JSON_KEY_BUNDLE_TARGET_BUNDLE).get<std::string>();
    shortcutIntent.targetClass = jsonObject.at(JSON_KEY_BUNDLE_TARGET_CLASS).get<std::string>();
}

void from_json(const nlohmann::json &jsonObject, ShortcutInfo &shortcutInfo)
{
    shortcutInfo.id = jsonObject.at(JSON_KEY_BUNDLE_ID).get<std::string>();
    shortcutInfo.bundleName = jsonObject.at(JSON_KEY_BUNDLE_NAME).get<std::string>();
    shortcutInfo.hostAbility = jsonObject.at(JSON_KEY_BUNDLE_HOST_ABILITY).get<std::string>();
    shortcutInfo.icon = jsonObject.at(JSON_KEY_BUNDLE_ICON).get<std::string>();
    shortcutInfo.label = jsonObject.at(JSON_KEY_BUNDLE_LABEL).get<std::string>();
    shortcutInfo.disableMessage = jsonObject.at(JSON_KEY_BUNDLE_DISABLE_MESSAGE).get<std::string>();
    shortcutInfo.isStatic = jsonObject.at(JSON_KEY_BUNDLE_IS_STATIC).get<bool>();
    shortcutInfo.isHomeShortcut = jsonObject.at(JSON_KEY_BUNDLE_IS_HOME_SHORTCUT).get<bool>();
    shortcutInfo.isEnables = jsonObject.at(JSON_KEY_BUNDLE_IS_ENABLES).get<bool>();
    shortcutInfo.intents = jsonObject.at(JSON_KEY_BUNDLE_INTENTS).get<std::vector<ShortcutIntent>>();
}

}  // namespace AppExecFwk
}  // namespace OHOS
