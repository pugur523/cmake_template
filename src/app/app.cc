#include "app/app.h"

#include "app/cli_handler.h"
#include "app/init_handler.h"
#include "app/preinit_handler.h"

namespace app {

int start(int argc, char** argv) {
  on_preinit();
  all_initialize();

  handle_arguments(argc, argv);

  return 0;
}

}  // namespace app
