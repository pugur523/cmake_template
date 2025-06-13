#ifndef CORE_TIME_TIME_UTIL_H_
#define CORE_TIME_TIME_UTIL_H_

#include <cstdint>
#include <ctime>
#include <string>

#include "core/base/core_export.h"

namespace core {

class CORE_EXPORT TimeUtil {
 public:
  explicit TimeUtil(const std::string& format = "%Y-%m-%d %H:%M:%S");
  ~TimeUtil();

  TimeUtil(const TimeUtil&) = delete;
  TimeUtil& operator=(const TimeUtil&) = delete;

  const std::string& format() const;

  void set_format(const std::string& format);

  std::time_t current_time() const;
  std::tm current_time_local() const;
  std::string format_time(const std::tm& time, const char* format) const;
  inline std::string format_time(const std::tm& time) const {
    return format_time(time, format_.c_str());
  }

  int64_t unix_time() const;

 private:
  std::string format_;
};

}  // namespace core

#endif  // CORE_TIME_TIME_UTIL_H_
