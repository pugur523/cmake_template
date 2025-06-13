#include "gtest/gtest.h"

TEST(sample, add) {
  EXPECT_EQ(1 + 2, 3);
  EXPECT_NE(1 + 5, 7);
}

TEST(sample, subtract) {
  EXPECT_EQ(3 - 1, 2);
  EXPECT_NE(250 - 250, 250);
}
