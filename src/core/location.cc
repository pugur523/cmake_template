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
  stack_trace_from_current_context(stack_trace_buf, kStackTraceBufSize, true, 1,
                                   kPlatformMaxFrames);
}

void Location::to_string(char* buf, std::size_t buf_size) const {
  char* cursor = buf;
  const char* end = buf + buf_size;

  write_format(cursor, end, "@{} {} at {}:{}", program_counter_, function_,
               file_, line_);

  if (cursor < end) {
    *cursor = '\0';
  } else if (buf_size > 0) {
    buf[buf_size - 1] = '\0';
  }
}

const char* Location::stack_trace() const {
  return stack_trace_buf;
}
}  // namespace core
