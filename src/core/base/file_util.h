#ifndef CORE_BASE_FILE_UTIL_H_
#define CORE_BASE_FILE_UTIL_H_

#include <sys/stat.h>

#include <cstddef>
#include <string>
#include <vector>

#include "core/base/core_export.h"
#include "core/check.h"

#if ENABLE_AVX2
#include <immintrin.h>
#endif  // ENABLE_AVX2

namespace core {

constexpr const std::size_t kPathMaxLength = 4096;
constexpr const std::size_t kPredictedFilesNbPerDir = 64;
using Files = std::vector<std::string>;

[[nodiscard]] CORE_EXPORT bool file_exists(const char* file_name);
[[nodiscard]] CORE_EXPORT bool dir_exists(const char* dir_name);
[[nodiscard]] CORE_EXPORT std::vector<std::byte> read_file_bin(
    const char* path);
[[nodiscard]] CORE_EXPORT std::string read_file(const char* path);
[[nodiscard]] CORE_EXPORT std::u8string read_file_utf8(const char8_t* path);
[[nodiscard]] CORE_EXPORT const std::string& exe_path();
[[nodiscard]] CORE_EXPORT const std::string& exe_dir();
[[nodiscard]] CORE_EXPORT const std::string& resources_dir();
[[nodiscard]] CORE_EXPORT bool is_executable_in_path(const char* path);
[[nodiscard]] CORE_EXPORT Files list_files(const std::string& path);
[[nodiscard]] CORE_EXPORT std::string parent_dir(const std::string& path);
[[nodiscard]] CORE_EXPORT std::string base_name(const std::string& path);
[[nodiscard]] CORE_EXPORT std::string temp_directory();
[[nodiscard]] CORE_EXPORT std::string temp_path(const std::string& prefix);
CORE_EXPORT bool compress(const char* src_path,
                          const char* dest_path,
                          bool remove_after_compress = true);

CORE_EXPORT int create_directory(const char* path);
CORE_EXPORT int create_directories(const char* path);
CORE_EXPORT int create_file(const char* path);
CORE_EXPORT int remove_file(const char* path);
CORE_EXPORT int remove_directory(const char* path);
CORE_EXPORT int rename_file(const char* old_path, const char* new_path);
CORE_EXPORT int write_file(const char* path, const std::string& content);
CORE_EXPORT int write_binary_to_file(const void* binary_data,
                                     std::size_t binary_size,
                                     const std::string& output_path);

[[nodiscard]] CORE_EXPORT std::string file_extension(const std::string& path);
[[nodiscard]] CORE_EXPORT std::string file_name_without_extension(
    const std::string& path);
[[nodiscard]] CORE_EXPORT std::string sanitize_component(const char* part,
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
        std::string sanitized = sanitize_component(part.c_str(), is_first);
        if (!sanitized.empty()) {
          if (!is_first) {
            joined.push_back(DIR_SEPARATOR);
          }
          joined.append(sanitized);
          is_first = false;
        }
      }(),
      ...);

  return joined;
}

[[nodiscard]] CORE_EXPORT std::vector<std::string> read_lines_default(
    std::string_view content);

#if ENABLE_AVX2
[[nodiscard]] CORE_EXPORT std::vector<std::string> read_lines_with_avx2(
    std::string_view content);
#endif

template <bool use_avx2_if_available = true>
[[nodiscard]] inline std::vector<std::string> read_lines(
    std::string_view content) {
#if ENABLE_AVX2
  if constexpr (use_avx2_if_available) {
    return read_lines_with_avx2(content);
  }
#endif
  return read_lines_default(content);
}

[[nodiscard]] CORE_EXPORT std::vector<std::size_t> index_newlines_default(
    std::string_view content);

#if ENABLE_AVX2
[[nodiscard]] CORE_EXPORT std::vector<std::size_t> index_newlines_with_avx2(
    std::string_view content);
#endif

template <bool use_avx2_if_available = true>
[[nodiscard]] inline std::vector<std::size_t> index_newlines(
    std::string_view content) {
#if ENABLE_AVX2
  if constexpr (use_avx2_if_available) {
    return index_newlines_with_avx2(content);
  }
#endif
  return index_newlines_default(content);
}

class CORE_EXPORT TempFile {
 public:
  explicit TempFile(const std::string& prefix = "tmp_",
                    const std::string& content = "");

  ~TempFile();

  inline constexpr const std::string& path() const { return path_; }
  inline constexpr bool valid() const { return valid_; }

 private:
  std::string path_;
  bool valid_ : 1 = true;
};

class CORE_EXPORT TempDir {
 public:
  explicit TempDir(const std::string& prefix = "tmp_dir_");

  ~TempDir();

  inline constexpr const std::string& path() const { return path_; }
  inline constexpr bool valid() const { return valid_; }

 private:
  std::string path_;
  bool valid_ : 1 = true;
};

class CORE_EXPORT File {
 public:
  File(std::string&& file_name, std::string&& source);
  explicit File(std::string&& file_name);

  ~File() = default;

  File(const File&) = delete;
  File& operator=(const File&) = delete;

  File(File&&) noexcept = default;
  File& operator=(File&&) noexcept = default;

  inline const std::string& file_name() const { return file_name_; }
  inline const std::string& source() const { return source_; }

  // 1 indexed
  inline std::string_view line(std::size_t line_no) const {
    DCHECK_GT(line_no, 0);
    DCHECK_LE(line_no, line_count());

    std::size_t line_start = (line_no == 1) ? 0 : line_ends_[line_no - 2] + 1;
    std::size_t line_end = line_ends_[line_no - 1];

    // crlf
    if (line_end > line_start && source_[line_end - 1] == '\r') {
      --line_end;
    }

    return std::string_view(&source_[line_start], line_end - line_start);
  }

  inline std::size_t line_count() const { return line_ends_.size(); }

 private:
  std::string file_name_;
  std::string source_;
  std::vector<std::size_t> line_ends_;
};

}  // namespace core

#endif  // CORE_BASE_FILE_UTIL_H_
