///
/// Copyright (c) 2022 Hiroshi Nakashima
///
/// This software is released under the MIT License, see LICENSE.
///
/// @file
///
#ifndef CXXLOG_CXXLOG_HXX_
#define CXXLOG_CXXLOG_HXX_

#include <chrono>
#include <cstddef>
#include <functional>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <vector>

/// @brief Specifies the log level
///
/// This definition is specified by `target_compile_definitions` of cmake.
/// @code
/// target_compile_definitions(<target> PRIVATE CXXLOG_LEVEL=cxxlog::info)
/// @endcode
/// @see cxxlog::severity_t
#ifndef CXXLOG_LEVEL
#define CXXLOG_LEVEL cxxlog::error
#endif  // CXXLOG_LEVEL

/// @brief Macro that checks log level
/// @code {.cxx}
/// if (CXXLOG_CHECK(cxxlog::info)) { /* info or higher */ }
/// @endcode
#define CXXLOG_CHECK(level) (CXXLOG_LEVEL >= level)

/// @brief Macro that optimizes logs by short-circuit evaluation
///
/// If this macro is expanded and the left side is false, the right side
/// will not be evaluated. It will be optimized by many compilers.
///
/// Examples:
/// @code {.cxx}
/// // CXXLOG_LEVEL cxxlog::info
///
/// // CXXLOG_I << "logger is enabled";
/// true  && (bool)(cxxlog::Logger(cxxlog::info)  << "logger is enabled");
/// ^^^^^^^^
///
/// // CXXLOG_D << "logger is disabled";
/// false && (bool)(cxxlog::Logger(cxxlog::debug) << "logger is disabled");
/// ^^^^^^^^
/// @endcode
#define CXXLOG(severity) CXXLOG_CHECK(severity) && cxxlog::Logger(severity)

/// @brief Macro for fatal log
/// @code {.cxx}
/// CXXLOG_F << "fatal log";
/// @endcode
#define CXXLOG_F CXXLOG(cxxlog::fatal)

/// @brief Macro for error log
/// @code {.cxx}
/// CXXLOG_E << "error log";
/// @endcode
#define CXXLOG_E CXXLOG(cxxlog::error)

/// @brief Macro for warning log
/// @code {.cxx}
/// CXXLOG_W << "warning log";
/// @endcode
#define CXXLOG_W CXXLOG(cxxlog::warning)

/// @brief Macro for information log
/// @code {.cxx}
/// CXXLOG_I << "information log";
/// @endcode
#define CXXLOG_I CXXLOG(cxxlog::info)

/// @brief Macro for debug log
/// @code {.cxx}
/// CXXLOG_D << "debug log";
/// @endcode
#define CXXLOG_D CXXLOG(cxxlog::debug)

/// @brief Macro for verbose log
/// @code {.cxx}
/// CXXLOG_V << "verbose log";
/// @endcode
#define CXXLOG_V CXXLOG(cxxlog::verbose)

/// @brief Namespace of cxxlog
namespace cxxlog {

/// @brief Log severity and log level
///
/// The log level can be specified by @ref CXXLOG_LEVEL.
enum severity_t {
  /// @brief If set to `CXXLOG_LEVEL`, no log will be output
  none = 0,
  /// @brief If set to `CXXLOG_LEVEL`, only fatal logs will be output
  fatal = 1,
  /// @brief If set to `CXXLOG_LEVEL`, more than error log will be output
  error = 2,
  /// @brief If set to `CXXLOG_LEVEL`, more than warning log will be output
  warning = 3,
  /// @brief If set to `CXXLOG_LEVEL`, more than information log will be output
  info = 4,
  /// @brief If set to `CXXLOG_LEVEL`, more than debug log will be output
  debug = 5,
  /// @brief If set to `CXXLOG_LEVEL`, all logs will be output
  verbose = 6,
};

/// @brief Namespace of cxxlog column
namespace col {

/// @brief Arguments for column function
struct arguments {
  std::ostream &out;
  severity_t severity;
};

/// @brief Alias for column function
using Function = std::function<void (const arguments &)>;

/// @brief Column of time
struct time {
  void operator()(const arguments &args) {
    const auto usec = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    args.out << std::setw(10) << (usec / 1000000) << '.'
        << std::setw(6) << std::setfill('0') << (usec % 1000000);
  }
};

/// @brief Column of severity
struct severity {
  void operator()(const arguments &args) {
    const char *severity_string[] = {
        "     ",  // none
        "FATAL",  // fatal
        "ERROR",  // error
        "WARN ",  // warning
        "INFO ",  // info
        "DEBUG",  // debug
        "VERB ",  // verbose
    };
    args.out << severity_string[args.severity];
  }
};

}  // namespace col

namespace detail {

inline std::mutex& get_mutex() {
  static std::mutex _mutex;
  return _mutex;
}

template<typename T>
T* to_ptr(T &obj) {
  return &obj;
}

template<typename T>
T* to_ptr(T *obj) {
  return obj;
}

inline std::nullptr_t to_ptr(std::nullptr_t) {
  return nullptr;
}

inline void add_streams(std::vector<std::ostream*>*) {
}

template<typename Stream, typename... Args>
void add_streams(
    std::vector<std::ostream*> *streams, Stream &&stream, Args &&...args) {
  auto stream_ptr = to_ptr(std::forward<Stream>(stream));
  if (stream_ptr != nullptr) {
    streams->push_back(stream_ptr);
  }
  add_streams(streams, std::forward<Args>(args)...);
}

}  // namespace detail

/// @brief A simple logger that wraps the output stream
class Logger {
 public:
  /// @brief Constructor
  /// @param[in] severity - log severity
  explicit Logger(severity_t severity)
      : severity_(severity),
        buffer_(),
        streams_({ &std::cout }),
        columns_({ col::time(), col::severity() }) {
  }

  /// @brief Destructor
  ///
  /// If data is inserted, flush it.
  ~Logger() {
    if (!streams_.empty() && (buffer_.tellp() != 0)) {
      buffer_ << std::endl;
      const auto str = buffer_.str();
      std::lock_guard<std::mutex> lock(detail::get_mutex());
      for (auto out : streams_) {
        *out << str;
      }
    }
  }

  /// @brief Specifies the output streams
  ///
  /// Examples:
  /// @code {.cxx}
  /// CXXLOG_E(std::cerr) << "standard error ref";
  /// CXXLOG_E(&std::cerr) << "standard error ptr";
  ///
  /// std::ofstream fs("log.txt");
  /// CXXLOG_E(&fs) << "file stream";
  ///
  /// CXXLOG_E(std::cerr, fs) << "multiple outputs";
  /// @endcode
  /// @param[in] output_streams - output streams
  template<typename... OutputStreams>
  Logger& operator()(OutputStreams &&...output_streams) {
    std::vector<std::ostream*> streams;
    detail::add_streams(
        &streams, std::forward<OutputStreams>(output_streams)...);
    streams_.swap(streams);
    return *this;
  }

  Logger& operator()() {
    return *this;
  }

  /// @brief Specifies the columns
  ///
  /// Examples:
  /// @code {.cxx}
  /// CXXLOG_E.cols() << "no columns";
  /// CXXLOG_E.cols(cxxlog::col::time()) << "time column only";
  /// CXXLOG_E.cols(cxxlog::col::severity(), cxxlog::col::time())
  ///     << "multiple columns";
  /// @endcode
  /// @param[in] functions - column funcions
  template<typename... ColumnFunctions>
  Logger& cols(ColumnFunctions &&...functions) {
    if (buffer_.tellp() == 0) {
      std::vector<col::Function> columns { functions... };
      columns_.swap(columns);
    }
    return *this;
  }

  /// @brief Inserts data into the output stream
  /// @param[in] value - value to insert
  template<typename T>
  Logger& operator<<(T &&value) {
    if (!streams_.empty()) {
      if ((buffer_.tellp() == 0) && !columns_.empty()) {
        const auto flags = buffer_.flags();
        for (const auto &column : columns_) {
          column({ buffer_, severity_ });
          buffer_.flags(flags);
          buffer_ << ' ';
        }
      }
      buffer_ << std::forward<T>(value);
    }
    return *this;
  }

  /// @brief boolean conversion for short-circuit evaluation
  /// @see CXXLOG
  explicit operator bool() const {
    return true;
  }

 private:
  const severity_t severity_;
  std::ostringstream buffer_;
  std::vector<std::ostream*> streams_;
  std::vector<col::Function> columns_;
};

}  // namespace cxxlog

#endif  // CXXLOG_CXXLOG_HXX_
