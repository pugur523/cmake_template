#include "core/diagnostics/stack_trace.h"

#include <algorithm>
#include <cstdio>
#include <string>

#include "build/build_flag.h"
#include "core/diagnostics/stack_trace_entry.h"

#if IS_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#define NOMINMAX
// clang-format off
#include <windows.h>
#include <dbghelp.h>
// clang-format on
#elif IS_UNIX
#include <cxxabi.h>
#include <dlfcn.h>
#include <unistd.h>
#if ENABLE_LLVM_UNWIND
#include <libunwind.h>
#else
#include <execinfo.h>
#endif
#endif

namespace core {

namespace {

constexpr std::size_t kLineBufferSize = 1024;
#if IS_UNIX
constexpr std::size_t kSymbolBufferSize = 512;
constexpr std::size_t kDemangledBufferSize = 512;
#endif

}  // namespace

void stack_trace_entries_to_string(
    const StackTraceEntry entries[kPlatformMaxFrames],
    std::size_t count,
    std::string* out) {
  char line_buffer[1024];
  for (std::size_t i = 0; i < count; ++i) {
    entries[i].to_string(line_buffer, sizeof(line_buffer));
    out->append(line_buffer);
    out->push_back('\n');
  }
}

void stack_trace_entries_to_buffer(
    const StackTraceEntry entries[kPlatformMaxFrames],
    std::size_t count,
    char* buffer,
    std::size_t buffer_size) {
  char line_buffer[kLineBufferSize];
  std::size_t written = 0;

  for (std::size_t i = 0; i < count && written < buffer_size - 1; ++i) {
    entries[i].to_string(line_buffer, sizeof(line_buffer));
    std::size_t line_len = std::strlen(line_buffer);

    if (written + line_len + 1 < buffer_size) {
      std::snprintf(buffer + written, sizeof(buffer + written), "%s",
                    line_buffer);
      written += line_len;
      buffer[written++] = '\n';
    } else {
      break;
    }
  }
  buffer[written] = '\0';
}

std::size_t collect_stack_trace(
#if ENABLE_LLVM_UNWIND
    unw_context_t* context,
#endif
    StackTraceEntry out[kPlatformMaxFrames],
    bool use_index,
    std::size_t first_frame,
    std::size_t max_frames) {
  max_frames = std::min(max_frames, kPlatformMaxFrames);
  first_frame = std::min(first_frame, max_frames);

#if ENABLE_LLVM_UNWIND
  unw_cursor_t cursor;
  if (unw_init_local(&cursor, context) != 0) {
    return 0;
  }

  std::size_t i = 0;
  char symbol[kSymbolBufferSize];
  char address_buf[kAddressStrLength];
  char demangled_buf[kDemangledBufferSize];

  std::size_t count = 0;
  while (unw_step(&cursor) > 0 && count < max_frames) {
    if (i++ < first_frame) {
      continue;
    }

    unw_word_t addr;
    if (unw_get_reg(&cursor, UNW_REG_IP, &addr) != 0) {
      break;
    }

    std::snprintf(address_buf, sizeof(address_buf), "0x%016lx",
                  static_cast<std::size_t>(addr));
    const char* addr_str = address_buf;

    const char* function = "";
    unw_word_t offset = 0;

    if (unw_get_proc_name(&cursor, symbol, sizeof(symbol), &offset) == 0) {
      int status;
      std::size_t demangled_len = kDemangledBufferSize;
      if (abi::__cxa_demangle(symbol, demangled_buf, &demangled_len, &status) !=
              nullptr &&
          status == 0) {
        function = demangled_buf;
      } else {
        function = symbol;
      }
    }

    const char* file = "";
    Dl_info info{};
    if (dladdr(reinterpret_cast<void*>(static_cast<uintptr_t>(addr)), &info) &&
        info.dli_fname) {
      file = info.dli_fname;
    }

    StackTraceEntry entry;
    entry.index = i - first_frame;
    std::strncpy(entry.address.data(), addr_str, kAddressStrLength - 1);
    std::strncpy(entry.function.data(), function, kFunctionStrLength - 1);
    std::strncpy(entry.file.data(), file, kFileStrLength - 1);
    entry.line = 0;
    entry.offset = offset;
    entry.use_index = use_index;
    out[count++] = entry;
  }
  return count;
#elif IS_UNIX
  void* stack[kPlatformMaxFrames];
  int frames = backtrace(stack, static_cast<int>(max_frames));
  char** strs = backtrace_symbols(stack, frames);
  if (!strs) {
    return 0;
  }

  char address_buf[kAddressStrLength];
  char demangled_buf[kDemangledBufferSize];
  std::size_t count = 0;

  for (std::size_t i = first_frame; i < static_cast<std::size_t>(frames); i++) {
    const char* frame = strs[i];
    if (!frame) {
      continue;
    }

    const char* addr_begin = strchr(frame, '[');
    const char* addr_end = strchr(frame, ']');
    const char* addr_str = "";
    if (addr_begin && addr_end && addr_end > addr_begin) {
      std::snprintf(address_buf, sizeof(address_buf), "%.*s",
                    static_cast<int>(addr_end - addr_begin - 1),
                    addr_begin + 1);
      addr_str = address_buf;
    }

    const char* function = frame;
    int status;
    char mangled_buf[256];

    const char* lparen = strchr(frame, '(');
    const char* plus = strchr(frame, '+');
    if (lparen && plus && plus > lparen) {
      std::size_t mangled_len = std::min(
          static_cast<std::size_t>(plus - lparen - 1), sizeof(mangled_buf) - 1);
      std::strncpy(mangled_buf, lparen + 1, mangled_len);
      mangled_buf[mangled_len] = '\0';

      std::size_t demangled_len = kDemangledBufferSize;
      if (abi::__cxa_demangle(mangled_buf, demangled_buf, &demangled_len,
                              &status) != nullptr &&
          status == 0) {
        function = demangled_buf;
      } else {
        function = mangled_buf;
      }
    }

    const char* file = "";
    std::size_t offset = 0;
    Dl_info info{};
    if (dladdr(stack[i], &info)) {
      if (info.dli_fname) {
        file = info.dli_fname;
      }
      if (info.dli_saddr) {
        std::uintptr_t base = reinterpret_cast<std::uintptr_t>(info.dli_saddr);
        std::uintptr_t addr = reinterpret_cast<std::uintptr_t>(stack[i]);
        offset = addr - base;
      }
    }

    StackTraceEntry entry;
    entry.index = i - first_frame;
    std::strncpy(entry.address.data(), addr_str, kAddressStrLength - 1);
    std::strncpy(entry.function.data(), function, kFunctionStrLength - 1);
    std::strncpy(entry.file.data(), file, kFileStrLength - 1);
    entry.line = 0;
    entry.offset = offset;
    entry.use_index = use_index;
    out[count++] = entry;
  }

  free(strs);
  return count;
#elif IS_WINDOWS
  SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME | SYMOPT_LOAD_LINES);
  HANDLE process = GetCurrentProcess();
  if (!SymInitialize(process, nullptr, TRUE)) {
    return 0;
  }

  void* stack[kPlatformMaxFrames];
  WORD frames =
      CaptureStackBackTrace(static_cast<DWORD>(first_frame),
                            static_cast<DWORD>(max_frames), stack, nullptr);

  constexpr int MAX_NAME_LEN = 256;
  alignas(SYMBOL_INFO) char symbol_buffer[sizeof(SYMBOL_INFO) + MAX_NAME_LEN];
  SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(symbol_buffer);
  std::memset(symbol, 0, sizeof(symbol_buffer));

  symbol->MaxNameLen = MAX_NAME_LEN;
  symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

  IMAGEHLP_LINE64 line;
  line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
  DWORD displacement;

  char address_buf[32];
  std::size_t count = 0;

  for (WORD i = 0; i < frames; i++) {
    DWORD64 address = reinterpret_cast<DWORD64>(stack[i]);

    std::snprintf(address_buf, sizeof(address_buf), "0x%016llx", address);
    const char* addr_str = address_buf;

    const char* function = "";
    const char* file = "";
    int line_number = 0;

    if (SymFromAddr(process, address, nullptr, symbol)) {
      function = symbol->Name;
    }

    if (SymGetLineFromAddr64(process, address, &displacement, &line)) {
      file = line.FileName;
      line_number = line.LineNumber;
    }

    StackTraceEntry entry;
    entry.index = i - first_frame;
    std::strncpy(entry.address.data(), addr_str, kAddressStrLength - 1);
    std::strncpy(entry.function.data(), function, kFunctionStrLength - 1);
    std::strncpy(entry.file.data(), file, kFileStrLength - 1);
    entry.line = line_number;
    entry.offset = 0;
    entry.use_index = use_index;
    out[count++] = entry;
  }

  SymCleanup(process);
  return count;
#endif
}

#if ENABLE_LLVM_UNWIND
std::string stack_trace_with_libunwind(unw_context_t* context,
                                       bool use_index,
                                       std::size_t first_frame,
                                       std::size_t max_frames) {
  StackTraceEntry entries[kPlatformMaxFrames];
  std::size_t count =
      collect_stack_trace(context, entries, use_index, first_frame, max_frames);
  std::string result;
  result.reserve(count * 256);
  stack_trace_entries_to_string(entries, count, &result);
  return result;
}

void stack_trace_with_libunwind_to_buffer(char* buffer,
                                          std::size_t buffer_size,
                                          unw_context_t* context,
                                          bool use_index,
                                          std::size_t first_frame,
                                          std::size_t max_frames) {
  StackTraceEntry entries[kPlatformMaxFrames];
  std::size_t count =
      collect_stack_trace(context, entries, use_index, first_frame, max_frames);
  stack_trace_entries_to_buffer(entries, count, buffer, buffer_size);
}
#endif

std::string stack_trace_from_current_context(bool use_index,
                                             std::size_t first_frame,
                                             std::size_t max_frames) {
#if ENABLE_LLVM_UNWIND
  unw_context_t context;
  if (unw_getcontext(&context) != 0) {
    return "";
  }
  return stack_trace_with_libunwind(&context, use_index, first_frame,
                                    max_frames);
#else
  StackTraceEntry entries[kPlatformMaxFrames];
  std::size_t count =
      collect_stack_trace(entries, use_index, first_frame, max_frames);
  std::string result;
  result.reserve(count * 256);
  stack_trace_entries_to_string(entries, count, &result);
  return result;
#endif
}

void stack_trace_from_current_context_to_buffer(char* buffer,
                                                std::size_t buffer_size,
                                                bool use_index,
                                                std::size_t first_frame,
                                                std::size_t max_frames) {
#if ENABLE_LLVM_UNWIND
  unw_context_t context;
  if (unw_getcontext(&context) != 0) {
    const char* err_msg = "Failed to get context\n";
    std::size_t err_len = std::strlen(err_msg);
    std::size_t copy_len = std::min(err_len, buffer_size - 1);
    std::strncpy(buffer, err_msg, copy_len);
    buffer[copy_len] = '\0';
    return;
  }
  stack_trace_with_libunwind_to_buffer(buffer, buffer_size, &context, use_index,
                                       first_frame, max_frames);
#else
  StackTraceEntry entries[kPlatformMaxFrames];
  std::size_t count =
      collect_stack_trace(entries, use_index, first_frame, max_frames);
  stack_trace_entries_to_buffer(entries, count, buffer, buffer_size);
#endif
}

}  // namespace core
