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
    EXPECT_NE(location.stack_trace(), nullptr);
    return;
  }
  verify_loop();
}

void verify_transfer(Location&& location) {
  EXPECT_NE(location.stack_trace(), nullptr);
}

}  // namespace

TEST(LocationTest, BasicStackTrace) {
  Location location = FROM_HERE;

  std::ostringstream oss;
  EXPECT_EQ(&oss, &(oss << location));
  EXPECT_NE(location.stack_trace(), nullptr);
}

TEST(LocationTest, LoopStackTrace) {
  verify_loop();
}

TEST(LocationTest, TransferStackTrace) {
  verify_transfer(FROM_HERE);
}

}  // namespace core
