#include <algorithm>
#include <random>
#include <string>
#include <vector>

#include "benchmark/benchmark.h"
#include "core/base/string_util.h"

namespace {

const char* kShortString = "hello world";
const char* kMediumString = "The quick brown fox jumps over the lazy dog.";
const std::string kLongString = [] {  // NOLINT
  std::string s;
  s.reserve(1024 * 10);  // 10 KiB
  for (int i = 0; i < 1000; ++i) {
    s += "a long string for benchmarking purposes. ";
  }
  return s;
}();

constexpr const char* kStringWithEscape = "hello world\\n\\t\\r\\\"test\\\'";
// constexpr const char* kStringWithMixedCase = "hElLo WoRlD";
constexpr const char* kUtf8String = "こんにちは世界🌍";  // 15 chars, 20 bytes
constexpr const char* kSplitString = "one,two,three,four,five";
constexpr const char* kBracketString = "(abc)[def]{ghi}";
constexpr const char* kFormatString = "Value: {}, Text: {}";
constexpr const char* kPrefixString = "prefix_test_string";

std::mt19937 g_rng(std::random_device{}());  // NOLINT

std::string generate_random_string(std::size_t length) {
  std::string s(length, '\0');
  static const char charset[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
  std::uniform_int_distribution<> distrib(0, sizeof(charset) - 2);
  for (std::size_t i = 0; i < length; ++i) {
    s[i] = charset[distrib(g_rng)];
  }
  return s;
}

void string_util_encode_escape_short(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(core::encode_escape(kStringWithEscape));
  }
}
BENCHMARK(string_util_encode_escape_short);

void string_util_encode_escape_long(benchmark::State& state) {
  std::string long_escape_string = kLongString;
  for (int i = 0; i < 1000; ++i) {
    long_escape_string += "\\n";
  }
  for (auto _ : state) {
    benchmark::DoNotOptimize(core::encode_escape(long_escape_string));
  }
}
BENCHMARK(string_util_encode_escape_long);

void string_util_decode_escape_short(benchmark::State& state) {
  std::string encoded = core::encode_escape(kStringWithEscape);
  for (auto _ : state) {
    benchmark::DoNotOptimize(core::decode_escape(encoded));
  }
}
BENCHMARK(string_util_decode_escape_short);

void string_util_decode_escape_long(benchmark::State& state) {
  std::string long_escape_string = kLongString;
  for (int i = 0; i < 1000; ++i) {
    long_escape_string += "\\n";
  }
  std::string encoded = core::encode_escape(long_escape_string);
  for (auto _ : state) {
    benchmark::DoNotOptimize(core::decode_escape(encoded));
  }
}
BENCHMARK(string_util_decode_escape_long);

void string_util_to_lower_ascii(benchmark::State& state) {
  char* buf = new char[kLongString.length() + 1];
  for (auto _ : state) {
    constexpr const auto lowered =
        core::to_lower_ascii("AbCdeFgHijkLmNOPQRStuVwXYZ");
    benchmark::DoNotOptimize(buf);
    core::write_raw(buf, lowered.data(), lowered.size() + 1);
  }
  delete[] buf;
}
BENCHMARK(string_util_to_lower_ascii);

void string_util_to_lower_char_ptr(benchmark::State& state) {
  char* buf = new char[kLongString.length() + 1];
  core::write_raw(buf, kLongString.c_str(), kLongString.length() + 1);
  for (auto _ : state) {
    core::to_lower(buf);
    benchmark::DoNotOptimize(buf);
    core::write_raw(buf, kLongString.c_str(), kLongString.length() + 1);
  }
  delete[] buf;
}
BENCHMARK(string_util_to_lower_char_ptr);

void string_util_to_lower_string_ptr(benchmark::State& state) {
  std::string s = kLongString;
  for (auto _ : state) {
    core::to_lower(&s);
    benchmark::DoNotOptimize(s);
    s = kLongString;
  }
}
BENCHMARK(string_util_to_lower_string_ptr);

void string_util_to_lower_const_string_ref(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(core::to_lower(kLongString));
  }
}
BENCHMARK(string_util_to_lower_const_string_ref);

void string_util_to_upper_char_ptr(benchmark::State& state) {
  char* buf = new char[kLongString.length() + 1];
  core::write_raw(buf, kLongString.c_str(), kLongString.length() + 1);
  for (auto _ : state) {
    core::to_upper(buf);
    benchmark::DoNotOptimize(buf);
    core::write_raw(buf, kLongString.c_str(), kLongString.length() + 1);
  }
  delete[] buf;
}
BENCHMARK(string_util_to_upper_char_ptr);

void string_util_to_upper_string_ptr(benchmark::State& state) {
  std::string s = kLongString;
  for (auto _ : state) {
    core::to_upper(&s);
    benchmark::DoNotOptimize(s);
    s = kLongString;
  }
}
BENCHMARK(string_util_to_upper_string_ptr);

void string_util_to_upper_const_string_ref(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(core::to_upper(kLongString));
  }
}
BENCHMARK(string_util_to_upper_const_string_ref);

void string_util_utf8_char_length(benchmark::State& state) {
  const unsigned char lead_byte_ascii = 'A';
  const unsigned char lead_byte_2byte = 0xC2;  // Example for '¢'
  const unsigned char lead_byte_3byte = 0xE2;  // Example for '€'
  const unsigned char lead_byte_4byte = 0xF0;  // Example for '𐐷'

  for (auto _ : state) {
    benchmark::DoNotOptimize(core::utf8_char_length(lead_byte_ascii));
    benchmark::DoNotOptimize(core::utf8_char_length(lead_byte_2byte));
    benchmark::DoNotOptimize(core::utf8_char_length(lead_byte_3byte));
    benchmark::DoNotOptimize(core::utf8_char_length(lead_byte_4byte));
  }
}
BENCHMARK(string_util_utf8_char_length);

void string_util_utf8_truncate_no_truncate(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(core::utf8_truncate(kUtf8String, 20));
  }
}
BENCHMARK(string_util_utf8_truncate_no_truncate);

void string_util_utf8_truncate_truncate_mid(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(core::utf8_truncate(kUtf8String, 5));
  }
}
BENCHMARK(string_util_utf8_truncate_truncate_mid);

void string_util_utf8_truncate_long_string(benchmark::State& state) {
  std::string long_utf8_string = kLongString;
  long_utf8_string += kUtf8String;
  long_utf8_string += kUtf8String;

  for (auto _ : state) {
    benchmark::DoNotOptimize(
        core::utf8_truncate(long_utf8_string, long_utf8_string.length() / 2));
  }
}
BENCHMARK(string_util_utf8_truncate_long_string);

void string_util_split_string_comma(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(core::split_string(kSplitString, ","));
  }
}
BENCHMARK(string_util_split_string_comma);

void string_util_split_string_long(benchmark::State& state) {
  std::string long_split_string;
  for (int i = 0; i < 1000; ++i) {
    long_split_string += "item" + std::to_string(i) + ",";
  }
  long_split_string.pop_back();

  for (auto _ : state) {
    benchmark::DoNotOptimize(core::split_string(long_split_string, ","));
  }
}
BENCHMARK(string_util_split_string_long);

void string_util_remove_bracket_simple(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(core::remove_bracket(kBracketString));
  }
}
BENCHMARK(string_util_remove_bracket_simple);

void string_util_remove_bracket_no_bracket(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(core::remove_bracket(kShortString));
  }
}
BENCHMARK(string_util_remove_bracket_no_bracket);

void string_util_remove_bracket_no_bracket_long(benchmark::State& state) {
  std::string long_bracket_string = kLongString;
  for (auto _ : state) {
    benchmark::DoNotOptimize(core::remove_bracket(long_bracket_string));
  }
}
BENCHMARK(string_util_remove_bracket_no_bracket_long);

void string_util_remove_bracket_long(benchmark::State& state) {
  std::string long_bracket_string = kLongString;
  long_bracket_string += "(extra){brackets}[here]";
  for (auto _ : state) {
    benchmark::DoNotOptimize(core::remove_bracket(long_bracket_string));
  }
}
BENCHMARK(string_util_remove_bracket_long);

void string_util_safe_strlen(benchmark::State& state) {
  const char* c_str = kLongString.c_str();
  for (auto _ : state) {
    benchmark::DoNotOptimize(core::safe_strlen(c_str));
  }
}
BENCHMARK(string_util_safe_strlen);

void string_util_safe_strlen_short(benchmark::State& state) {
  const char* c_str = kShortString;
  for (auto _ : state) {
    benchmark::DoNotOptimize(core::safe_strlen(c_str));
  }
}
BENCHMARK(string_util_safe_strlen_short);

void string_util_padding(benchmark::State& state) {
  const std::size_t buffer_size = 1024;
  std::vector<char> buffer(buffer_size);
  char* cursor;
  const char* end = buffer.data() + buffer_size;

  for (auto _ : state) {
    cursor = buffer.data();
    std::size_t current_len = 50;
    std::size_t align_len = 8;
    core::padding(cursor, end, current_len, align_len);
    benchmark::DoNotOptimize(cursor);
  }
}
BENCHMARK(string_util_padding);

void string_util_write_raw(benchmark::State& state) {
  const std::size_t buffer_size = 1024;
  std::vector<char> buffer(buffer_size);
  const char* source_data = kMediumString;
  std::size_t source_len = core::safe_strlen(kMediumString);

  for (auto _ : state) {
    char* cursor = buffer.data();
    benchmark::DoNotOptimize(core::write_raw(cursor, source_data, source_len));
  }
}
BENCHMARK(string_util_write_raw);

void string_util_write_raw_long(benchmark::State& state) {
  const std::size_t buffer_size = kLongString.length() + 100;
  std::vector<char> buffer(buffer_size);
  const char* source_data = kLongString.c_str();
  std::size_t source_len = kLongString.length();

  for (auto _ : state) {
    char* cursor = buffer.data();
    benchmark::DoNotOptimize(core::write_raw(cursor, source_data, source_len));
  }
}
BENCHMARK(string_util_write_raw_long);

void string_util_write_format_int_string(benchmark::State& state) {
  const std::size_t buffer_size = 256;
  std::vector<char> buffer(buffer_size);

  for (auto _ : state) {
    char* cursor = buffer.data();
    const char* end = buffer.data() + buffer_size;
    benchmark::DoNotOptimize(
        core::write_format(cursor, end, kFormatString, 123, "benchmark"));
  }
}
BENCHMARK(string_util_write_format_int_string);

void string_util_write_format_long_output(benchmark::State& state) {
  const std::size_t buffer_size = 1024;
  std::vector<char> buffer(buffer_size);
  std::string long_text_arg = generate_random_string(500);

  for (auto _ : state) {
    char* cursor = buffer.data();
    const char* end = buffer.data() + buffer_size;
    benchmark::DoNotOptimize(core::write_format(cursor, end, "Long text: {}",
                                                long_text_arg.c_str()));
  }
}
BENCHMARK(string_util_write_format_long_output);

void string_util_starts_with_match(benchmark::State& state) {
  const std::string prefix = "prefix";
  for (auto _ : state) {
    benchmark::DoNotOptimize(core::starts_with(kPrefixString, prefix));
  }
}
BENCHMARK(string_util_starts_with_match);

void string_util_starts_with_no_match(benchmark::State& state) {
  const std::string prefix = "nomatch";
  for (auto _ : state) {
    benchmark::DoNotOptimize(core::starts_with(kPrefixString, prefix));
  }
}
BENCHMARK(string_util_starts_with_no_match);

void string_util_starts_with_long_string_match(benchmark::State& state) {
  std::string long_string = kLongString;
  std::string prefix = long_string.substr(0, 100);
  for (auto _ : state) {
    benchmark::DoNotOptimize(core::starts_with(long_string, prefix));
  }
}
BENCHMARK(string_util_starts_with_long_string_match);

void string_util_starts_with_long_string_no_match(benchmark::State& state) {
  std::string long_string = kLongString;
  std::string prefix = long_string.substr(0, 99) + "X";
  for (auto _ : state) {
    benchmark::DoNotOptimize(core::starts_with(long_string, prefix));
  }
}
BENCHMARK(string_util_starts_with_long_string_no_match);

}  // namespace

