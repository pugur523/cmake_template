#ifndef CORE_CLI_ARG_PARSER_H_
#define CORE_CLI_ARG_PARSER_H_

#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "core/base/core_export.h"
#include "core/cli/argument_types/flag.h"
#include "core/cli/argument_types/list.h"
#include "core/cli/argument_types/option.h"
#include "core/cli/argument_types/positional.h"

namespace core {

enum class ParseResult : uint8_t {
  kSuccess = 0,
  kShowHelp = 1,
  kShowVersion = 2,
  kErrorInvalidFormat = 3,
  kErrorUnknownArgument = 4,
  kErrorFlagHasValue = 5,
  kErrorFlagParseFailed = 6,
  kErrorOptionMissingValue = 7,
  kErrorOptionParseFailed = 8,
  kErrorRequiredOptionMissing = 9,
  kErrorPositionalMissing = 10,
  kErrorPositionalParseFailed = 11,
  kErrorPositionalTooMany = 12,

  // Keep this at the end and equal to the last entry.
  kMaxValue = kErrorPositionalTooMany,
};

class CORE_EXPORT ArgumentParser {
 public:
  explicit ArgumentParser(const std::string& program_name,
                          const std::string& description = "");
  ~ArgumentParser();

  ArgumentParser(const ArgumentParser&) = delete;
  ArgumentParser& operator=(const ArgumentParser&) = delete;
  ArgumentParser(ArgumentParser&&) noexcept = default;
  ArgumentParser& operator=(ArgumentParser&&) noexcept = default;

  // Flag / Option / List / Positional adders

  template <typename T = bool>
  void add_flag(T* storage,
                const std::string& name,
                const std::string& description = "",
                bool required = false,
                const std::optional<T>& default_value = {false}) {
    static_assert(std::is_same_v<T, bool> || std::is_integral_v<T>,
                  "Flag only supports bool or integral types.");
    add_option_impl<Flag>(storage, name, description, required, default_value);
  }

  template <typename T>
  void add_option(T* storage,
                  const std::string& name,
                  const std::string& description = "",
                  bool required = false,
                  const std::optional<T>& default_value = std::nullopt) {
    add_option_impl<Option>(storage, name, description, required,
                            default_value);
  }

  template <typename T>
  void add_list(std::vector<T>* storage,
                const std::string& name,
                const std::string& description = "",
                bool required = false) {
    auto list = std::make_unique<List<T>>(storage, name, description, required);
    options_[name] = std::move(list);
    option_order_.push_back(name);
  }

  template <typename T>
  void add_positional(T* storage,
                      const std::string& name,
                      const std::string& description = "",
                      bool required = true) {
    auto positional =
        std::make_unique<Positional<T>>(storage, name, description, required);
    positional_parsers_.push_back(std::move(positional));
    positional_names_.push_back(name);
    positional_descriptions_.push_back(description);
    positional_required_.push_back(required);
  }

  void add_alias(const std::string& alias, const std::string& option_name);

  ParseResult parse(int argc, char** argv);

  void print_help() const;
  static void print_version();

 private:
  template <template <typename> typename ArgType, typename T>
  void add_option_impl(T* storage,
                       const std::string& name,
                       const std::string& description,
                       bool required,
                       const std::optional<T>& default_value) {
    auto arg = std::make_unique<ArgType<T>>(storage, name, description,
                                            required, default_value);
    options_[name] = std::move(arg);
    option_order_.push_back(name);
  }

  std::string resolve_alias(const std::string& name) const;
  void print_warn(const std::string& message) const;
  void print_error(const std::string& message) const;

  std::unordered_map<std::string, std::unique_ptr<OptionBase>> options_;
  std::unordered_map<std::string, std::string> aliases_;
  std::vector<std::string> option_order_;

  std::string program_name_;
  std::string description_;

  std::vector<bool> positional_required_;
  std::vector<std::unique_ptr<PositionalBase>> positional_parsers_;
  std::vector<std::string> positional_names_;
  std::vector<std::string> positional_descriptions_;
};

}  // namespace core

#endif  // CORE_CLI_ARG_PARSER_H_
