#include "core/time/time_util.h"

#include <chrono>
#include <cstdint>
#include <ctime>
#include <string>

namespace core {

TimeUtil::TimeUtil(const std::string& format) : format_(format) {}

TimeUtil::~TimeUtil() = default;

const std::string& TimeUtil::format() const {
  return format_;
}

void TimeUtil::set_format(const std::string& format) {
  format_ = format;
}

std::time_t TimeUtil::current_time() const {
  auto now = std::chrono::system_clock::now();
  return std::chrono::system_clock::to_time_t(now);
}

std::tm TimeUtil::current_time_local() const {
  std::time_t now = current_time();
  std::tm local_tm{};

#if IS_WINDOWS
  localtime_s(&local_tm, &now);
#else
  localtime_r(&now, &local_tm);
#endif
  return local_tm;
}

std::string TimeUtil::format_time(const std::tm& time,
                                  const char* format) const {
  char buffer[128];
  if (std::strftime(buffer, sizeof(buffer), format, &time)) {
    return std::string(buffer);
  }
  return "";
}

int64_t TimeUtil::unix_time() const {
  return std::chrono::duration_cast<std::chrono::seconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

}  // namespace core
