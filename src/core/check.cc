#include "core/check.h"

#include <iostream>

namespace core {

CheckFailureStream::CheckFailureStream(const char* type,
                                       const char* file,
                                       int line,
                                       const char* condition)
    : type_(type), file_(file), line_(line), condition_(condition) {}

std::ostream& CheckFailureStream::stream() {
  return std::cout << type_ << " failed: \"" << condition_ << "\" at " << file_
                   << ":" << line_ << "\n";
}

CheckFailureStream::~CheckFailureStream() {
  std::cout << std::flush;

  std::terminate();
}

}  // namespace core
