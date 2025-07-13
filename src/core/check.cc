#include "core/check.h"

#include <exception>
#include <iostream>

#include "core/base/logger.h"

namespace core {

CheckFailureStream::CheckFailureStream(const char* type,
                                       const char* file,
                                       int line,
                                       const char* condition)
    : type_(type), file_(file), line_(line), condition_(condition) {}

std::ostream& CheckFailureStream::stream() {
  has_output_ = true;
  glog.fatal<"{} failed: \"{}\" at {}:{}\n">(type_, condition_, file_, line_);
  return std::cerr;
}

CheckFailureStream::~CheckFailureStream() {
  if (has_output_) {
    std::cerr << std::flush;
  }
  std::terminate();
}

}  // namespace core
