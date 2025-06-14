#ifndef CORE_CLI_ARGUMENT_TYPES_LIST_H_
#define CORE_CLI_ARGUMENT_TYPES_LIST_H_

#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include "core/base/core_export.h"
#include "core/cli/argument_types/option_base.h"

namespace core {

template <typename T>
class List : public OptionBase {
 public:
  List(std::vector<T>* storage,
       const std::string& name,
       const std::string& description = "",
       bool required = false)
      : storage_(storage),
        name_(name),
        description_(description),
        required_(required) {}

  bool parse(const std::string& value) override {
    T item;
    if constexpr (std::is_same_v<T, std::string>) {
      storage_->push_back(value);
      return true;
    } else if constexpr (std::is_arithmetic_v<T>) {
      std::istringstream iss(value);
      if (iss >> item) {
        storage_->push_back(item);
        return true;
      }
      return false;
    } else {
      std::istringstream iss(value);
      if (iss >> item) {
        storage_->push_back(item);
        return true;
      }
      return false;
    }
  }

  std::string description() const override { return description_; }
  std::string name() const override { return name_; }
  std::string default_value_str() const override { return ""; }
  bool is_flag() const override { return false; }
  bool is_required() const override { return required_; }
  bool has_value() const override { return true; }
  bool has_default() const override { return false; }

 private:
  std::vector<T>* storage_;
  std::string name_;
  std::string description_;
  bool required_ : 1;
};

}  // namespace core

#endif  // CORE_CLI_ARGUMENT_TYPES_LIST_H_
