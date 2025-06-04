// Copyright 2025 pugur
// All rights reserved.

#include <iostream>

#include "app/app.h"

#include "app/init_handler.h"
#include "app/prelaunch_handler.h"

#include "core/base/core.h"

namespace app {

[[clang::xray_always_instrument]]
int start() {
  std::cout << "hello, world" << std::endl;

  on_prelaunch();
  all_initialize();

  int result = core::sample(2, 3);
  std::cout << result << std::endl;

  return 0;
}

}  // namespace app