#ifndef CORE_BASE_STRING_UTIL_H_
#define CORE_BASE_STRING_UTIL_H_

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iterator>
#include <queue>
#include <string>
#include <utility>

#include "core/base/core_export.h"

namespace core {

[[nodiscard]] CORE_EXPORT std::string encode_escape(const std::string& input);
[[nodiscard]] CORE_EXPORT std::string decode_escape(const std::string& input);

CORE_EXPORT void to_lower(char* input);
CORE_EXPORT void to_lower(std::string* input);
[[nodiscard]] CORE_EXPORT std::string to_lower(const std::string& input);

CORE_EXPORT void to_upper(char* input);
CORE_EXPORT void to_upper(std::string* input);
[[nodiscard]] CORE_EXPORT std::string to_upper(const std::string& input);

[[nodiscard]] CORE_EXPORT std::size_t utf8_char_length(unsigned char lead);
[[nodiscard]] CORE_EXPORT std::string utf8_truncate(const std::string& input,
                                                    std::size_t max_chars);
[[nodiscard]] CORE_EXPORT std::queue<std::string> split_string(
    const std::string& input,
    const std::string& delimiter);
[[nodiscard]] CORE_EXPORT std::string remove_bracket(const std::string& input);
[[nodiscard]] CORE_EXPORT std::size_t safe_strlen(const char* str);

CORE_EXPORT void padding(char*& cursor,
                         const char* const end,
                         std::size_t current_len,
                         std::size_t align_len);

CORE_EXPORT std::size_t write_raw(char*& dest,
                                  const char* source,
                                  std::size_t len);

template <typename... Args>
std::size_t write_format(char*& cursor,
                         const char* const end,
                         const char* fmt,
                         Args... args) {
  std::ptrdiff_t remaining = end - cursor;
  if (remaining <= 0) {
    return 0;
  }

  int result = std::snprintf(cursor, static_cast<std::size_t>(remaining), fmt,
                             std::forward<Args>(args)...);

  if (result < 0) {
    return 0;
  }

  std::size_t written = static_cast<std::size_t>(
      std::min(result, static_cast<int>(remaining) - 1));

  cursor += written;
  return written;
}

inline bool starts_with(const std::string& input,
                        const std::string& prefix,
                        std::size_t index = 0) {
  return input.length() >= prefix.length() &&
         input.compare(index, prefix.length(), prefix) == 0;
}

constexpr const char* kReset = "\033[0m";
constexpr const char* kBold = "\033[1m";
constexpr const char* kRed = "\033[31m";
constexpr const char* kYellow = "\033[33m";
constexpr const char* kGreen = "\033[32m";
constexpr const char* kCyan = "\033[36m";
constexpr const char* kMagenta = "\033[35m";
constexpr const char* kBright_red = "\033[91m";
constexpr const char* kBright_green = "\033[92m";
constexpr const char* kBright_cyan = "\033[96m";

}  // namespace core

#endif  // CORE_BASE_STRING_UTIL_H_
