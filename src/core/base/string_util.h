#ifndef CORE_BASE_STRING_UTIL_H_
#define CORE_BASE_STRING_UTIL_H_

#include <array>
#include <cstdio>
#include <cstring>
#include <format>
#include <queue>
#include <string>
#include <utility>

#include "core/base/core_export.h"

namespace core {

[[nodiscard]] CORE_EXPORT std::string encode_escape(const std::string& input);
[[nodiscard]] CORE_EXPORT std::string decode_escape(const std::string& input);

constexpr char to_lower_ascii_char(char c) {
  return (c >= 'A' && c <= 'Z') ? (c | 0x20) : c;
}

constexpr char to_upper_ascii_char(char c) {
  return (c >= 'a' && c <= 'z') ? (c & ~0x20) : c;
}

template <std::size_t N>
constexpr auto to_lower_ascii(const char (&input)[N]) {
  std::array<char, N> result = {};
  for (std::size_t i = 0; i < N - 1; ++i) {
    result[i] = to_lower_ascii_char(input[i]);
  }
  result[N - 1] = '\0';
  return result;
}

template <std::size_t N>
constexpr auto to_upper_ascii(const char (&input)[N]) {
  std::array<char, N> result = {};
  for (std::size_t i = 0; i < N - 1; ++i) {
    result[i] = to_upper_ascii_char(input[i]);
  }
  result[N - 1] = '\0';
  return result;
}

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

CORE_EXPORT void format_address_safe(uintptr_t addr,
                                     char* buffer_start,
                                     std::size_t buffer_size);

CORE_EXPORT void padding(char*& cursor,
                         const char* const end,
                         std::size_t current_len,
                         std::size_t align_len);

CORE_EXPORT std::size_t write_raw(char*& dest,
                                  const char* source,
                                  std::size_t len);

template <typename... Args>
constexpr std::size_t write_format(char*& cursor,
                                   const char* const end,
                                   std::format_string<Args...> fmt,
                                   Args&&... args) {
  std::ptrdiff_t remaining = end - cursor;
  if (remaining <= 0) {
    return 0;
  }

  auto result =
      std::format_to_n(cursor, static_cast<std::size_t>(remaining - 1), fmt,
                       std::forward<Args>(args)...);
  std::size_t written = result.out - cursor;

  cursor += written;
  *cursor = '\0';
  return written;
}

inline bool starts_with(const std::string& input,
                        const std::string& prefix,
                        std::size_t index = 0) {
  return input.size() >= prefix.size() &&
         input.compare(index, prefix.size(), prefix) == 0;
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
