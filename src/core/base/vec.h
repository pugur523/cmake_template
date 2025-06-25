#ifndef CORE_BASE_VEC_H_
#define CORE_BASE_VEC_H_

#include <algorithm>
#include <array>
#include <cmath>
#include <initializer_list>
#include <iostream>
#include <type_traits>

namespace core {

template <typename T, std::size_t kDimNumber>
class Vec {
 public:
  static_assert(std::is_arithmetic_v<T>, "Vec only supports arithmetic types.");

  // Constructors
  constexpr Vec() : data_() {}

  constexpr Vec(std::initializer_list<T> list) : data_{} {
    std::size_t i = 0;
    for (T v : list) {
      if (i < kDimNumber) {
        data_[i++] = v;
      }
    }
  }

  inline constexpr Vec(const Vec&) = default;
  inline constexpr Vec& operator=(const Vec&) = default;

  inline constexpr Vec(Vec&&) noexcept = default;
  inline constexpr Vec& operator=(Vec&&) noexcept = default;

  static inline constexpr Vec from_array(const std::array<T, kDimNumber>& arr) {
    Vec v;
    v.data_ = arr;
    return v;
  }

  inline constexpr std::array<T, kDimNumber> to_array() const { return data_; }

  // Element access
  inline constexpr T& operator[](std::size_t i) { return data_[i]; }
  inline constexpr const T& operator[](std::size_t i) const { return data_[i]; }

  // Arithmetic
  constexpr Vec operator+(const Vec& other) const {
    Vec result;
    for (std::size_t i = 0; i < kDimNumber; ++i) {
      result[i] = data_[i] + other[i];
    }
    return result;
  }

  constexpr Vec operator-(const Vec& other) const {
    Vec result;
    for (std::size_t i = 0; i < kDimNumber; ++i) {
      result[i] = data_[i] - other[i];
    }
    return result;
  }

  constexpr Vec operator*(T scalar) const {
    Vec result;
    for (std::size_t i = 0; i < kDimNumber; ++i) {
      result[i] = data_[i] * scalar;
    }
    return result;
  }

  constexpr Vec operator/(T scalar) const {
    Vec result;
    for (std::size_t i = 0; i < kDimNumber; ++i) {
      result[i] = data_[i] / scalar;
    }
    return result;
  }

  constexpr Vec& operator+=(const Vec& other) {
    for (std::size_t i = 0; i < kDimNumber; ++i) {
      data_[i] += other[i];
    }
    return *this;
  }

  constexpr Vec& operator-=(const Vec& other) {
    for (std::size_t i = 0; i < kDimNumber; ++i) {
      data_[i] -= other[i];
    }
    return *this;
  }

  constexpr Vec& operator*=(T scalar) {
    for (std::size_t i = 0; i < kDimNumber; ++i) {
      data_[i] *= scalar;
    }
    return *this;
  }

  constexpr Vec& operator/=(T scalar) {
    for (std::size_t i = 0; i < kDimNumber; ++i) {
      data_[i] /= scalar;
    }
    return *this;
  }

  // Dot product
  constexpr T dot(const Vec& other) const {
    T result = T{};
    for (std::size_t i = 0; i < kDimNumber; ++i) {
      result += data_[i] * other[i];
    }
    return result;
  }

  inline constexpr T length() const { return std::sqrt(dot(*this)); }

  inline constexpr Vec normalized() const {
    T len = length();
    return len == T{} ? *this : *this / len;
  }

  inline constexpr T distance(const Vec& other) const {
    return (*this - other).length();
  }

  constexpr Vec clamp(const Vec& min, const Vec& max) const {
    Vec result;
    for (std::size_t i = 0; i < kDimNumber; ++i) {
      result[i] = std::max(min[i], std::min(max[i], data_[i]));
    }
    return result;
  }

  // Min/Max element
  constexpr T min_element() const {
    return *std::min_element(data_.begin(), data_.end());
  }

  constexpr T max_element() const {
    return *std::max_element(data_.begin(), data_.end());
  }

  // Cross product (3D only)
  template <std::size_t D = kDimNumber>
  inline constexpr typename std::enable_if_t<D == 3, Vec> cross(
      const Vec& other) const {
    return Vec{data_[1] * other[2] - data_[2] * other[1],
               data_[2] * other[0] - data_[0] * other[2],
               data_[0] * other[1] - data_[1] * other[0]};
  }

  // Comparison
  inline constexpr bool operator==(const Vec& other) const {
    return data_ == other.data_;
  }

  inline constexpr bool operator!=(const Vec& other) const {
    return !(*this == other);
  }

  // Output
  friend std::ostream& operator<<(std::ostream& os, const Vec& v) {
    os << "[";
    for (std::size_t i = 0; i < kDimNumber; ++i) {
      if (i > 0) {
        os << ", ";
      }
      os << v[i];
    }
    os << "]";
    return os;
  }

 private:
  std::array<T, kDimNumber> data_;
};

// Type aliases
template <typename T>
using Vec2 = Vec<T, 2>;
template <typename T>
using Vec3 = Vec<T, 3>;
template <typename T>
using Vec4 = Vec<T, 4>;

}  // namespace core

#endif  // CORE_BASE_VEC_H_
