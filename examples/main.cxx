///
/// Copyright (c) 2022 Hiroshi Nakashima
///
/// This software is released under the MIT License, see LICENSE.
///
#include <fstream>
#include <iostream>
#include <thread>

#include "cxxlog/cxxlog.hxx"

int main() {
  int count = 0;

  // basic usage (std::cout)
  CXXLOG_V << "verbose log. count=" << count++;
  CXXLOG_D << "debug log. count=" << count++;
  CXXLOG_I << "information log. count=" << count++;
  CXXLOG_W << "warning log. count=" << count++;
  CXXLOG_E << "error log. count=" << count++;
  CXXLOG_F << "fatal log. count=" << count++;

  // output stream: standard error
  std::ostream* out = &std::cerr;
  CXXLOG_E(out) << "standard error. count=" << count++;

  // output stream: nullptr (no output, but the expression is evaluated)
  out = nullptr;
  CXXLOG_I(out) << "out == nullptr. count=" << count++;
  out = &std::cout;
  CXXLOG_I(out) << "out != nullptr. count=" << count++;

  // output stream: file stream
  std::ofstream fs("log.txt");
  CXXLOG_V(&fs) << "verbose log";
  CXXLOG_D(&fs) << "debug log";
  CXXLOG_I(&fs) << "information log";
  CXXLOG_W(&fs) << "warning log";
  CXXLOG_E(&fs) << "error log";
  CXXLOG_F(&fs) << "fatal log";

  // checks log level
  if (CXXLOG_CHECK(cxxlog::info)) {
    CXXLOG(cxxlog::none) << "log level is info or higher";
  }
  if (CXXLOG_CHECK(cxxlog::debug)) {
    CXXLOG(cxxlog::none) << "log level is debug or higher";
  }

  // customize columns
  CXXLOG_E.cols() << "no columns";
  CXXLOG_E.cols(cxxlog::col::time()) << "time column only";
  CXXLOG_E.cols(cxxlog::col::severity(), cxxlog::col::time())
      << "multiple columns";

  // custom extension
  struct thread_id_column {
    void operator()(const cxxlog::col::arguments &args) {
      args.out << '[' << std::hex << std::this_thread::get_id() << ']';
    }
  };
  #define CUSTOM_LOG_I CXXLOG_I(&std::cout) \
      .cols(cxxlog::col::time(), cxxlog::col::severity(), thread_id_column())
  CUSTOM_LOG_I << "custom extension log";

  // thread safe
  std::thread thread([]{
    for (int i = 0; i < 100; ++i) {
      CUSTOM_LOG_I << i;
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  });
  for (int i = 0; i < 100; ++i) {
    CUSTOM_LOG_I << i;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  thread.join();

  return 0;
}
