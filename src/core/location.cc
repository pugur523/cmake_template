#include "core/location.h"

#include <cstdio>
#include <cstring>
#include <string>

#include "core/base/string_util.h"
#include "core/diagnostics/stack_trace.h"

namespace core {

Location::Location(const char* file,
                   int line,
                   const char* function,
                   const void* program_counter)
    : file_(file),
      line_(line),
      function_(function),
      program_counter_(program_counter) {
#if ENABLE_LLVM_UNWIND
  unw_getcontext(&context_);
#endif
}

Location::~Location() = default;

void Location::to_string(char* buf, std::size_t buf_size) const {
  char* cursor = buf;
  const char* end = buf + buf_size;

  write(cursor, end, "%s", file_);
  write(cursor, end, "%s", ":");
  write(cursor, end, "%d", line_);
  write(cursor, end, "%s", " (");
  write(cursor, end, "%s", function_);
  write(cursor, end, "%s", ") @");
  write(cursor, end, "%p", program_counter_);

  if (cursor < end) {
    *cursor = '\0';
  } else if (buf_size > 0) {
    buf[buf_size - 1] = '\0';
  }
}

// static
std::string Location::stack_trace(bool use_index,
                                  std::size_t first_frame,
                                  std::size_t max_frames) {
#if ENABLE_LLVM_UNWIND
  return stack_trace_with_libunwind(&context_, use_index, first_frame,
                                    max_frames);
#else
  return stack_trace_from_current_context(use_index, first_frame, max_frames);
#endif
}

}  // namespace core
