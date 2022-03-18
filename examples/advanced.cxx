///
/// Copyright (c) 2022 Hiroshi Nakashima
///
/// This software is released under the MIT License, see LICENSE.
///
#include "advanced.hxx"

Singleton& Singleton::get_instance() {
  static Singleton instance;
  return instance;
}

Singleton::Singleton()
    : stream_("log_advanced.txt") {
  CXXLOG_I << "Singleton.ctor";
}

Singleton::~Singleton() {
  CXXLOG_I << "Singleton.dtor";
}

std::ostream& Singleton::get_stream() {
  return stream_;
}
