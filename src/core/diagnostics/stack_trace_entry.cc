#include "core/diagnostics/stack_trace_entry.h"

#include <cstddef>
#include <cstdio>

#include "core/base/string_util.h"

namespace core {

void StackTraceEntry::to_string(char* out_buf, std::size_t out_buf_size) const {
  char* cursor = out_buf;
  const char* const end = out_buf + out_buf_size;

  if (use_index) {
    std::size_t index_len = std::snprintf(nullptr, 0, "@%zu", index);
    write(cursor, end, "@%zu", index);
    padding(cursor, end, index_len, kIndexAlignLength);
  }

  if (address[0]) {
    std::size_t addr_len = std::strlen(address.data());
    write(cursor, end, "%s", address.data());
    padding(cursor, end, addr_len, kAddressAlignLength);
  }

  const char* func = function[0] ? function.data() : kUnknownFunction;
  std::size_t func_len = std::strlen(func);
  write(cursor, end, "%s", func);

  if (offset > 0) {
    int offset_len = std::snprintf(nullptr, 0, "+0x%zx", offset);
    write(cursor, end, "+0x%zx", offset);
    func_len += offset_len;
  }

  padding(cursor, end, func_len, kFunctionAlignLength);

  if (file[0]) {
    write(cursor, end, " at %s", file.data());
    if (line > 0) {
      write(cursor, end, ":%zu", line);
    }
  }

  // Ensure null termination
  if (cursor < end) {
    *cursor = '\0';
  } else if (out_buf_size > 0) {
    out_buf[out_buf_size - 1] = '\0';
  }
}

}  // namespace core
