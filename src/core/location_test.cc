#include "core/location.h"

#include <sstream>

#include "gtest/gtest.h"

namespace core {

namespace {

void verify_loop() {
  static int cnt = 0;

  if (cnt++ >= 15) {
    Location location = FROM_HERE;

    std::ostringstream oss;
    EXPECT_EQ(&oss, &(oss << location));
    EXPECT_FALSE(location.stack_trace().empty());
    return;
  }
  verify_loop();
}

void verify_transfer(Location&& location) {
  EXPECT_FALSE(location.stack_trace().empty());
}

}  // namespace

TEST(location, basic_stack_trace) {
  Location location = FROM_HERE;

  std::ostringstream oss;
  EXPECT_EQ(&oss, &(oss << location));
  EXPECT_FALSE(location.stack_trace().empty());
}

TEST(location, loop_stack_trace) {
  verify_loop();
}

TEST(location, transfer_stack_trace) {
  verify_transfer(FROM_HERE);
}

}  // namespace core
