#
# Copyright (c) 2022 Hiroshi Nakashima
#
# This software is released under the MIT License, see LICENSE.
#
find_package(Threads REQUIRED)

add_executable(example main.cxx)
target_link_libraries(example cxxlog::cxxlog Threads::Threads)
target_compile_definitions(example PRIVATE CXXLOG_LEVEL=cxxlog::info)
if(MSVC)
    target_compile_options(example PRIVATE /W4)
else()
    target_compile_options(example PRIVATE -Wall -Wextra -Wpedantic)
endif()
