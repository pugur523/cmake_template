#ifndef CORE_CLI_ARGUMENT_TYPES_OPTION_H_
#define CORE_CLI_ARGUMENT_TYPES_OPTION_H_

#include <optional>
#include <sstream>
#include <string>

#include "core/base/core_export.h"
#include "core/cli/argument_types/option_base.h"

namespace core {

template <typename T>
class Option : public OptionBase {
 public:
  Option(T* storage,
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
    if constexpr (std::is_same_v<T, std::string>) {
      *storage_ = value;
      return true;
    } else if constexpr (std::is_arithmetic_v<T>) {
      std::istringstream iss(value);
      return static_cast<bool>(iss >> *storage_);
    } else {
      std::istringstream iss(value);
      return static_cast<bool>(iss >> *storage_);
    }
  }

  std::string description() const override { return description_; }
  std::string name() const override { return name_; }
  std::string default_value_str() const override {
    if constexpr (std::is_same_v<T, std::string>) {
      return has_default_ ? default_value_ : "";
    } else {
      return has_default_ ? std::to_string(default_value_) : "";
    }
  }
  bool is_flag() const override { return false; }
  bool is_required() const override { return required_; }
  bool has_value() const override { return true; }
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

#endif  // CORE_CLI_ARGUMENT_TYPES_OPTION_H_
