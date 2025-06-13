#include "app/init_handler.h"

#include "build/build_flag.h"

namespace app {

namespace {

int init_common() {
  return 0;
}

int init_on_release_build() {
#if IS_RELEASE
#endif  // IS_RELEASE

  return 0;
}

int init_on_debug_build() {
#if IS_DEBUG
#endif  // IS_DEBUG

  return 0;
}

}  // namespace

int all_initialize() {
  init_on_debug_build();
  init_on_release_build();
  init_common();

  return 0;
}

}  // namespace app
