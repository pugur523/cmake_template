#ifndef CORE_BASE_STRING_UTIL_H_
#define CORE_BASE_STRING_UTIL_H_

#include <algorithm>
#include <cstdio>
#include <queue>
#include <string>

#include "core/base/core_export.h"

namespace core {

CORE_EXPORT std::string encode_escape(const std::string& input);
CORE_EXPORT std::string decode_escape(const std::string& input);

CORE_EXPORT void to_lower(char* input);
CORE_EXPORT void to_lower(std::string* input);
CORE_EXPORT std::string to_lower(const std::string& input);

CORE_EXPORT void to_upper(char* input);
CORE_EXPORT void to_upper(std::string* input);
CORE_EXPORT std::string to_upper(const std::string& input);

CORE_EXPORT std::size_t utf8_char_length(unsigned char lead);
CORE_EXPORT std::string utf8_truncate(const std::string& input,
                                      std::size_t max_chars);
CORE_EXPORT std::queue<std::string> split_string(const std::string& input,
                                                 const std::string& delimiter);
CORE_EXPORT std::string remove_bracket(const std::string& input);

CORE_EXPORT void padding(char*& cursor,
                         const char* const end,
                         std::size_t current_len,
                         std::size_t align_len);
template <typename... Args>
void write(char*& cursor,
           const char* const end,
           const char* fmt,
           Args... args) {
  if (cursor >= end) {
    return;
  }
  int n = std::snprintf(cursor, end - cursor, fmt, args...);
  if (n > 0) {
    cursor += std::min(static_cast<std::size_t>(n),
                       static_cast<std::size_t>(end - cursor));
  }
}

}  // namespace core

#endif  // CORE_BASE_STRING_UTIL_H_
