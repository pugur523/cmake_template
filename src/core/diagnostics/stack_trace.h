#ifndef CORE_DIAGNOSTICS_STACK_TRACE_H_
#define CORE_DIAGNOSTICS_STACK_TRACE_H_

#include <string>

#include "core/base/core_export.h"

#if ENABLE_LLVM_UNWIND
struct unw_context_t;
#endif

namespace core {

struct StackTraceEntry;

static constexpr std::size_t kDefaultFirstFrame = 1;

#if IS_WINDOWS
static constexpr std::size_t kPlatformMaxFrames = 62;
#elif IS_MAC
static constexpr std::size_t kPlatformMaxFrames = 128;
#elif IS_LINUX
static constexpr std::size_t kPlatformMaxFrames = 64;
#else
static constexpr std::size_t kPlatformMaxFrames = 64;
#endif

CORE_EXPORT void stack_trace_entries_to_string(
    const StackTraceEntry entries[kPlatformMaxFrames],
    std::size_t count,
    std::string* out);

CORE_EXPORT std::size_t collect_stack_trace(
#if ENABLE_LLVM_UNWIND
    unw_context_t* context,
#endif
    StackTraceEntry out[kPlatformMaxFrames],
    bool use_index,
    std::size_t first_frame,
    std::size_t max_frames);

// Example stack trace output:
//
// @0     0x123456789abc   bar        at /path/to/source/file:42
// @1     0x23456789abcd   foo        at /path/to/source/file:57
// @2     0x3456789abcde   hoge       at /path/to/source/file:91
// @3     0x456789abcdef   fuga       at /path/to/source/file:108

#if ENABLE_LLVM_UNWIND
CORE_EXPORT std::string stack_trace_with_libunwind(
    unw_context_t* context,
    bool use_index = true,
    std::size_t first_frame = kDefaultFirstFrame,
    std::size_t max_frames = kPlatformMaxFrames);

CORE_EXPORT void stack_trace_with_libunwind_to_buffer(
    char* buffer,
    std::size_t buffer_size,
    unw_context_t* context,
    bool use_index = true,
    std::size_t first_frame = kDefaultFirstFrame,
    std::size_t max_frames = kPlatformMaxFrames);
#endif

CORE_EXPORT std::string stack_trace_from_current_context(
    bool use_index = true,
    std::size_t first_frame = kDefaultFirstFrame,
    std::size_t max_frames = kPlatformMaxFrames);

CORE_EXPORT void stack_trace_from_current_context_to_buffer(
    char* buffer,
    std::size_t buffer_size,
    bool use_index = true,
    std::size_t first_frame = kDefaultFirstFrame,
    std::size_t max_frames = kPlatformMaxFrames);

}  // namespace core

#endif  // CORE_DIAGNOSTICS_STACK_TRACE_H_
