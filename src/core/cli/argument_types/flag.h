#ifndef CORE_CLI_ARGUMENT_TYPES_FLAG_H_
#define CORE_CLI_ARGUMENT_TYPES_FLAG_H_

#include <optional>
#include <sstream>
#include <string>
#include <type_traits>

#include "core/base/core_export.h"
#include "core/cli/argument_types/option_base.h"

namespace core {

template <typename T = bool>
class Flag : public OptionBase {
 public:
  Flag(T* storage,
       const std::string& name,
       const std::string& description = "",
       bool required = false,
       const std::optional<T>& default_value = std::nullopt)
      : storage_(storage),
        name_(name),
        description_(description),
        required_(required),
        has_default_(false) {
    if (default_value.has_value()) {
      default_value_ = *default_value;
      has_default_ = true;
      *storage_ = default_value_;
    }
  }

  bool parse(const std::string& value) override {
    if constexpr (std::is_same_v<T, bool>) {
      *storage_ = true;
      return true;
    } else {
      std::istringstream iss(value);
      return static_cast<bool>(iss >> *storage_);
    }
  }

  std::string description() const override { return description_; }
  std::string name() const override { return name_; }
  std::string default_value_str() const override {
    if constexpr (std::is_same_v<T, bool>) {
      return has_default_ ? (default_value_ ? "true" : "false") : "";
    } else {
      return has_default_ ? std::to_string(default_value_) : "";
    }
  }
  bool is_flag() const override { return std::is_same_v<T, bool>; }
  bool is_required() const override { return required_; }
  bool has_value() const override { return !std::is_same_v<T, bool>; }
  bool has_default() const override { return has_default_; }

 private:
  T* storage_;
  std::string name_;
  std::string description_;
  bool required_ : 1;
  bool has_default_ : 1;
  T default_value_;
};

}  // namespace core

#endif  // CORE_CLI_ARGUMENT_TYPES_FLAG_H_
