#include "benchmark/benchmark.h"

[[clang::xray_always_instrument]] BENCHMARK_MAIN();
