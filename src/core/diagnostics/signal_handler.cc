#include "core/diagnostics/signal_handler.h"

#include <csignal>
#include <iostream>
#include <thread>

#include "build/build_flag.h"
#include "core/base/logger.h"
#include "core/diagnostics/stack_trace.h"
#include "core/time/time_util.h"

#if IS_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace core {

namespace {

inline int get_pid() {
#if IS_WINDOWS
  return static_cast<int>(GetCurrentProcessId());
#else
  return getpid();
#endif
}

}  // namespace

const char* signal_to_string(int signal_number) {
  switch (signal_number) {
    case SIGSEGV: return "SIGSEGV (Invalid access to storage)";
    case SIGABRT: return "SIGABRT (Abnormal termination)";
    case SIGFPE: return "SIGFPE (Floating point exception)";
    case SIGILL: return "SIGILL (Illegal instruction)";
    case SIGINT: return "SIGINT (Interactive attention signal)";
    case SIGTERM: return "SIGTERM (Termination request)";
    // case SIGBUS: return "SIGBUS (Bus error)";
    // case SIGKILL: return "SIGKILL (Kill signal)";
    // case SIGSTOP: return "SIGSTOP (Stop signal)";
    // case SIGALRM: return "SIGALRM (Alarm clock)";
    default: return "Unknown signal";
  }
}

// Example signal handling output:
//
// Aborted at Thu Jan  1 00:00:00 1970
// (1234567890 in unix time)
// SIGABRT (Aborted) received by PID 12345(TID 67890)
void signal_handler(int signal_number) {
  TimeUtil time_util;
  std::time_t now = time_util.unix_time();

  static thread_local uint32_t cached_tid = []() noexcept -> uint32_t {
    const auto tid = std::this_thread::get_id();
    const auto hash_val = std::hash<std::thread::id>{}(tid);

    // Ensure non-zero ID for better performance characteristics
    return static_cast<uint32_t>(hash_val) | 1;
  }();
  core::glog.fatal<
      "Aborted at {} \n"
      "({} in unix time)\n"
      "{} received by PID {} (TID {})\n{}">(
      std::ctime(&now), now, signal_to_string(signal_number), get_pid(),
      cached_tid, stack_trace_from_current_context());
  std::exit(EXIT_FAILURE);
}

void register_signal_handlers() {
  std::signal(SIGSEGV, signal_handler);
  std::signal(SIGABRT, signal_handler);
  std::signal(SIGFPE, signal_handler);
  std::signal(SIGILL, signal_handler);

#if IS_DEBUG
  std::signal(SIGINT, signal_handler);
#endif
}

}  // namespace core
