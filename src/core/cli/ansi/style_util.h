#ifndef CORE_CLI_ANSI_STYLE_UTIL_H_
#define CORE_CLI_ANSI_STYLE_UTIL_H_

#include <array>
#include <cstdint>

#include "core/base/core_export.h"
#include "core/check.h"

namespace core {

// base styles
enum class Style : uint8_t {
  kReset = 0,
  kBold = 1 << 0,       // 1
  kDim = 1 << 1,        // 2
  kItalic = 1 << 2,     // 4
  kUnderline = 1 << 3,  // 8
  kBlink = 1 << 4,      // 16
  kReverse = 1 << 5,    // 32
  kHidden = 1 << 6,     // 64
  kStrike = 1 << 7,     // 128

  // alias
  kBoldUnderline = kBold | kUnderline,
  kBoldItalic = kBold | kItalic,

  kDefault = kReset,
};

// foreground colors
enum class Color : uint8_t {
  kBlack = 0,
  kRed = 1,
  kGreen = 2,
  kYellow = 3,
  kBlue = 4,
  kMagenta = 5,
  kCyan = 6,
  kWhite = 7,
  kGray = 8,
  kBrightRed = 9,
  kBrightGreen = 10,
  kBrightYellow = 11,
  kBrightBlue = 12,
  kBrightMagenta = 13,
  kBrightCyan = 14,
  kBrightWhite = 15,

  kDefault = 255,
};

// background colors
enum class BgColor : uint8_t {
  kBlack = 0,
  kRed = 1,
  kGreen = 2,
  kYellow = 3,
  kBlue = 4,
  kMagenta = 5,
  kCyan = 6,
  kWhite = 7,
  kGray = 8,
  kBrightRed = 9,
  kBrightGreen = 10,
  kBrightYellow = 11,
  kBrightBlue = 12,
  kBrightMagenta = 13,
  kBrightCyan = 14,
  kBrightWhite = 15,

  kDefault = 255,
};

constexpr const char* kResetCode = "\033[0m";
constexpr const char* kFgRgbPrefix = "\033[38;2;";
constexpr const char* kBgRgbPrefix = "\033[48;2;";
constexpr const char kRgbSuffix = 'm';
constexpr const char kSemicolon = ';';

constexpr const std::size_t kStyleCodeLength = 4;
constexpr const std::size_t kResetCodeLength = kStyleCodeLength;
constexpr const std::size_t kRgbCodeLength = 20;

inline constexpr std::array<const char*, 8> kStyleCodes = {
    "\033[1m", "\033[2m", "\033[3m", "\033[4m",
    "\033[5m", "\033[7m", "\033[8m", "\033[9m",
};

inline constexpr std::array<const char*, 16> kColorCodes = {
    "\033[30m", "\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m",
    "\033[36m", "\033[37m", "\033[90m", "\033[91m", "\033[92m", "\033[93m",
    "\033[94m", "\033[95m", "\033[96m", "\033[97m",
};

inline constexpr std::array<const char*, 16> kBgColorCodes = {
    "\033[40m",  "\033[41m",  "\033[42m",  "\033[43m",
    "\033[44m",  "\033[45m",  "\033[46m",  "\033[47m",
    "\033[100m", "\033[101m", "\033[102m", "\033[103m",
    "\033[104m", "\033[105m", "\033[106m", "\033[107m",
};

inline constexpr const char* style_str(Style style) {
  DCHECK_LE(static_cast<uint8_t>(style), static_cast<uint8_t>(Style::kStrike));
  if (style == Style::kReset) {
    return kResetCode;
  }
  return kStyleCodes[static_cast<uint8_t>(style)];
}

inline constexpr const char* color_str(Color color) {
  if (color == Color::kDefault) {
    return kResetCode;
  }
  return kColorCodes[static_cast<uint8_t>(color)];
}

inline constexpr const char* color_str(BgColor bg_color) {
  if (bg_color == BgColor::kDefault) {
    return kResetCode;
  }
  return kBgColorCodes[static_cast<uint8_t>(bg_color)];
}

}  // namespace core

#endif  // CORE_CLI_ANSI_STYLE_UTIL_H_
