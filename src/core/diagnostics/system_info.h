#ifndef CORE_DIAGNOSTICS_SYSTEM_INFO_H_
#define CORE_DIAGNOSTICS_SYSTEM_INFO_H_

#include <cstdint>
#include <ostream>
#include <string>

#include "core/base/core_export.h"

namespace core {

enum class Platform : uint8_t {
  kUnknown = 0,
  kWindows = 1,
  kMac = 2,
  kLinux = 3,
  kAndroid = 4,

  // Keep this at the end and equal to the last entry.
  kMaxValue = kAndroid,
};

class CORE_EXPORT SystemInfo {
 public:
  SystemInfo();
  ~SystemInfo();

  SystemInfo(const SystemInfo&) = delete;
  SystemInfo& operator=(const SystemInfo&) = delete;

  Platform platform() const;
  const std::string& os() const;
  const std::string& cpu_arch() const;
  uint64_t total_ram_raw() const;
  std::string total_ram() const;
  uint64_t ram_usage_raw() const;
  std::string ram_usage() const;
  std::string to_string() const;

 private:
  bool init();
  bool init_platform();

  std::string os_str_;
  std::string cpu_arch_;
  uint64_t total_ram_;
  Platform platform_ = Platform::kUnknown;

  static constexpr std::size_t kSystemInfoStringPredictedSize = 256;
};

CORE_EXPORT std::ostream& operator<<(std::ostream& os,
                                     const SystemInfo& sys_info);
[[nodiscard]] CORE_EXPORT std::string format_bytes(
    const uint64_t bytes,
    const std::size_t precision = 2);

}  // namespace core

#endif  // CORE_DIAGNOSTICS_SYSTEM_INFO_H_
