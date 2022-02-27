#
# Copyright (c) 2022 Hiroshi Nakashima
#
# This software is released under the MIT License, see LICENSE.
#
cmake_minimum_required(VERSION 3.10)

project(cxxlog VERSION 0.1.0)
message(STATUS "${PROJECT_NAME} ${PROJECT_VERSION}")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(cxxlog INTERFACE)
add_library(cxxlog::cxxlog ALIAS cxxlog)

target_include_directories(cxxlog INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_subdirectory(examples)

install(TARGETS cxxlog
    EXPORT cxxlog-config
)
install(EXPORT cxxlog-config
    NAMESPACE cxxlog::
    DESTINATION lib/cmake/cxxlog
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION include
)
