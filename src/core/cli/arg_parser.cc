#include "core/cli/arg_parser.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "build/build_config.h"
#include "core/base/file_util.h"
#include "core/base/string_util.h"

namespace core {

namespace {

enum class ArgType {
  Long,
  Short,
  DoubleDash,
  LoneDash,
  Positional,
  Invalid,
};

ArgType classify_argument(const std::string& arg) {
  if (arg == "--") {
    return ArgType::DoubleDash;
  }
  if (arg == "-") {
    return ArgType::LoneDash;
  }
  if (starts_with(arg, "--")) {
    return arg.length() > 2 ? ArgType::Long : ArgType::Invalid;
  }
  if (starts_with(arg, "-")) {
    return arg.length() > 1 ? ArgType::Short : ArgType::Invalid;
  }
  return ArgType::Positional;
}

bool is_help_flag(const std::string& arg) {
  return arg == "-h" || arg == "--help";
}

bool is_version_flag(const std::string& arg) {
  return arg == "-v" || arg == "--version";
}

}  // namespace

ArgumentParser::ArgumentParser(const std::string& program_name,
                               const std::string& description)
    : program_name_(program_name), description_(description) {}

ArgumentParser::~ArgumentParser() = default;

void ArgumentParser::add_alias(const std::string& alias,
                               const std::string& option_name) {
  aliases_[alias] = option_name;
}

std::string ArgumentParser::resolve_alias(const std::string& name) const {
  auto it = aliases_.find(name);
  return it != aliases_.end() ? it->second : name;
}

ParseResult ArgumentParser::parse(int argc, char** argv) {
  if (argc < 1) {
    return ParseResult::kErrorInvalidFormat;
  }

  std::vector<std::string> args(argv + 1, argv + argc);
  std::vector<std::string> positional_args;
  std::unordered_map<std::string, bool> parsed;

  for (const auto& name : option_order_) {
    parsed[name] = false;
  }

  for (std::size_t i = 0; i < args.size(); ++i) {
    const std::string& arg = args[i];

    if (is_help_flag(arg)) {
      print_help();
      return ParseResult::kShowHelp;
    }
    if (is_version_flag(arg)) {
      print_version();
      return ParseResult::kShowVersion;
    }

    ArgType type = classify_argument(arg);
    switch (type) {
      case ArgType::DoubleDash:
        positional_args.insert(positional_args.end(), args.begin() + i + 1,
                               args.end());
        i = args.size();
        break;
      case ArgType::LoneDash:
      case ArgType::Positional: positional_args.push_back(arg); break;
      case ArgType::Invalid:
        print_error("invalid option format: '" + arg + "'");
        return ParseResult::kErrorInvalidFormat;
      case ArgType::Long:
      case ArgType::Short: {
        std::string optname, value;
        bool has_value = false;

        if (type == ArgType::Long) {
          auto eq = arg.find('=');
          if (eq != std::string::npos) {
            optname = arg.substr(2, eq - 2);
            value = arg.substr(eq + 1);
            has_value = true;
          } else {
            optname = arg.substr(2);
          }
        } else {
          auto eq = arg.find('=');
          if (eq != std::string::npos) {
            optname = arg.substr(1, eq - 1);
            value = arg.substr(eq + 1);
            has_value = true;
          } else {
            auto sp = arg.find(' ');
            if (sp != std::string::npos) {
              optname = arg.substr(1, sp - 1);
              value = arg.substr(sp + 1);
              has_value = true;
            } else {
              optname = arg.substr(1, arg.length() - 1);
            }
          }
        }

        optname = resolve_alias(optname);
        auto it = options_.find(optname);
        if (it == options_.end()) {
          print_error("unknown argument: '" + arg + "'");
          return ParseResult::kErrorUnknownArgument;
        }

        OptionBase* opt = it->second.get();
        parsed[optname] = true;

        if (opt->is_flag()) {
          if (has_value) {
            print_error("flag option '" + arg + "' does not accept a value");
            return ParseResult::kErrorFlagHasValue;
          }
          if (!opt->parse("")) {
            print_error("failed to parse flag: '" + arg + "'");
            return ParseResult::kErrorFlagParseFailed;
          }
        } else {
          if (!has_value) {
            if (i + 1 >= args.size()) {
              print_error("option '" + arg + "' requires a value");
              return ParseResult::kErrorOptionMissingValue;
            }
            value = args[++i];
            if (starts_with(value, "-") && value != "-") {
              print_warn(
                  "option '" + arg + "' value '" + value +
                  "' looks like an option\nuse '=value' format if intended");
            }
          }

          if (!opt->parse(value)) {
            print_error("failed to parse option: '" + arg + "'");
            return ParseResult::kErrorOptionParseFailed;
          }
        }
        break;
      }
    }
  }

  for (const auto& name : option_order_) {
    auto it = options_.find(name);
    if (it != options_.end() && it->second->is_required() &&
        !it->second->has_default() && !parsed[name]) {
      print_error("required option '--" + name + "' is missing");
      return ParseResult::kErrorRequiredOptionMissing;
    }
  }

  if (positional_args.size() <
      static_cast<std::size_t>(std::count(positional_required_.begin(),
                                          positional_required_.end(), true))) {
    print_error("not enough positional arguments provided");
    return ParseResult::kErrorPositionalMissing;
  }

  for (std::size_t i = 0;
       i < positional_args.size() && i < positional_parsers_.size(); ++i) {
    if (!positional_parsers_[i]->parse(positional_args[i])) {
      print_error("failed to parse positional argument: '" +
                  positional_args[i] + "'");
      return ParseResult::kErrorPositionalParseFailed;
    }
  }

  return ParseResult::kSuccess;
}

void ArgumentParser::print_warn(const std::string& message) const {
  std::cerr << program_name_ << ": " << kRed << kBold << "warn: " << kReset
            << kBold << message << kReset << std::endl;
}

void ArgumentParser::print_error(const std::string& message) const {
  std::cerr << program_name_ << ": " << kRed << kBold << "error: " << kReset
            << kBold << message
            << "\nUse '-h' or '--help' to show the help message" << kReset
            << std::endl;
}

void ArgumentParser::print_help() const {
  std::cout << "Usage: " << program_name_;
  for (const auto& name : option_order_) {
    const auto& opt = options_.at(name);
    std::cout << (opt->is_required() ? " " : " [") << "--" << name;
    if (opt->has_value()) {
      std::cout << "=<value>";
    }
    std::cout << (opt->is_required() ? "" : "]");
  }
  for (std::size_t i = 0; i < positional_names_.size(); ++i) {
    std::cout << (positional_required_[i] ? " " : " [") << positional_names_[i]
              << (positional_required_[i] ? "" : "]");
  }
  std::cout << "\n\n" << description_ << "\n";
  if (!positional_names_.empty()) {
    std::cout << "\nPositional arguments:\n";
    for (std::size_t i = 0; i < positional_names_.size(); ++i) {
      std::cout << "  " << std::left << std::setw(20) << positional_names_[i]
                << positional_descriptions_[i];
      if (positional_required_[i]) {
        std::cout << " (required)";
      }
      std::cout << "\n";
    }
  }
  if (!option_order_.empty()) {
    std::cout << "\nOptions:\n";
    std::unordered_map<std::string, std::vector<std::string>> reverse_aliases;
    for (const auto& pair : aliases_) {
      reverse_aliases[pair.second].push_back(pair.first);
    }
    for (const auto& name : option_order_) {
      const auto& opt = options_.at(name);
      std::string display = "--" + name;
      for (const auto& alias : reverse_aliases[name]) {
        display =
            (alias.length() == 1 ? "-" + alias + ", " : "--" + alias + ", ") +
            display;
      }
      if (opt->has_value()) {
        display += "=<value>";
      }
      std::cout << "  " << std::left << std::setw(25) << display
                << opt->description();
      if (opt->has_default()) {
        std::cout << " (default: " << opt->default_value_str() << ")";
      }
      if (opt->is_required()) {
        std::cout << " (required)";
      }
      std::cout << "\n";
    }
    std::cout << "  " << std::left << std::setw(25) << "-v, --version"
              << "Show version information\n";
    std::cout << "  " << std::left << std::setw(25) << "-h, --help"
              << "Show this help message\n";
  }
}

void ArgumentParser::print_version() const {
  std::cout << BUILD_NAME " version " BUILD_VERSION " (" BUILD_TYPE ")\n"
            << "Build Platform: " BUILD_PLATFORM " - " BUILD_ARCH << "\n"
            << "Target Platform: " TARGET_PLATFORM " - " << TARGET_ARCH "\n"
            << "Target Bits: " TARGET_BITS "\n"
            << "Build Compiler: " BUILD_COMPILER "\n"
            << "Installed Directory: " << get_exe_dir() << "\n"
            << "Build Time: " BUILD_TIME "\n"
            << "Commit Hash: " BUILD_GIT_COMMIT_HASH "\n";

  std::cout << std::flush;
}

}  // namespace core
