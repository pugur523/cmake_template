#include "app/preinit_handler.h"

#include "core/base/logger.h"
#include "core/diagnostics/signal_handler.h"
#include "core/diagnostics/stack_trace.h"
#include "core/diagnostics/terminate_handler.h"

namespace app {

int on_preinit() {
  core::register_stack_trace_handler();
  core::register_terminate_handler();
  core::register_signal_handlers();
  core::register_glog();

  return 0;
}

}  // namespace app
