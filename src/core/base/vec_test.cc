#include "core/base/vec.h"

#include "gtest/gtest.h"

namespace core {

TEST(VecTest, DefaultConstructor) {
  constexpr Vec2<int> v;
  EXPECT_EQ(v[0], 0);
  EXPECT_EQ(v[1], 0);
}

TEST(VecTest, InitializerListConstructor) {
  constexpr Vec3<float> v{1.0f, 2.0f, 3.0f};
  EXPECT_FLOAT_EQ(v[0], 1.0f);
  EXPECT_FLOAT_EQ(v[1], 2.0f);
  EXPECT_FLOAT_EQ(v[2], 3.0f);
}

TEST(VecTest, ArithmeticOperators) {
  Vec2<int> a{1, 2};
  Vec2<int> b{3, 4};

  auto c = a + b;
  EXPECT_EQ(c, (Vec2<int>{4, 6}));

  auto d = b - a;
  EXPECT_EQ(d, (Vec2<int>{2, 2}));

  auto e = a * 3;
  EXPECT_EQ(e, (Vec2<int>{3, 6}));

  auto f = b / 2;
  EXPECT_EQ(f, (Vec2<int>{1, 2}));
}

TEST(VecTest, DotAndLength) {
  Vec3<double> a{1.0, 2.0, 3.0};
  Vec3<double> b{4.0, -5.0, 6.0};

  double dot = a.dot(b);  // 1*4 + 2*-5 + 3*6 = 4 -10 + 18 = 12
  EXPECT_DOUBLE_EQ(dot, 12.0);

  double length = a.length();  // sqrt(1^2 + 2^2 + 3^2) = sqrt(14)
  EXPECT_NEAR(length, std::sqrt(14.0), 1e-9);
}

TEST(VecTest, Normalized) {
  Vec2<double> a{3.0, 4.0};
  auto n = a.normalized();
  EXPECT_NEAR(n[0], 0.6, 1e-6);
  EXPECT_NEAR(n[1], 0.8, 1e-6);
}

TEST(VecTest, CrossProduct3D) {
  Vec3<int> a{1, 0, 0};
  Vec3<int> b{0, 1, 0};
  auto c = a.cross(b);
  EXPECT_EQ(c, (Vec3<int>{0, 0, 1}));
}

TEST(VecTest, Clamp) {
  Vec3<int> a{5, 10, 15};
  Vec3<int> min{6, 9, 0};
  Vec3<int> max{8, 10, 14};
  auto clamped = a.clamp(min, max);
  EXPECT_EQ(clamped, (Vec3<int>{6, 10, 14}));
}

TEST(VecTest, MinMaxElement) {
  constexpr Vec3<int> v{8, 2, 5};
  EXPECT_EQ(v.min_element(), 2);
  EXPECT_EQ(v.max_element(), 8);
}

TEST(VecTest, ComparisonOperators) {
  Vec4<float> a{1.0f, 2.0f, 3.0f, 4.0f};
  Vec4<float> b{1.0f, 2.0f, 3.0f, 4.0f};
  Vec4<float> c{4.0f, 3.0f, 2.0f, 1.0f};

  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a == c);
  EXPECT_TRUE(a != c);
}

}  // namespace core
