#include "core/cli/ansi/style_builder.h"

#include <charconv>
#include <string>
#include <string_view>

namespace core {

namespace {

// used for 0~255 only
inline char* uint8_to_chars(char* buffer, uint8_t value) {
  if (value >= 100) {
    *buffer++ = '0' + (value / 100);
    value %= 100;
    *buffer++ = '0' + (value / 10);
    *buffer++ = '0' + (value % 10);
  } else if (value >= 10) {
    *buffer++ = '0' + (value / 10);
    *buffer++ = '0' + (value % 10);
  } else {
    *buffer++ = '0' + value;
  }
  return buffer;
}

void append_rgb_sequence(std::string* result,
                         std::string_view prefix,
                         const uint8_t rgb[3]) {
  constexpr std::size_t kBufSize = 32;
  char buffer[kBufSize];
  char* pos = buffer;

  // prefix
  for (char c : prefix) {
    *pos++ = c;
  }

  // r
  pos = uint8_to_chars(pos, rgb[0]);
  *pos++ = kSemicolon;

  // g
  pos = uint8_to_chars(pos, rgb[1]);
  *pos++ = kSemicolon;

  // b
  pos = uint8_to_chars(pos, rgb[2]);

  // suffix
  *pos++ = kRgbSuffix;

  result->append(buffer, pos - buffer);
}

}  // namespace

std::string StyleBuilder::build(std::string_view text) const {
  std::string result;

  if (!should_build_) {
    result = text;
    return result;
  }

  // size prediction
  std::size_t estimated_size = text.size() + kResetCodeLength;
  uint16_t style_bits = static_cast<uint16_t>(styles_);
  int style_count = 0;
  for (int i = 0; i < 8; ++i) {
    if (style_bits & (1 << i)) {
      style_count++;
    }
  }
  estimated_size += style_count * kStyleCodeLength;
  if (use_fg_rgb_) {
    estimated_size += kRgbCodeLength;
  } else if (fg_ != Color::kDefault) {
    estimated_size += kStyleCodeLength;
  }
  if (use_bg_rgb_) {
    estimated_size += kRgbCodeLength;
  } else if (bg_ != BgColor::kDefault) {
    estimated_size += kStyleCodeLength;
  }
  result.reserve(estimated_size);

  // style
  if (styles_ != Style::kDefault) {
    uint16_t style_bits = static_cast<uint16_t>(styles_);
    for (int i = 0; i < 8; ++i) {
      if (style_bits & (1 << i)) {
        result.append(kStyleCodes[i]);
      }
    }
  }

  // foreground
  if (use_fg_rgb_) {
    append_rgb_sequence(&result, kFgRgbPrefix, fg_rgb_);
  } else if (fg_ != Color::kDefault) {
    result.append(kColorCodes[static_cast<uint8_t>(fg_)]);
  }

  // background
  if (use_bg_rgb_) {
    append_rgb_sequence(&result, kBgRgbPrefix, bg_rgb_);
  } else if (bg_ != BgColor::kDefault) {
    result.append(kBgColorCodes[static_cast<uint8_t>(bg_)]);
  }

  // text + reset
  result.append(text);
  result.append(kResetCode);

  return result;
}

}  // namespace core
