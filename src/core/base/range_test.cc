#include "core/base/range.h"

#include "gtest/gtest.h"

namespace core {

TEST(Range1DTest, BasicConstructionAndAccess) {
  Range1D<int> r(1, 5);
  EXPECT_EQ(r.min(), 1);
  EXPECT_EQ(r.max(), 5);
  EXPECT_TRUE(r.contains(3));
  EXPECT_FALSE(r.contains(0));
  EXPECT_EQ(r.length(), 4);
}

TEST(Range1DTest, Setters) {
  Range1D<float> r(0.0f, 10.0f);
  r.set_min(2.0f);
  EXPECT_FLOAT_EQ(r.min(), 2.0f);
  r.set_max(12.0f);
  EXPECT_FLOAT_EQ(r.max(), 12.0f);

  r.set(5.0f, 8.0f);
  EXPECT_FLOAT_EQ(r.min(), 5.0f);
  EXPECT_FLOAT_EQ(r.max(), 8.0f);
}

TEST(RangeTest, ConstructionAndMinMax) {
  std::array<Range1D<int>, 2> dims = {Range1D<int>(0, 10), Range1D<int>(5, 15)};
  Range<int, 2> r(dims);

  EXPECT_EQ(r.min(0), 0);
  EXPECT_EQ(r.max(1), 15);
  EXPECT_EQ(r.min(), (std::array<int, 2>{0, 5}));
  EXPECT_EQ(r.max(), (std::array<int, 2>{10, 15}));
}

TEST(RangeTest, ContainsPoint) {
  Range<int, 2> r({Range1D<int>(0, 5), Range1D<int>(10, 20)});
  EXPECT_TRUE(r.contains({3, 15}));
  EXPECT_FALSE(r.contains({6, 15}));
  EXPECT_FALSE(r.contains({3, 21}));
}

TEST(RangeTest, ContainsRange) {
  Range<int, 2> outer({Range1D<int>(0, 10), Range1D<int>(10, 20)});
  Range<int, 2> inner({Range1D<int>(2, 8), Range1D<int>(11, 19)});
  Range<int, 2> overlap({Range1D<int>(5, 15), Range1D<int>(11, 19)});
  EXPECT_TRUE(outer.contains(inner));
  EXPECT_FALSE(outer.contains(overlap));
}

TEST(RangeTest, Intersects) {
  Range<int, 2> a({Range1D<int>(0, 5), Range1D<int>(0, 5)});
  Range<int, 2> b({Range1D<int>(4, 6), Range1D<int>(4, 6)});
  Range<int, 2> c({Range1D<int>(6, 8), Range1D<int>(6, 8)});
  EXPECT_TRUE(a.intersects(b));
  EXPECT_FALSE(a.intersects(c));
}

TEST(RangeTest, SetFunctions) {
  Range<int, 3> r({Range1D<int>(1, 5), Range1D<int>(2, 6), Range1D<int>(3, 7)});
  r.set(1, 10, 20);
  EXPECT_EQ(r.min(1), 10);
  EXPECT_EQ(r.max(1), 20);

  r.set_min(0, 0);
  r.set_max(2, 10);
  EXPECT_EQ(r.min(0), 0);
  EXPECT_EQ(r.max(2), 10);
}

TEST(RangeTest, Volume) {
  Range<int, 2> r({Range1D<int>(1, 4), Range1D<int>(2, 6)});  // (3 x 4) = 12
  EXPECT_EQ(r.volume(), 12);
}

TEST(RangeTest, EqualityOperator) {
  Range<int, 2> a({Range1D<int>(1, 2), Range1D<int>(3, 4)});
  Range<int, 2> b({Range1D<int>(1, 2), Range1D<int>(3, 4)});
  Range<int, 2> c({Range1D<int>(0, 2), Range1D<int>(3, 4)});
  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

TEST(RangeTest, StreamOutput) {
  Range<int, 2> r({Range1D<int>(1, 3), Range1D<int>(4, 6)});
  std::stringstream ss;
  ss << r;
  EXPECT_EQ(ss.str(), "[[1, 3], [4, 6]]");
}

}  // namespace core
