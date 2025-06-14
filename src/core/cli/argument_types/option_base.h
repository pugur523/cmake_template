#ifndef CORE_CLI_ARGUMENT_TYPES_OPTION_BASE_H_
#define CORE_CLI_ARGUMENT_TYPES_OPTION_BASE_H_

#include <string>

#include "core/base/core_export.h"

namespace core {

class CORE_EXPORT OptionBase {
 public:
  OptionBase() = default;
  virtual ~OptionBase() = default;

  OptionBase(const OptionBase&) = delete;
  OptionBase& operator=(const OptionBase&) = delete;

  OptionBase(OptionBase&&) noexcept = default;
  OptionBase& operator=(OptionBase&&) noexcept = default;

  virtual bool parse(const std::string& value) = 0;

  virtual std::string description() const = 0;
  virtual std::string name() const = 0;
  virtual std::string default_value_str() const = 0;

  virtual bool is_flag() const = 0;
  virtual bool is_required() const = 0;
  virtual bool has_value() const = 0;
  virtual bool has_default() const = 0;
};

}  // namespace core

#endif  // CORE_CLI_ARGUMENT_TYPES_OPTION_BASE_H_
