#ifndef CORE_CLI_ANSI_STYLE_BUILDER_H_
#define CORE_CLI_ANSI_STYLE_BUILDER_H_

#include <string>
#include <string_view>

#include "core/base/core_export.h"
#include "core/cli/ansi/style_util.h"
#include "core/cli/console.h"

namespace core {

inline constexpr Style operator|(Style a, Style b) {
  return static_cast<Style>(static_cast<uint16_t>(a) |
                            static_cast<uint16_t>(b));
}

inline constexpr Style operator&(Style a, Style b) {
  return static_cast<Style>(static_cast<uint16_t>(a) &
                            static_cast<uint16_t>(b));
}

inline constexpr Style operator^(Style a, Style b) {
  return static_cast<Style>(static_cast<uint16_t>(a) ^
                            static_cast<uint16_t>(b));
}

inline constexpr Style operator~(Style a) {
  return static_cast<Style>(~static_cast<uint16_t>(a));
}

inline constexpr Style& operator|=(Style& a, Style b) {
  return a = a | b;
}

inline constexpr Style& operator&=(Style& a, Style b) {
  return a = a & b;
}

inline constexpr Style& operator^=(Style& a, Style b) {
  return a = a ^ b;
}

inline constexpr bool has_style(Style styles, Style style) {
  return (styles & style) != Style::kDefault;
}

class CORE_EXPORT StyleBuilder {
 public:
  constexpr StyleBuilder() = default;

  ~StyleBuilder() = default;

  StyleBuilder(const StyleBuilder&) = delete;
  StyleBuilder& operator=(const StyleBuilder&) = delete;

  StyleBuilder(StyleBuilder&&) noexcept = default;
  StyleBuilder& operator=(StyleBuilder&&) noexcept = default;

  inline constexpr StyleBuilder& style(Style s) {
    styles_ = s;
    update_should_build_cache();
    return *this;
  }

  inline constexpr StyleBuilder& add_style(Style s) {
    styles_ |= s;
    update_should_build_cache();
    return *this;
  }

  inline constexpr StyleBuilder& remove_style(Style s) {
    styles_ &= ~s;
    update_should_build_cache();
    return *this;
  }

  inline constexpr StyleBuilder& clear_style() {
    styles_ = Style::kDefault;
    update_should_build_cache();
    return *this;
  }

  inline constexpr StyleBuilder& color(Color c) {
    fg_ = c;
    update_should_build_cache();
    return *this;
  }

  inline constexpr StyleBuilder& bg(BgColor c) {
    bg_ = c;
    update_should_build_cache();
    return *this;
  }

  inline constexpr StyleBuilder& rgb(uint8_t r, uint8_t g, uint8_t b) {
    fg_rgb_[0] = r;
    fg_rgb_[1] = g;
    fg_rgb_[2] = b;
    use_fg_rgb_ = true;
    update_should_build_cache();
    return *this;
  }

  inline constexpr StyleBuilder& bg_rgb(uint8_t r, uint8_t g, uint8_t b) {
    bg_rgb_[0] = r;
    bg_rgb_[1] = g;
    bg_rgb_[2] = b;
    use_bg_rgb_ = true;
    update_should_build_cache();
    return *this;
  }

  std::string build(std::string_view text) const;

  inline std::string build(const char* ptr, std::size_t size) const {
    return build(std::string_view(ptr, size));
  }

  inline std::string build(std::string&& text) const {
    // zero copy
    if (!should_build_) {
      return text;
    }
    return build(std::string_view(text));
  }

  inline std::string build(const char* text) {
    return build(std::string_view(text));
  }

  inline constexpr void reset() {
    this->~StyleBuilder();
    *this = StyleBuilder();
  }

 private:
  inline void update_should_build_cache() {
    if (!can_use_ansi_escape_sequence()) {
      should_build_ = false;
      return;
    }

    should_build_ = styles_ != Style::kDefault || use_fg_rgb_ ||
                    fg_ != Color::kDefault || use_bg_rgb_ ||
                    bg_ != BgColor::kDefault;
  }

  Style styles_ = Style::kDefault;

  Color fg_ = Color::kDefault;
  BgColor bg_ = BgColor::kDefault;

  bool should_build_ : 1 = false;
  bool use_fg_rgb_ : 1 = false;
  bool use_bg_rgb_ : 1 = false;
  uint8_t fg_rgb_[3];
  uint8_t bg_rgb_[3];
};

}  // namespace core

#endif  // CORE_CLI_ANSI_STYLE_BUILDER_H_
