/*
 * Copyright 2020-2024 AVSystem <avsystem@avsystem.com>
 *
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

#pragma once

#include <avsystem/commons/avs_socket.h>

#include "../jni_wrapper.hpp"

#include <unordered_map>

#include "./exception.hpp"

namespace utils {

struct DtlsVersion {
    static constexpr auto Name() {
        return "com/avsystem/anjay/Anjay$DtlsVersion";
    }

    static avs_net_ssl_version_t
    into_native(const jni::Object<DtlsVersion> &result) {
        static std::unordered_map<std::string, avs_net_ssl_version_t> MAPPING{
            { "DEFAULT", AVS_NET_SSL_VERSION_DEFAULT },
            { "SSLv2_OR_3", AVS_NET_SSL_VERSION_SSLv2_OR_3 },
            { "SSLv2", AVS_NET_SSL_VERSION_SSLv2 },
            { "SSLv3", AVS_NET_SSL_VERSION_SSLv3 },
            { "TLSv1", AVS_NET_SSL_VERSION_TLSv1 },
            { "TLSv1_1", AVS_NET_SSL_VERSION_TLSv1_1 },
            { "TLSv1_2", AVS_NET_SSL_VERSION_TLSv1_2 }
        };
        return GlobalContext::call_with_env([&result](auto &&env) {
            auto clazz = jni::Class<DtlsVersion>::Find(*env);
            auto value = jni::Make<std::string>(
                    *env,
                    result.Call(*env, clazz.template GetMethod<jni::String()>(
                                              *env, "name")));
            auto mapped_to = MAPPING.find(value);
            if (mapped_to == MAPPING.end()) {
                avs_throw(IllegalArgumentException("Unsupported enum value: "
                                                   + value));
            }
            return mapped_to->second;
        });
    }
};

} // namespace utils
