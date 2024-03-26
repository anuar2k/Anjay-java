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

#include "../jni_wrapper.hpp"

#include "./accessor_base.hpp"
#include "./optional_tag.hpp"

namespace utils {

class Optional {
    jni::Global<jni::Object<Optional>> self_;

public:
    static constexpr auto Name() {
        return OptionalTag::Name();
    }

    operator bool() const {
        return is_present();
    }

    Optional(const jni::Local<jni::Object<Optional>> &value)
            : self_(GlobalContext::call_with_env([&](auto &&env) {
                  return jni::NewGlobal(*env, value);
              })) {}

    jni::Local<jni::Object<Optional>> into_java() {
        return GlobalContext::call_with_env(
                [&](auto &&env) { return jni::NewLocal(*env, self_); });
    }

    bool is_present() const {
        return AccessorBase<Optional>{ self_ }.get_method<jni::jboolean()>(
                "isPresent")();
    }

    template <typename T>
    jni::Local<jni::Object<T>> get() {
        return GlobalContext::call_with_env([&](auto &&env) {
            return jni::Cast(
                    *env, jni::Class<T>::Find(*env),
                    AccessorBase<Optional>{ self_ }.get_method<jni::Object<>()>(
                            "get")());
        });
    }

    template <typename T>
    static Optional of(const T &value) {
        return Optional{ AccessorBase<Optional>::get_static_method<
                jni::Object<Optional>(jni::Object<>)>("of")(value) };
    }

    static Optional empty() {
        return Optional{
            AccessorBase<Optional>::get_static_method<jni::Object<Optional>()>(
                    "empty")()
        };
    }
};

} // namespace utils
