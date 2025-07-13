#include "core/cli/arg_parser.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "build/build_config.h"
#include "core/base/file_util.h"
#include "core/base/logger.h"
#include "core/base/string_util.h"

namespace core {

namespace {

enum class ArgType : uint8_t {
  kLong = 0,
  kShort = 1,
  kDoubleDash = 2,
  kLoneDash = 3,
  kPositional = 4,
  kInvalid = 5,
};

ArgType classify_argument(const std::string& arg) {
  if (arg == "--") {
    return ArgType::kDoubleDash;
  }
  if (arg == "-") {
    return ArgType::kLoneDash;
  }
  if (starts_with(arg, "--")) {
    return arg.length() > 2 ? ArgType::kLong : ArgType::kInvalid;
  }
  if (starts_with(arg, "-")) {
    return arg.length() > 1 ? ArgType::kShort : ArgType::kInvalid;
  }
  return ArgType::kPositional;
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
      case ArgType::kDoubleDash:
        positional_args.insert(positional_args.end(), args.begin() + i + 1,
                               args.end());
        i = args.size();
        break;
      case ArgType::kLoneDash:
      case ArgType::kPositional: positional_args.push_back(arg); break;
      case ArgType::kInvalid:
        print_error("invalid option format: '" + arg + "'");
        return ParseResult::kErrorInvalidFormat;
      case ArgType::kLong:
      case ArgType::kShort: {
        std::string optname, value;
        bool has_value = false;

        if (type == ArgType::kLong) {
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
                  "option '" + arg + "' value '" += value +=
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

  if (positional_args.size() > positional_parsers_.size()) {
    print_error("too many positional arguments provided.");
    return ParseResult::kErrorPositionalTooMany;
  }

  return ParseResult::kSuccess;
}

void ArgumentParser::print_warn(const std::string& message) const {
  glog.warn<"{}{}{}\n">(kBold, message, kReset);
}

void ArgumentParser::print_error(const std::string& message) const {
  glog.error<
      "{}{}{}\n"
      "Use '-h' or '--help' to show the help "
      "message\n">(kBold, message, kReset);
}

void ArgumentParser::print_help() const {
  std::string help_str = "";

  std::string usage_str = "Usage: ";
  usage_str.append(program_name_);
  for (const auto& name : option_order_) {
    const auto& opt = options_.at(name);
    usage_str.append(opt->is_required() ? " --" : " [--");
    usage_str.append(name);
    if (opt->has_value()) {
      usage_str.append("=<value>");
    }
    if (!opt->is_required()) {
      usage_str.push_back(']');
    }
  }
  for (std::size_t i = 0; i < positional_names_.size(); i++) {
    if (positional_required_[i]) {
      usage_str.push_back(' ');
      usage_str.append(positional_names_[i]);
    } else {
      usage_str.append(" [");
      usage_str.append(positional_names_[i]);
      usage_str.push_back(']');
    }
  }
  help_str.append(usage_str);
  help_str.append("\n\n");
  help_str.append(description_);
  help_str.push_back('\n');

  if (!positional_names_.empty()) {
    std::string positional_str = "Positional arguments:\n";
    for (std::size_t i = 0; i < positional_names_.size(); ++i) {
      constexpr std::size_t kPositionalArgStrSize = 128;
      std::string positional_buf;
      positional_buf.reserve(kPositionalArgStrSize);
      positional_buf.append("  ");
      positional_buf.append(positional_names_[i]);

      const std::size_t padding_size = 25 - positional_names_[i].size();
      positional_buf.append(std::string(padding_size, ' '));

      positional_buf.append(positional_descriptions_[i]);
      if (positional_required_[i]) {
        positional_buf.append(" (required)");
      }
      positional_buf.push_back('\n');
      positional_str.append(positional_buf);
    }
    help_str.push_back('\n');
    help_str.append(positional_str);
  }
  if (!option_order_.empty()) {
    std::string options_str = "Options:\n";
    std::unordered_map<std::string, std::vector<std::string>> reverse_aliases;
    for (const auto& pair : aliases_) {
      reverse_aliases[pair.second].push_back(pair.first);
    }
    for (const auto& name : option_order_) {
      const auto& opt = options_.at(name);
      std::string display;
      display.reserve(reverse_aliases[name].size() * 12);
      bool first = true;
      for (int i = reverse_aliases[name].size() - 1; i >= 0; i--) {
        const std::string alias = reverse_aliases[name][i];
        if (first) {
          first = false;
        } else {
          display.append(", ");
        }

        if (alias.length() == 1) {
          display.append("-" + alias);
        } else {
          display.append("--" + alias);
        }
      }
      if (!first) {
        display.append(", ");
      }
      display.append("--" + name);

      if (opt->has_value()) {
        display.append("=<value>");
      }

      constexpr std::size_t kOptionsArgStrSize = 128;
      std::string options_buf;
      options_buf.reserve(kOptionsArgStrSize);
      options_buf.append("  ");
      options_buf.append(display);

      const std::size_t padding_size = 25 - display.size();
      options_buf.append(std::string(padding_size, ' '));

      options_buf.append(opt->description());

      if (opt->has_default()) {
        options_buf.append(" (default: ");
        options_buf.append(opt->default_value_str());
        options_buf.append(")");
      }

      if (opt->is_required()) {
        options_buf.append(" (required)");
      }

      options_buf.push_back('\n');
      options_str.append(options_buf);
    }
    help_str.push_back('\n');
    help_str.append(options_str);

    {
      help_str.append("  ");
      const std::string v_str = "-v, --version";
      help_str.append(v_str);
      const std::size_t v_padding_size = 25 - v_str.size();
      help_str.append(std::string(v_padding_size, ' '));
      help_str.append("Show version information\n");

      help_str.append("  ");
      const std::string h_str = "-h, --help";
      help_str.append(h_str);
      const std::size_t h_padding_size = 25 - h_str.size();
      help_str.append(std::string(h_padding_size, ' '));
      help_str.append("Show this help message\n");
    }
  }
  core::glog.info<"{}">(help_str);
}

// static
void ArgumentParser::print_version() {
  core::glog.info<R"(
    {} version {} ({})
    Build Platform: {} - {}
    Target Platform: {} - {}
    Target Bits: {}
    Build Compiler: {}
    Installed Directory: {}
    Build Time: {}
    Commit Hash: {}
)">(BUILD_NAME, BUILD_VERSION, BUILD_TYPE, BUILD_PLATFORM, BUILD_ARCH,
    TARGET_PLATFORM, TARGET_ARCH, TARGET_BITS, BUILD_COMPILER, exe_dir(),
    BUILD_TIME, BUILD_GIT_COMMIT_HASH);
}

}  // namespace core
