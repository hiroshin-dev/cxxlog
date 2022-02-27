#
# Copyright (c) 2022 Hiroshi Nakashima
#
# This software is released under the MIT License, see LICENSE.
#
find_package(Threads REQUIRED)

add_executable(cxxlog_example main.cxx)
target_link_libraries(cxxlog_example cxxlog::cxxlog Threads::Threads)
target_compile_definitions(cxxlog_example PRIVATE CXXLOG_LEVEL=cxxlog::info)
if(MSVC)
    target_compile_options(cxxlog_example PRIVATE /W4)
else()
    target_compile_options(cxxlog_example PRIVATE -Wall -Wextra -Wpedantic)
endif()
