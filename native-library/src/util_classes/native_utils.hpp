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

#include <avsystem/commons/avs_time.h>

#include "./construct.hpp"
#include "./duration.hpp"
#include "./selectable_channel.hpp"

namespace utils {

struct NativeUtils {
    static constexpr auto Name() {
        return "com/avsystem/anjay/impl/NativeUtils";
    }

    struct ReadyState {
        bool read;
        bool write;
        bool accept;
        bool connect;

        static constexpr auto Name() {
            return "com/avsystem/anjay/impl/NativeUtils$ReadyState";
        }

        ReadyState()
                : read(false), write(false), accept(false), connect(false) {}

        ReadyState(const jni::Object<ReadyState> &state) : ReadyState() {
            auto accessor = AccessorBase<ReadyState>{ state };
            read = accessor.get_value<jni::jboolean>("read");
            write = accessor.get_value<jni::jboolean>("write");
            accept = accessor.get_value<jni::jboolean>("accept");
            connect = accessor.get_value<jni::jboolean>("connect");
        }

        jni::Local<jni::Object<ReadyState>> into_java() {
            return construct<ReadyState>(static_cast<jni::jboolean>(read),
                                         static_cast<jni::jboolean>(write),
                                         static_cast<jni::jboolean>(connect),
                                         static_cast<jni::jboolean>(accept));
        }
    };

    static ReadyState
    wait_until_ready(const jni::Object<SelectableChannel> &channel,
                     avs_time_duration_t timeout,
                     ReadyState waitStates) {
        return ReadyState(
                AccessorBase<NativeUtils>::get_static_method<
                        jni::Object<ReadyState>(jni::Object<SelectableChannel>,
                                                jni::Object<Duration>,
                                                jni::Object<ReadyState>)>(
                        "waitUntilReady")(channel, Duration::into_java(timeout),
                                          waitStates.into_java()));
    }
};

} // namespace utils
