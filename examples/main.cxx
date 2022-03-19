///
/// Copyright (c) 2022 Hiroshi Nakashima
///
/// This software is released under the MIT License, see LICENSE.
///
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>

#include "cxxlog/cxxlog.hxx"
#include "advanced.hxx"

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
  CXXLOG_E(std::cerr) << "standard error ref. count=" << count++;
  CXXLOG_E(&std::cerr) << "standard error ptr. count=" << count++;

  // output stream: nullptr (no output, but the expression is evaluated)
  std::ostream* ptr = nullptr;
  CXXLOG_I(ptr) << "stream == nullptr. count=" << count++;
  ptr = &std::cout;
  CXXLOG_I(ptr) << "stream != nullptr. count=" << count++;

  CXXLOG_I(nullptr) << "nullptr. count=" << count++;
  CXXLOG_I() << "default. count=" << count++;

  // output stream: file stream
  std::ofstream fs("log.txt");
  CXXLOG_V(&fs) << "verbose log";
  CXXLOG_D(&fs) << "debug log";
  CXXLOG_I(&fs) << "information log";
  CXXLOG_W(&fs) << "warning log";
  CXXLOG_E(&fs) << "error log";
  CXXLOG_F(&fs) << "fatal log";

  // output stream: multiple
  CXXLOG_E(std::cerr, fs) << "multiple output streams (ref)";
  CXXLOG_E(&std::cerr, &fs) << "multiple output streams (ptr)";
  CXXLOG_E(std::cerr, &fs) << "multiple output streams (mix)";

  // checks log level
  if (CXXLOG_CHECK(cxxlog::warning)) {
    CXXLOG(cxxlog::none) << "warning or higher";
  }
  if (CXXLOG_CHECK(cxxlog::info)) {
    CXXLOG(cxxlog::none) << "info or higher";
  }
  if (CXXLOG_CHECK(cxxlog::debug)) {
    CXXLOG(cxxlog::none) << "debug or higher";
  }

  // customize columns
  CXXLOG_E.cols() << "----------------------- no columns";
  CXXLOG_E.cols(cxxlog::col::time()) << "----- time column only";

  // advanced
  ADVANCED_LOG_I << "advanced log";

  // thread safe
  std::thread thread([]{
    for (int i = 0; i < 100; ++i) {
      ADVANCED_LOG_I << i;
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  });
  for (int i = 0; i < 100; ++i) {
    ADVANCED_LOG_I << i;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  thread.join();

  return 0;
}
