#ifndef CORE_BASE_RANGE_H_
#define CORE_BASE_RANGE_H_

#include <algorithm>
#include <array>
#include <utility>

#include "core/check.h"

namespace core {

template <typename T>
class Range1D {
  static_assert(std::is_arithmetic_v<T>,
                "Range1D requires an arithmetic type (e.g., int, float)");

 public:
  explicit Range1D(const T& min, const T& max) : min_(min), max_(max) {
    DCHECK_LE(min_, max_) << "invalid range: min is greater than max.";
  }

  ~Range1D() = default;

  inline const T& min() const { return min_; }
  inline const T& max() const { return max_; }

  inline bool contains(const T& x) const { return min_ <= x && x <= max_; }

  inline T length() const { return max_ - min_; }

  inline void set(const T& new_min, const T& new_max) {
    DCHECK_LE(new_min, new_max) << "invalid range: min is greater than max.";
    min_ = new_min;
    max_ = new_max;
  }

  inline void set_min(const T& new_min) {
    DCHECK_LE(new_min, max_);
    min_ = new_min;
  }

  inline void set_max(const T& new_max) {
    DCHECK_GE(new_max, min_);
    max_ = new_max;
  }

  inline bool operator==(const Range1D& other) const {
    return min_ == other.min_ && max_ == other.max_;
  }

  inline bool operator!=(const Range1D& other) const {
    return !(*this == other);
  }

 private:
  T min_;
  T max_;
};

template <typename T, std::size_t dim_number>
class Range {
  static_assert(std::is_arithmetic_v<T>,
                "Range requires an arithmetic type (e.g., int, float)");

 public:
  explicit Range(const std::array<Range1D<T>, dim_number>& ranges)
      : ranges_(ranges) {}

  ~Range() = default;

  Range(const Range&) = delete;
  Range& operator=(const Range&) = delete;

  Range(Range&&) noexcept = default;
  Range& operator=(Range&&) noexcept = default;

  inline const T& min(std::size_t index) const {
    DCHECK_LT(index, dim_number);
    return ranges_[index].min();
  }

  inline const T& max(std::size_t index) const {
    DCHECK_LT(index, dim_number);
    return ranges_[index].max();
  }

  std::array<T, dim_number> min() const {
    std::array<T, dim_number> result;
    for (std::size_t i = 0; i < dim_number; ++i) {
      result[i] = ranges_[i].min();
    }
    return result;
  }

  std::array<T, dim_number> max() const {
    std::array<T, dim_number> result;
    for (std::size_t i = 0; i < dim_number; ++i) {
      result[i] = ranges_[i].max();
    }
    return result;
  }

  T volume() const {
    T result = 1;
    for (const auto& r : ranges_) {
      result *= r.length();
    }
    return result;
  }

  bool contains(const std::array<T, dim_number>& x) const {
    for (std::size_t i = 0; i < dim_number; ++i) {
      if (!ranges_[i].contains(x[i])) {
        return false;
      }
    }
    return true;
  }

  bool contains(const Range& other) const {
    for (std::size_t i = 0; i < dim_number; ++i) {
      if (other.ranges_[i].min() < ranges_[i].min() ||
          other.ranges_[i].max() > ranges_[i].max()) {
        return false;
      }
    }
    return true;
  }

  bool intersects(const Range& other) const {
    for (std::size_t i = 0; i < dim_number; ++i) {
      if (ranges_[i].max() < other.ranges_[i].min() ||
          ranges_[i].min() > other.ranges_[i].max()) {
        return false;
      }
    }
    return true;
  }

  inline void set(std::size_t index, const T& new_min, const T& new_max) {
    DCHECK_LT(index, dim_number);
    ranges_[index].set(new_min, new_max);
  }

  inline void set_min(std::size_t i, const T& val) {
    DCHECK_LT(i, dim_number);
    ranges_[i].set_min(val);
  }

  inline void set_max(std::size_t i, const T& val) {
    DCHECK_LT(i, dim_number);
    ranges_[i].set_max(val);
  }

  inline bool operator==(const Range& other) const {
    return ranges_ == other.ranges_;
  }

  inline bool operator!=(const Range& other) const { return !(*this == other); }

  friend std::ostream& operator<<(std::ostream& os, const Range& range) {
    os << "[";
    for (std::size_t i = 0; i < dim_number; ++i) {
      if (i > 0) {
        os << ", ";
      }
      os << "[" << range.ranges_[i].min() << ", " << range.ranges_[i].max()
         << "]";
    }
    os << "]";
    return os;
  }

 private:
  std::array<Range1D<T>, dim_number> ranges_;
};

}  // namespace core

#endif  // CORE_BASE_RANGE_H_
