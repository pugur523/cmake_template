#include "app/prelaunch_handler.h"

#include "core/diagnostics/signal_handler.h"
#include "core/diagnostics/terminate_handler.h"

namespace app {

int on_prelaunch() {
  core::register_terminate_handler();
  core::register_signal_handlers();

  return 0;
}

}  // namespace app

