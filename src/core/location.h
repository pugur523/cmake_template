#ifndef CORE_LOCATION_H_
#define CORE_LOCATION_H_

#include <ostream>
#include <string>

#include "build/build_flag.h"
#include "core/base/core_export.h"
#include "core/diagnostics/stack_trace.h"

#if ENABLE_LLVM_UNWIND
#include <libunwind.h>
#endif

namespace core {

class CORE_EXPORT Location {
 public:
  Location(const char* file,
           int line,
           const char* function,
           const void* program_counter);

  ~Location();

  constexpr const char* file() const { return file_; }
  constexpr int line() const { return line_; }
  constexpr const char* function() const { return function_; }
  constexpr const void* program_counter() const { return program_counter_; }

  explicit operator std::string() const {
    char buf[1024];
    to_string(buf, 1024);
    return buf;
  }
  void to_string(char* buf, std::size_t buf_size) const;

  friend std::ostream& operator<<(std::ostream& os, const Location& location) {
    return os << static_cast<std::string>(location);
  }

  std::string stack_trace(bool use_index = true,
                          std::size_t first_frame = 0,
                          std::size_t max_frames = kPlatformMaxFrames / 2);

 private:
  const char* file_;
  int line_;
  const char* function_;
  const void* program_counter_;

#if ENABLE_LLVM_UNWIND
  unw_context_t context_;
#endif
};

}  // namespace core

#if COMPILER_CLANG || COMPILER_GCC
#define FUNCTION_SIGNATURE __PRETTY_FUNCTION__
#elif COMIPLER_MSVC
#define FUNCTION_SIGNATURE __FUNCSIG__
#else
#define FUNCTION_SIGNATURE __func__
#endif

#define FROM_HERE                                          \
  ::core::Location(__FILE__, __LINE__, FUNCTION_SIGNATURE, \
                   __builtin_return_address(0))

#endif  // CORE_LOCATION_H_
