#include "app/app.h"

#include <iostream>

#include "app/init_handler.h"
#include "app/prelaunch_handler.h"

namespace app {

int start() {
  std::cout << "hello, world" << std::endl;

  on_prelaunch();
  all_initialize();

  return 0;
}

}  // namespace app
