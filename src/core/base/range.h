#ifndef CORE_BASE_RANGE_H_
#define CORE_BASE_RANGE_H_

#include <array>

#include "core/check.h"

namespace core {

template <typename T>
class Range1D {
  static_assert(std::is_arithmetic_v<T>,
                "Range1D requires an arithmetic type (e.g., int, float)");

 public:
  constexpr explicit Range1D(const T& min, const T& max)
      : min_(min), max_(max) {
    DCHECK_LE(min_, max_) << "invalid range: min is greater than max.";
  }

  ~Range1D() = default;

  inline constexpr const T& min() const { return min_; }
  inline constexpr const T& max() const { return max_; }

  inline constexpr bool contains(const T& x) const {
    return min_ <= x && x <= max_;
  }

  inline constexpr T length() const { return max_ - min_; }

  inline constexpr void set(const T& new_min, const T& new_max) {
    DCHECK_LE(new_min, new_max) << "invalid range: min is greater than max.";
    min_ = new_min;
    max_ = new_max;
  }

  inline constexpr void set_min(const T& new_min) {
    DCHECK_LE(new_min, max_);
    min_ = new_min;
  }

  inline constexpr void set_max(const T& new_max) {
    DCHECK_GE(new_max, min_);
    max_ = new_max;
  }

  inline constexpr bool operator==(const Range1D& other) const {
    return min_ == other.min_ && max_ == other.max_;
  }

  inline constexpr bool operator!=(const Range1D& other) const {
    return !(*this == other);
  }

 private:
  T min_;
  T max_;
};

template <typename T, std::size_t kDimNumber>
class Range {
  static_assert(std::is_arithmetic_v<T>,
                "Range requires an arithmetic type (e.g., int, float)");

 public:
  inline constexpr explicit Range(
      const std::array<Range1D<T>, kDimNumber>& ranges)
      : ranges_(ranges) {}

  inline constexpr ~Range() = default;

  inline constexpr Range(const Range&) = delete;
  inline constexpr Range& operator=(const Range&) = delete;

  inline constexpr Range(Range&&) noexcept = default;
  inline constexpr Range& operator=(Range&&) noexcept = default;

  inline constexpr const T& min(std::size_t index) const {
    DCHECK_LT(index, kDimNumber);
    return ranges_[index].min();
  }

  inline constexpr const T& max(std::size_t index) const {  // NOLINT
    DCHECK_LT(index, kDimNumber);
    return ranges_[index].max();
  }

  constexpr std::array<T, kDimNumber> min() const {
    std::array<T, kDimNumber> result;
    for (std::size_t i = 0; i < kDimNumber; ++i) {
      result[i] = ranges_[i].min();
    }
    return result;
  }

  constexpr std::array<T, kDimNumber> max() const {
    std::array<T, kDimNumber> result;
    for (std::size_t i = 0; i < kDimNumber; ++i) {
      result[i] = ranges_[i].max();
    }
    return result;
  }

  constexpr T volume() const {
    T result = 1;
    for (const auto& r : ranges_) {
      result *= r.length();
    }
    return result;
  }

  constexpr bool contains(const std::array<T, kDimNumber>& x) const {
    for (std::size_t i = 0; i < kDimNumber; ++i) {
      if (!ranges_[i].contains(x[i])) {
        return false;
      }
    }
    return true;
  }

  constexpr bool contains(const Range& other) const {
    for (std::size_t i = 0; i < kDimNumber; ++i) {
      if (other.ranges_[i].min() < ranges_[i].min() ||
          other.ranges_[i].max() > ranges_[i].max()) {
        return false;
      }
    }
    return true;
  }

  constexpr bool intersects(const Range& other) const {
    for (std::size_t i = 0; i < kDimNumber; ++i) {
      if (ranges_[i].max() < other.ranges_[i].min() ||
          ranges_[i].min() > other.ranges_[i].max()) {
        return false;
      }
    }
    return true;
  }

  inline constexpr void set(std::size_t index,
                            const T& new_min,
                            const T& new_max) {
    DCHECK_LT(index, kDimNumber);
    ranges_[index].set(new_min, new_max);
  }

  inline constexpr void set_min(std::size_t i, const T& val) {
    DCHECK_LT(i, kDimNumber);
    ranges_[i].set_min(val);
  }

  inline constexpr void set_max(std::size_t i, const T& val) {
    DCHECK_LT(i, kDimNumber);
    ranges_[i].set_max(val);
  }

  inline constexpr bool operator==(const Range& other) const {
    return ranges_ == other.ranges_;
  }

  inline constexpr bool operator!=(const Range& other) const {
    return !(*this == other);
  }

  friend std::ostream& operator<<(std::ostream& os, const Range& range) {
    os << "[";
    for (std::size_t i = 0; i < kDimNumber; ++i) {
      if (i > 0) {
        os << ", ";
      }
      os << "[" << range.ranges_[i].min() << ", " << range.ranges_[i].max()
         << "]";
    }
    return os << "]";
  }

 private:
  std::array<Range1D<T>, kDimNumber> ranges_;
};

}  // namespace core

#endif  // CORE_BASE_RANGE_H_
