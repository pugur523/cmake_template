#pragma once

#include <sstream>
#include <string>
#include <type_traits>

#include "core/base/core_export.h"

namespace core {

class PositionalBase {
 public:
  PositionalBase() = default;
  virtual ~PositionalBase() = default;

  virtual bool parse(const std::string& value) = 0;

  virtual std::string description() const = 0;
  virtual std::string name() const = 0;
  virtual bool is_required() const = 0;
};

template <typename T>
class Positional : public PositionalBase {
 public:
  Positional(T* storage,
             const std::string& name,
             const std::string& description = "",
             bool required = true)
      : storage_(storage),
        name_(name),
        description_(description),
        required_(required) {}

  ~Positional() override = default;

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
  bool is_required() const override { return required_; }

 private:
  T* storage_;
  std::string name_;
  std::string description_;
  bool required_ : 1;
};

}  // namespace core
