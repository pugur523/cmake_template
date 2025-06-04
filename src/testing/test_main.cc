// Copyright 2025 pugur
// All rights reserved.

#include "gtest/gtest.h"

namespace {
void init_tests() {
  testing::InitGoogleTest();
}

int run_tests() {
  return RUN_ALL_TESTS();
}
}  // namespace

int main() {
  init_tests();
  return run_tests();
}
