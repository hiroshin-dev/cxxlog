# cxxlog

## Overview

cxxlog is a simple C++ logging library.

Example:

```cpp
#include <fstream>
#include "cxxlog/cxxlog.hxx"

int main() {
  // basic usage (std::cout)
  CXXLOG_V << "verbose log";
  CXXLOG_D << "debug log";
  CXXLOG_I << "information log";
  CXXLOG_W << "warning log";
  CXXLOG_E << "error log";
  CXXLOG_F << "fatal log";

  // file stream
  std::ofstream fs("log.txt");
  CXXLOG_V(&fs) << "verbose log";
  CXXLOG_D(&fs) << "debug log";
  CXXLOG_I(&fs) << "information log";
  CXXLOG_W(&fs) << "warning log";
  CXXLOG_E(&fs) << "error log";
  CXXLOG_F(&fs) << "fatal log";

  return 0;
}
```

Output (when `CXXLOG_LEVEL=cxxlog::info`):

```none
1645872675.011621 INFO  information log
1645872675.011638 WARN  warning log
1645872675.011656 ERROR error log
1645872675.011671 FATAL fatal log
```

## Features

- Header only
- Cross platform: Linux, Windows
- Thread safe

## Requirement

- C++11

## Usage

### Building with CMake

Here's how to include cxxlog in CMake project:

`CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.11)
project(my_project)

# cxxlog requires at least C++11
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
    cxxlog
    GIT_REPOSITORY https://github.com/hiroshin-dev/cxxlog.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(cxxlog)

add_executable(example main.cxx)
target_link_libraries(example cxxlog::cxxlog)
target_compile_definitions(example PRIVATE CXXLOG_LEVEL=cxxlog::info)
```

#### Log level

The log level can be specified by `target_compile_definitions`.

```cmake
target_compile_definitions(example PRIVATE CXXLOG_LEVEL=cxxlog::info)
```

| CXXLOG_LEVEL      | Description                              |
|-------------------|------------------------------------------|
| `cxxlog::none`    | No log will be output                    |
| `cxxlog::fatal`   | Only fatal logs will be output           |
| `cxxlog::error`   | More than error log will be output       |
| `cxxlog::warning` | More than warning log will be output     |
| `cxxlog::info`    | More than information log will be output |
| `cxxlog::debug`   | More than debug log will be output       |
| `cxxlog::verbose` | All logs will be output                  |

## License

cxxlog is under the [MIT License](LICENSE)
