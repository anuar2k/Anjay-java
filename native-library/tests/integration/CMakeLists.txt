# Copyright 2020-2024 AVSystem <avsystem@avsystem.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set(INTEGRATION_TEST_SUITE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/suites")
set(INTEGRATION_TEST_PREFIX "test_anjay_jni_")
set(PYMBEDTLS_MODULE_DIR "${CMAKE_BINARY_DIR}/pymbedtls")

file(GLOB_RECURSE DEMO_TESTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/suites "${INTEGRATION_TEST_SUITE_ROOT}/*.py")
list(SORT DEMO_TESTS)

foreach(DEMO_TEST ${DEMO_TESTS})
    string(REGEX REPLACE "^${CMAKE_CURRENT_SOURCE_DIR}" "" DEMO_TEST "${DEMO_TEST}")
    string(REGEX REPLACE "\\.py" "" DEMO_TEST "${DEMO_TEST}")
    string(REGEX REPLACE "/" "." DEMO_TEST "${DEMO_TEST}")

    if (NOT DEMO_TEST MATCHES "__init__$")
        add_test(${INTEGRATION_TEST_PREFIX}${DEMO_TEST}
                 ${CMAKE_CURRENT_SOURCE_DIR}/runtest.py "^${DEMO_TEST}\\\$"
                                                        --client=${CMAKE_BINARY_DIR}/demo.jar
                                                        --target-logs-path=${CMAKE_CURRENT_BINARY_DIR}/log)

        # Tell python tests where to look for pymbedtls*.so
        set_property(TEST ${INTEGRATION_TEST_PREFIX}${DEMO_TEST}
                     APPEND PROPERTY ENVIRONMENT "PYTHONPATH=${PYMBEDTLS_MODULE_DIR}")
    endif()
endforeach()

if(CMAKE_MAJOR_VERSION LESS 3
        OR (CMAKE_MAJOR_VERSION EQUAL 3 AND CMAKE_MINOR_VERSION LESS 1))
    message(FATAL_ERROR "CMake >=3.1.0 is required to run integration tests")
endif()

set(_MISSING_PYTHON_MSG "If you have it installed on your system, but your cmake version is outdated you could try to restart the process that lead to this error message with: -DPython_ADDITIONAL_VERSIONS=3.x")

if(Python_ADDITIONAL_VERSIONS)
    find_package(PythonInterp)
else()
    find_package(PythonInterp 3)
endif()
if(NOT PYTHONINTERP_FOUND
        OR PYTHON_VERSION_MAJOR LESS 3
        OR (PYTHON_VERSION_MAJOR EQUAL 3 AND PYTHON_VERSION_MINOR LESS 6))
    message(FATAL_ERROR
            "Python >=3.6 is required for integration tests. ${_MISSING_PYTHON_MSG}")
endif()
find_package(PythonLibs "${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}")
if(NOT PYTHONLIBS_FOUND)
    message(FATAL_ERROR
            "libpython ${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR} is required for integration tests.")
endif()
unset(_MISSING_PYTHON_MSG)

add_custom_target(pymbedtls COMMAND
                  python3 "${CMAKE_CURRENT_SOURCE_DIR}/framework/nsh-lwm2m/pymbedtls/setup.py" install --install-lib ${PYMBEDTLS_MODULE_DIR})

include(ProcessorCount)
ProcessorCount(ANJAY_DEFAULT_NPROC)

set(NPROC "${ANJAY_DEFAULT_NPROC}" CACHE STRING "Number of threads for multi-threaded build/test operations")

add_custom_target(integration_check
                  COMMAND ${CMAKE_CTEST_COMMAND} -j ${NPROC} --output-on-failure
                  DEPENDS demo pymbedtls)
add_dependencies(check integration_check)
