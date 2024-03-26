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

#include "../util_classes/accessor_base.hpp"
#include "../util_classes/construct.hpp"

#include <cstring>
#include <string>

namespace compat {

struct SocketAddress {
    static constexpr auto Name() {
        return "java/net/SocketAddress";
    }
};

class InetAddress {
    jni::Global<jni::Object<InetAddress>> self_;

public:
    static constexpr auto Name() {
        return "java/net/InetAddress";
    }

    InetAddress(const jni::Local<jni::Object<InetAddress>> &self)
            : self_(GlobalContext::call_with_env([&](auto &&env) {
                  return jni::NewGlobal(*env, self);
              })) {}

    jni::Local<jni::Object<InetAddress>> into_java() const {
        return GlobalContext::call_with_env(
                [&](auto &&env) { return jni::NewLocal(*env, self_); });
    }

    std::string get_host_address() {
        return GlobalContext::call_with_env([&](auto &&env) {
            return jni::Make<std::string>(
                    *env,
                    utils::AccessorBase<InetAddress>{ self_ }
                            .get_method<jni::String()>("getHostAddress")());
        });
    }

    std::string get_host_name() {
        return GlobalContext::call_with_env([&](auto &&env) {
            return jni::Make<std::string>(
                    *env,
                    utils::AccessorBase<InetAddress>{ self_ }
                            .get_method<jni::String()>("getHostName")());
        });
    }

    bool is_ipv4() {
        return strchr(get_host_address().c_str(), ':') != nullptr;
    }

    static std::vector<InetAddress> get_all_by_name(const std::string &host) {
        std::vector<InetAddress> result{};
        GlobalContext::call_with_env([&](auto &&env) {
            auto resolved_addrs =
                    utils::AccessorBase<InetAddress>::get_static_method<
                            jni::Array<jni::Object<InetAddress>>(jni::String)>(
                            "getAllByName")(jni::Make<jni::String>(*env, host));

            for (jni::jsize i = 0; i < resolved_addrs.Length(*env); ++i) {
                result.emplace_back(resolved_addrs.Get(*env, i));
            }
        });
        return result;
    }
};

struct InetSocketAddress {
    static constexpr auto Name() {
        return "java/net/InetSocketAddress";
    }

    static jni::Local<jni::Object<SocketAddress>>
    from_host_port(const std::string &host, int port) {
        return GlobalContext::call_with_env([&](auto &&env) {
            return jni::Cast<SocketAddress>(
                    *env, jni::Class<SocketAddress>::Find(*env),
                    utils::construct<InetSocketAddress>(
                            jni::Make<jni::String>(*env, host), port));
        });
    }

    static jni::Local<jni::Object<SocketAddress>>
    from_host_port(const std::string &host, const std::string &port) {
        return from_host_port(host, std::stoi(port));
    }

    static jni::Local<jni::Object<SocketAddress>>
    from_port(const std::string &port) {
        return GlobalContext::call_with_env([&](auto &&env) {
            return jni::Cast<SocketAddress>(
                    *env, jni::Class<SocketAddress>::Find(*env),
                    utils::construct<InetSocketAddress>(std::stoi(port)));
        });
    }

    static jni::Local<jni::Object<SocketAddress>>
    from_resolved(const InetAddress &address, int port) {
        return GlobalContext::call_with_env([&](auto &&env) {
            return jni::Cast<SocketAddress>(
                    *env, jni::Class<SocketAddress>::Find(*env),
                    utils::construct<InetSocketAddress>(address.into_java(),
                                                        port));
        });
    }
};

} // namespace compat
