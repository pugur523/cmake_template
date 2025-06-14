#include "core/diagnostics/terminate_handler.h"

#include <iostream>

#include "core/diagnostics/stack_trace.h"

namespace core {

void terminate_handler() {
  std::cout << "\nProgram terminated unexpectedly\n"
            << "Stack trace (most recent call last):\n"
            << stack_trace_from_current_context() << std::endl;

  std::exit(EXIT_FAILURE);
}

void register_terminate_handler() {
  std::set_terminate(terminate_handler);
}

}  // namespace core
