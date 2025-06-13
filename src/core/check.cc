#include "core/check.h"

#include <iostream>

namespace core {

CheckFailureStream::CheckFailureStream(const char* type,
                                       const char* file,
                                       int line,
                                       const char* condition)
    : type_(type), file_(file), line_(line), condition_(condition) {}

const std::ostream& CheckFailureStream::stream() {
  return std::cerr << type_ << " failed: \"" << condition_ << "\" at " << file_
                   << "#" << line_ << "\n";
}

CheckFailureStream::~CheckFailureStream() {
  std::cerr << std::flush;

  std::exit(-1);
}

}  // namespace core
