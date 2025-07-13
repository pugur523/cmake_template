#include "core/diagnostics/terminate_handler.h"

#include <iostream>

#include "core/base/logger.h"
#include "core/diagnostics/stack_trace.h"

namespace core {

void terminate_handler() {
  core::glog.fatal<
      "\nProgram terminated unexpectedly\n"
      "Stack trace (most recent call last):\n{}\n">(
      stack_trace_from_current_context());
  std::exit(EXIT_FAILURE);
}

void register_terminate_handler() {
  std::set_terminate(terminate_handler);
}

}  // namespace core
