#ifndef CORE_BASE_FILE_UTIL_H_
#define CORE_BASE_FILE_UTIL_H_

#include <sys/stat.h>

#include <cstddef>
#include <string>
#include <vector>

#include "build/build_flag.h"
#include "core/base/core_export.h"

namespace core {

constexpr const std::size_t kPathMaxLength = 4096;
constexpr const std::size_t kPredictedFilesNbPerDir = 64;

[[nodiscard]] CORE_EXPORT bool file_exists(const char* file_name);
[[nodiscard]] CORE_EXPORT bool dir_exists(const char* dir_name);
[[nodiscard]] CORE_EXPORT const std::string& get_exe_path();
[[nodiscard]] CORE_EXPORT const std::string& get_exe_dir();
[[nodiscard]] CORE_EXPORT const std::string& get_resources_dir();
[[nodiscard]] CORE_EXPORT std::vector<std::string> list_files(
    const std::string& path);
CORE_EXPORT int create_directory(const char* path);
CORE_EXPORT int create_directories(const std::string& path);
CORE_EXPORT int remove_file(const char* path);
CORE_EXPORT int remove_directory(const char* path);
CORE_EXPORT int rename_file(const char* old_path, const char* new_path);

CORE_EXPORT int write_binary_to_file(const void* binary_data,
                                     std::size_t binary_size,
                                     const std::string& output_path);

[[nodiscard]] CORE_EXPORT std::string file_extension(const std::string& path);

[[nodiscard]] CORE_EXPORT std::string sanitize_component(
    const std::string& part,
    bool is_first);

template <typename T>
inline int write_binary_to_file(const std::vector<T>& data,
                                const std::string& output_path) {
  return write_binary_to_file(static_cast<const void*>(data.data()),
                              data.size() * sizeof(T), output_path);
}

template <typename T>
[[nodiscard]] inline std::string to_string_path_part(const T& part) {
  if constexpr (std::is_convertible_v<T, std::string>) {
    return static_cast<std::string>(part);
  } else if constexpr (std::is_convertible_v<T, const char*>) {
    return std::string(part);
  } else {
    static_assert(std::is_same_v<T, void>,
                  "to_string_path_part: unsupported path element type");
  }
}

template <typename... Parts>
[[nodiscard]] std::string join_path(const Parts&... parts) {
  std::string joined;
  joined.reserve(kPathMaxLength);
  bool is_first = true;

  (
      [&] {
        std::string part = to_string_path_part(parts);
        std::string sanitized = sanitize_component(part, is_first);
        if (!sanitized.empty()) {
          if (!is_first) {
            joined += DIR_SEPARATOR;
          }
          joined += sanitized;
          is_first = false;
        }
      }(),
      ...);

  return joined;
}

}  // namespace core

#endif  // CORE_BASE_FILE_UTIL_H_
