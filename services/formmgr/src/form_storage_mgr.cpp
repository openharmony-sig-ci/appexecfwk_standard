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

#include <cinttypes>
#include <dirent.h>
#include <fstream>
#include <iomanip> 
#include <sys/stat.h>
#include <sys/types.h>

#include "app_log_wrapper.h"
#include "form_storage_mgr.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const char* FORM_DB_DATA_BASE_FILE_DIR = "/data/formmgr";
const int32_t FORM_DB_DATA_BASE_FILE_PATH_LEN = 255;
}

// bool FormStorageMgr::KeyToDeviceAndName(const std::string &key, std::string &deviceId, std::string &bundleName) const
// {
//     bool ret = false;
//     std::vector<std::string> splitStrs;
//     const std::string::size_type EXPECT_SPLIT_SIZE = 2;
//     OHOS::SplitStr(key, Constants::FILE_UNDERLINE, splitStrs);
//     // the expect split size should be 2.
//     // key rule is <deviceId>_<bundleName>
//     if (splitStrs.size() == EXPECT_SPLIT_SIZE) {
//         deviceId = splitStrs[0];
//         bundleName = splitStrs[1];
//         ret = true;
//     }
//     APP_LOGD("key = %{private}s, bundleName = %{public}s", key.c_str(), bundleName.c_str());
//     return ret;
// }

// void FormStorageMgr::DeviceAndNameToKey(
//     const std::string &deviceId, const std::string &bundleName, std::string &key) const
// {
//     key.append(deviceId);
//     key.append(Constants::FILE_UNDERLINE);
//     key.append(bundleName);
//     APP_LOGD("key = %{private}s, bundleName = %{public}s", key.c_str(), bundleName.c_str());
// }

// bool FormStorageMgr::bool LoadAllData(std::map<std::string, std::map<std::string, InnerFormInfo>> &infos) const
// {
//     bool ret = false;
//     APP_LOGI("load all installed bundle data to map");

//     std::fstream i(Constants::BUNDLE_DATA_BASE_FILE);
//     nlohmann::json jParse;
//     if (!i.is_open()) {
//         APP_LOGE("failed to open bundle database file");
//         // if file not exist, should create file here
//         std::ofstream o(Constants::BUNDLE_DATA_BASE_FILE);
//         o.close();
//         return false;
//     }
//     APP_LOGI("open bundle database file success");
//     i.seekg(0, std::ios::end);
//     int len = static_cast<int>(i.tellg());
//     if (len > 0) {
//         i.seekg(0, std::ios::beg);
//         i >> jParse;
//         for (auto &app : jParse.items()) {
//             std::map<std::string, InnerBundleInfo> deviceMap;
//             for (auto &device : app.value().items()) {
//                 InnerBundleInfo innerBundleInfo;
//                 ret = innerBundleInfo.FromJson(device.value());
//                 deviceMap.emplace(device.key(), innerBundleInfo);
//             }
//             auto pair = infos.emplace(app.key(), deviceMap);
//             ret = pair.second;
//         }
//     }
//     i.close();
//     return ret;
// }

/**
 * @brief Load form data from fileNamePath to innerFormInfos.
 * @param fileNamePath load file path.
 * @param innerFormInfos Save form data.
 * @return Returns true if the data is successfully loaded; returns false otherwise.
 */
static bool LoadFormDataFile(const char* fileNamePath, std::vector<InnerFormInfo> &innerFormInfos)
{
    bool ret = false;
    std::ifstream i(fileNamePath);
    if (!i.is_open()) {
        APP_LOGE("%{public}s, failed to open file[%{public}s]", __func__, fileNamePath);
        return false;
    }

    nlohmann::json jParse;
    i.seekg(0, std::ios::end);
    int len = static_cast<int>(i.tellg());
    if (len != 0) {
        i.seekg(0, std::ios::beg);
        i >> jParse;
        for (auto &it : jParse.items()) {
            InnerFormInfo innerFormInfo;
            if (innerFormInfo.FromJson(it.value())) {
                innerFormInfos.emplace_back(innerFormInfo);
            } else {
                APP_LOGE("%{public}s, failed to parse json, formId[%{public}s]", __func__, it.key().c_str());
            }
        }
        ret = true;
    } else {
        APP_LOGE("%{public}s, file[%{public}s] is empty", __func__, fileNamePath);
        ret = false;
    }
    i.close();
    return ret;
}

/**
 * @brief Load all form data from DB to innerFormInfos.
 * @param innerFormInfos Storage all form data.
 * @return Returns ERR_OK on success, others on failure.
 */
ErrCode FormStorageMgr::LoadFormData(std::vector<InnerFormInfo> &innerFormInfos) const
{
    APP_LOGI("%{public}s called.", __func__);
    DIR *dirptr = opendir(FORM_DB_DATA_BASE_FILE_DIR);
    if (dirptr == NULL) {
        APP_LOGE("%{public}s, opendir failed, should no formmgr dir", __func__);
        return ERR_APPEXECFWK_FORM_JSON_NO_DIR;
    }

    struct dirent *ptr;
    while ((ptr = readdir(dirptr)) != NULL) {
        APP_LOGI("%{public}s, readdir fileName[%{public}s]", __func__, ptr->d_name);
        if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0)) {
            continue;
        }
        char fileNamePath[FORM_DB_DATA_BASE_FILE_PATH_LEN] = {0};
        sprintf(fileNamePath, "%s/%s", FORM_DB_DATA_BASE_FILE_DIR, ptr->d_name);
        if (!LoadFormDataFile(fileNamePath, innerFormInfos)) {
            APP_LOGE("%{public}s, LoadFormDataFile failed, file[%{public}s]", __func__, ptr->d_name);
        }
    }
    APP_LOGI("%{public}s, readdir over", __func__);
    closedir(dirptr);
    return ERR_OK;
}

/**
 * @brief Get form data from DB to innerFormInfo with formId.
 * @param innerFormInfo Storage form data.
 * @return Returns ERR_OK on success, others on failure.
 */
ErrCode FormStorageMgr::GetStorageFormInfoById(const std::string &formId, InnerFormInfo &innerFormInfo) const
{
    ErrCode ret = ERR_OK;
    APP_LOGD("%{public}s called, formId[%{public}s]", __func__, formId.c_str());
    char fileNamePath[FORM_DB_DATA_BASE_FILE_PATH_LEN] = {0};
    sprintf(fileNamePath, "%s/%s.json", FORM_DB_DATA_BASE_FILE_DIR, formId.c_str());
    std::ifstream i(fileNamePath);
    nlohmann::json jParse;
    if (!i.is_open()) {
        APP_LOGE("%{public}s, open failed, should no this file[%{public}s.json]", __func__, formId.c_str());
        return ERR_APPEXECFWK_FORM_JSON_OPEN_FAIL;
    }
    APP_LOGD("%{public}s, open success file[%{public}s.json]", __func__, formId.c_str());
    i.seekg(0, std::ios::end);
    int len = static_cast<int>(i.tellg());
    if (len != 0) {
        i.seekg(0, std::ios::beg);
        i >> jParse;
        auto it = jParse.find(formId);
        if (it != jParse.end()) {
            if (innerFormInfo.FromJson(it.value()) == false) {
                APP_LOGE("%{public}s, fromJson parse failed formId[%{public}s]", __func__, it.key().c_str());
                ret = ERR_APPEXECFWK_FORM_JSON_PARSE_FAIL;
            } else {
                ret = ERR_OK;
            }
        } else {
            APP_LOGE("%{public}s, not find formId[%{public}s]", __func__, formId.c_str());
            ret = ERR_APPEXECFWK_FORM_JSON_FIND_FAIL;
        }
    } else {
        APP_LOGE("%{public}s, file is empty formId[%{public}s]", __func__, formId.c_str());
        ret = ERR_APPEXECFWK_FORM_JSON_FILE_EMPTY;
    }
    i.close();

    return ret;
}

/**
 * @brief Save or update the form data in DB.
 * @param innerFormInfo Indicates the InnerFormInfo object to be save.
 * @return Returns ERR_OK on success, others on failure.
 */
ErrCode FormStorageMgr::SaveStorageFormInfo(const InnerFormInfo &innerFormInfo) const
{
    //APP_LOGI("%{public}s called, formId[%{public}lld]", __func__, innerFormInfo.GetFormId());
    ErrCode ret = ERR_OK;
    std::string formId = std::to_string(innerFormInfo.GetFormId());

    DIR *dirptr = opendir(FORM_DB_DATA_BASE_FILE_DIR);
    if (dirptr == NULL) {
        APP_LOGW("%{public}s, failed to open dir", __func__);
        if (-1 == mkdir(FORM_DB_DATA_BASE_FILE_DIR, S_IRWXU)) {
            APP_LOGE("%{public}s, failed to create dir", __func__);
            return ERR_APPEXECFWK_FORM_JSON_CREATE_DIR_FAIL;
        }
    } else {
        closedir(dirptr);
    }
    char tmpFilePath[FORM_DB_DATA_BASE_FILE_PATH_LEN] = {0};
    sprintf(tmpFilePath, "%s/%s.json", FORM_DB_DATA_BASE_FILE_DIR, formId.c_str());

    std::fstream f(tmpFilePath);
    nlohmann::json jParse;
    if (!f.is_open()) {
        std::ofstream o(tmpFilePath); // if file not exist, should create file here
        if (!o.is_open()) {
            APP_LOGE("%{public}s, touch new file[%{public}s] failed", __func__, tmpFilePath);
            return ERR_APPEXECFWK_FORM_JSON_NEW_FILE_FAIL;
        }
        o.close();
        APP_LOGI("%{public}s, touch new file[%{public}s.json]", __func__, formId.c_str());
        f.open(tmpFilePath);
    }
    bool isExist = f.good();
    if (isExist) {
        nlohmann::json innerInfo;
        innerFormInfo.ToJson(innerInfo);
        f.seekg(0, std::ios::end);
        int len = static_cast<int>(f.tellg());
        if (len == 0) {
            nlohmann::json formRoot;
            formRoot[formId] = innerInfo;
            f << formRoot << std::endl;
        } else {
            APP_LOGE("%{public}s, file[%{public}s.json] is not empty", __func__, formId.c_str());
        }
    } else {
        APP_LOGE("%{public}s, touch new file[%{public}s] failed", __func__, formId.c_str());
        ret = ERR_APPEXECFWK_FORM_JSON_OPEN_FAIL;
    }
    f.close();
    return ret;
}

/**
 * @brief Modify the form data in DB.
 * @param innerFormInfo Indicates the InnerFormInfo object to be Modify.
 * @return Returns ERR_OK on success, others on failure.
 */
ErrCode FormStorageMgr::ModifyStorageFormInfo(const InnerFormInfo &innerFormInfo) const
{
    //APP_LOGI("%{public}s called, formId[%{public}lld]", __func__, innerFormInfo.GetFormId());
    char fileNamePath[FORM_DB_DATA_BASE_FILE_PATH_LEN] = {0};
    //sprintf(fileNamePath, "%s/%lld.json", FORM_DB_DATA_BASE_FILE_DIR, innerFormInfo.GetFormId());

    std::ofstream o(fileNamePath, std::ios_base::trunc | std::ios_base::out);
    if (!o.is_open()) {
        APP_LOGE("%{public}s, open failed file[%{public}s]", __func__, fileNamePath);
        return ERR_APPEXECFWK_FORM_JSON_OPEN_FAIL;
    }

    nlohmann::json innerInfo;
    innerFormInfo.ToJson(innerInfo);
    nlohmann::json formRoot;
    std::string formId = std::to_string(innerFormInfo.GetFormId());

    formRoot[formId] = innerInfo;
    o << formRoot << std::endl;

    o.close();
    return ERR_OK;
}

/**
 * @brief Delete the form data in DB.
 * @param formId The form data Id.
 * @return Returns ERR_OK on success, others on failure.
 */
ErrCode FormStorageMgr::DeleteStorageFormInfo(const std::string &formId) const
{
    APP_LOGI("%{public}s called, formId[%{public}s]", __func__, formId.c_str());
    char fileNamePath[FORM_DB_DATA_BASE_FILE_PATH_LEN] = {0};
    sprintf(fileNamePath, "%s/%s.json", FORM_DB_DATA_BASE_FILE_DIR, formId.c_str());

    if (std::remove(fileNamePath) != 0) {
        APP_LOGE("%{public}s, delete failed file[%{public}s]", __func__, fileNamePath);
        return ERR_APPEXECFWK_FORM_JSON_DELETE_FAIL;
    }

    return ERR_OK;
}
}  // namespace AppExecFwk
}  // namespace OHOS
