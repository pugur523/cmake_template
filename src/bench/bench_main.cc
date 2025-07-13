#include "benchmark/benchmark.h"
#include "core/base/logger.h"
#include "core/diagnostics/signal_handler.h"
#include "core/diagnostics/stack_trace.h"
#include "core/diagnostics/terminate_handler.h"

[[clang::xray_always_instrument]]
int main(int argc, char** argv) {
  core::register_signal_handlers();
  core::register_terminate_handler();
  core::register_stack_trace_handler();
  core::register_glog();

  benchmark ::MaybeReenterWithoutASLR(argc, argv);
  char arg0_default[] = "benchmark";
  char* args_default = reinterpret_cast<char*>(arg0_default);
  if (!argv) {
    argc = 1;
    argv = &args_default;
  }
  benchmark::Initialize(&argc, argv);
  if (benchmark::ReportUnrecognizedArguments(argc, argv)) {
    return 1;
  }
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  return 0;
}
