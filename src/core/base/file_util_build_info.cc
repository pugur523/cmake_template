#include <chrono>
#include <random>
#include <string>

#include "build/build_flag.h"
#include "build/project_config.h"
#include "core/base/file_util.h"

namespace core {

std::string temp_path(const std::string& prefix) {
  std::string dir = temp_directory();

  static constexpr char charset[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
  constexpr std::size_t length = 8;

  std::mt19937 rng(static_cast<unsigned>(
      std::chrono::steady_clock::now().time_since_epoch().count()));
  std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

  std::string suffix = PROJECT_NAME "_";
  for (std::size_t i = 0; i < length; ++i) {
    suffix += charset[dist(rng)];
  }

#if IS_WINDOWS
  return dir + prefix + suffix + ".tmp";
#else
  return dir + prefix + suffix;
#endif
}

}  // namespace core
