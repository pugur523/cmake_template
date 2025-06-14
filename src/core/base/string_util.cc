#include "core/base/string_util.h"

#include <algorithm>
#include <cuchar>
#include <queue>
#include <string>

namespace core {

std::string encode_escape(const std::string& input) {
  std::string encoded;
  // encoded requires 2 * size in worst case.
  encoded.reserve(input.size() * 2);

  for (char c : input) {
    switch (c) {
      case '\n': encoded += "\\n"; break;
      case '\r': encoded += "\\r"; break;
      case '\t': encoded += "\\t"; break;
      case '\\': encoded += "\\\\"; break;
      default: encoded += c;
    }
  }
  return encoded;
}

std::string decode_escape(const std::string& input) {
  std::string decoded;
  decoded.reserve(input.size());

  for (std::size_t i = 0; i < input.size(); ++i) {
    if (input[i] == '\\' && i + 1 < input.size()) {
      switch (input[i + 1]) {
        case 'n': decoded += '\n'; break;
        case 'r': decoded += '\r'; break;
        case 't': decoded += '\t'; break;
        case '\\': decoded += '\\'; break;
        default: decoded += input[i + 1]; break;
      }
      i++;
    } else {
      decoded += input[i];
    }
  }
  return decoded;
}

void to_lower(char* input) {
  if (!input) {
    return;
  }
  for (std::size_t i = 0; input[i] != '\0'; ++i) {
    input[i] =
        static_cast<char>(std::tolower(static_cast<unsigned char>(input[i])));
  }
}

void to_lower(std::string* input) {
  if (!input) {
    return;
  }
  std::transform(
      input->begin(), input->end(), input->begin(),
      [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
}

std::string to_lower(const std::string& input) {
  std::string result(input.size(), '\0');
  std::transform(
      input.begin(), input.end(), result.begin(),
      [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  return result;
}

void to_upper(char* input) {
  if (!input) {
    return;
  }
  for (std::size_t i = 0; input[i] != '\0'; ++i) {
    input[i] =
        static_cast<char>(std::toupper(static_cast<unsigned char>(input[i])));
  }
}

void to_upper(std::string* input) {
  if (!input) {
    return;
  }
  std::transform(
      input->begin(), input->end(), input->begin(),
      [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
}

std::string to_upper(const std::string& input) {
  std::string result(input.size(), '\0');
  std::transform(
      input.begin(), input.end(), result.begin(),
      [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
  return result;
}

std::size_t utf8_char_length(unsigned char lead) {
  if (lead < 0x80) {
    return 1;
  } else if ((lead >> 5) == 0x6) {
    return 2;
  } else if ((lead >> 4) == 0xE) {
    return 3;
  } else if ((lead >> 3) == 0x1E) {
    return 4;
  } else {
    return 1;
  }
}

std::string utf8_truncate(const std::string& input, std::size_t max_chars) {
  std::size_t i = 0;
  std::size_t chars = 0;

  while (i < input.size() && chars < max_chars) {
    std::size_t len = utf8_char_length(static_cast<unsigned char>(input[i]));
    if (i + len > input.size()) {
      break;
    }
    i += len;
    ++chars;
  }

  return input.substr(0, i);
}

std::queue<std::string> split_string(const std::string& input,
                                     const std::string& delimiter) {
  std::queue<std::string> result;
  std::size_t pos = 0;
  std::size_t delimiter_length = delimiter.length();
  std::size_t input_length = input.length();
  while (pos < input_length) {
    std::size_t next_pos = input.find(delimiter, pos);
    if (next_pos == std::string::npos) {
      result.push(input.substr(pos));
      break;
    } else {
      result.push(input.substr(pos, next_pos - pos));
      pos = next_pos + delimiter_length;
    }
  }
  return result;
}

std::string remove_bracket(const std::string& input) {
  std::string output;
  output.reserve(input.size());

  constexpr std::size_t kRemoveBracketMaxNestSize = 32;
  char stack[kRemoveBracketMaxNestSize];
  std::size_t depth = 0;

  for (char c : input) {
    if (depth > 0) {
      switch (c) {
        case '(':
        case '{':
        case '[':
        case '<':
          if (depth < kRemoveBracketMaxNestSize) {
            stack[depth++] = c;
          }
          break;
        case ')':
          if (depth > 0 && stack[depth - 1] == '(') {
            --depth;
          }
          break;
        case '}':
          if (depth > 0 && stack[depth - 1] == '{') {
            --depth;
          }
          break;
        case ']':
          if (depth > 0 && stack[depth - 1] == '[') {
            --depth;
          }
          break;
        case '>':
          if (depth > 0 && stack[depth - 1] == '<') {
            --depth;
          }
          break;
        default: break;
      }
      continue;
    }

    switch (c) {
      case '(':
      case '{':
      case '[':
      case '<': stack[depth++] = c; break;
      default: output += c; break;
    }
  }

  return output;
}

std::size_t safe_strlen(const char* str) {
  if (!str) {
    return 0;
  }
  std::size_t len = 0;
  while (str[len] != '\0') {
    ++len;
  }
  return len;
}

void padding(char*& cursor,
             const char* const end,
             std::size_t current_len,
             std::size_t align_len) {
  std::size_t pad_len = (align_len > current_len) ? align_len - current_len : 0;
  std::size_t to_pad =
      std::min(pad_len, static_cast<std::size_t>(end - cursor));
  std::memset(cursor, ' ', to_pad);
  cursor += to_pad;
}

std::size_t write_raw(char*& dest, const char* source, std::size_t len) {
  if (len == 0) {
    return 0;
  }

  std::memcpy(dest, source, len);
  return len;
}

}  // namespace core
