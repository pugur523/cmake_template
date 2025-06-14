#include "core/diagnostics/signal_handler.h"
#include "core/diagnostics/stack_trace.h"
#include "core/diagnostics/terminate_handler.h"
#include "gtest/gtest.h"

namespace {

[[clang::xray_always_instrument]]
void init_tests() {
  testing::InitGoogleTest();
}

[[clang::xray_always_instrument]]
int run_tests() {
  return RUN_ALL_TESTS();
}

}  // namespace

[[clang::xray_always_instrument]]
int main() {
  core::register_stack_trace_handler();
  core::register_terminate_handler();
  core::register_signal_handlers();

  init_tests();
  return run_tests();
}
