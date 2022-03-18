///
/// Copyright (c) 2022 Hiroshi Nakashima
///
/// This software is released under the MIT License, see LICENSE.
///
#ifndef CXXLOG_EXAMPLES_ADVANCED_HXX_
#define CXXLOG_EXAMPLES_ADVANCED_HXX_

#include <fstream>
#include <iostream>
#include <thread>

#include "cxxlog/cxxlog.hxx"

#define ADVANCED_LOG_I \
  CXXLOG_I(std::cout, Singleton::get_instance().get_stream()) \
    .cols(cxxlog::col::time(), cxxlog::col::severity(), thread_id_column())

class Singleton {
 public:
  static Singleton& get_instance();

  Singleton(const Singleton&) = delete;
  Singleton& operator=(const Singleton&) = delete;
  Singleton(Singleton&&) = delete;
  Singleton& operator=(Singleton&&) = delete;

  std::ostream& get_stream();

 private:
  Singleton();
  ~Singleton();

 private:
  std::ofstream stream_;
};

struct thread_id_column {
  void operator()(const cxxlog::col::arguments &args) {
    args.out << '[' << std::hex << std::this_thread::get_id() << ']';
  }
};

#endif  // CXXLOG_EXAMPLES_ADVANCED_HXX_
