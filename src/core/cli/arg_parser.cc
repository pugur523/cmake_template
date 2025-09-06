#include "core/cli/arg_parser.h"

#include <iomanip>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/base/file_util.h"
#include "core/base/logger.h"
#include "core/base/string_util.h"
#include "core/cli/ansi/style_builder.h"
#include "core/cli/ansi/style_util.h"

namespace core {

namespace {

enum class ArgType : uint8_t {
  kLong = 0,
  kShort = 1,
  kDoubleDash = 2,
  kLoneDash = 3,
  kPositional = 4,
  kInvalid = 5,

  kLongDashWithSingleCharacter = 6,
  kShortDashWithMultipleLetters = 7,
};

ArgType classify_argument(const std::string& arg) {
  if (arg == "--") {
    return ArgType::kDoubleDash;
  }
  if (arg == "-") {
    return ArgType::kLoneDash;
  }
  if (starts_with(arg, "--")) {
    // "--debug" 7 letters word including 2 dashes
    // "--d" 3 letters word including 2 dashes which is invalid
    return arg.length() > 3 ? ArgType::kLong
                            : ArgType::kLongDashWithSingleCharacter;
  }
  if (starts_with(arg, "-")) {
    // "-a": 2 letters word including 1 dash
    return arg.length() == 2 ? ArgType::kShort
                             : ArgType::kShortDashWithMultipleLetters;
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

ArgParser::ArgParser(const std::string& program_name,
                     const std::string& description)
    : program_name_(program_name), description_(description) {}

ArgParser::~ArgParser() = default;

void ArgParser::add_alias(const std::string& alias,
                          const std::string& option_name) {
  aliases_[alias] = option_name;
}

std::string ArgParser::resolve_alias(const std::string& name) const {
  auto it = aliases_.find(name);
  return it != aliases_.end() ? it->second : name;
}

ParseResult ArgParser::parse(int argc, char** argv) {
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
      case ArgType::kShortDashWithMultipleLetters:
        print_error("short dash with multiple letters argument is invalid: '" +
                    arg + "'");
        return ParseResult::kErrorInvalidFormat;
      case ArgType::kLongDashWithSingleCharacter:
        print_error("long dash with single character argument is invalid: '" +
                    arg + "'");
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

inline void ArgParser::print_warn(const std::string& message) const {
  glog.warn_ref<"{}\n">(message);
  glog.flush();
}

inline void ArgParser::print_error(const std::string& message) const {
  glog.error_ref<
      "{}\n"
      "use '-h' or '--help' "
      "to show the help message\n">(message);
  glog.flush();
}

void ArgParser::print_help() const {
  StyleBuilder strong_style;
  StyleBuilder category_style;
  StyleBuilder arg_style;
  StyleBuilder desc_style;

  strong_style.style(Style::kBold).rgb(255, 100, 32);
  category_style.style(Style::kBoldUnderline);
  arg_style.style(Style::kBold).color(Color::kBrightCyan);
  desc_style.style(Style::kBold).color(Color::kBrightMagenta);

  std::string help_str = desc_style.build(description_);
  help_str.append("\n\n");

  std::string usage_line = category_style.build("Usage:");
  usage_line.append(3, ' ');
  usage_line.append(strong_style.build(program_name_));

  std::string buf;
  constexpr const std::size_t kBufSize = 256;  // heuristic value
  buf.reserve(kBufSize);

  // options usage summary
  for (const auto& name : option_order_) {
    const auto& opt = options_.at(name);
    buf.append(opt->is_required() ? " --" : " [--");
    buf.append(name);
    if (opt->has_value()) {
      buf.append("=<value>");
    }
    if (!opt->is_required()) {
      buf.push_back(']');
    }
    usage_line.append(arg_style.build(buf));
    buf.clear();
  }

  // positionals usage summary
  for (std::size_t i = 0; i < positional_names_.size(); ++i) {
    if (positional_required_[i]) {
      buf.push_back(' ');
      buf.append(positional_names_[i]);
    } else {
      buf.append(" [");
      buf.append(positional_names_[i]);
      buf.push_back(']');
    }
    usage_line.append(arg_style.build(buf));
    buf.clear();
  }

  help_str.append(usage_line);
  help_str.append("\n\n");

  // positionals list
  if (!positional_names_.empty()) {
    std::string positional_str =
        category_style.build("Positional arguments:\n");
    for (std::size_t i = 0; i < positional_names_.size(); ++i) {
      buf.append("  ");
      buf.append(arg_style.build(positional_names_[i]));

      const std::size_t padding_size = 25 - positional_names_[i].size();
      buf.append(std::string(padding_size, ' '));

      buf.append(positional_descriptions_[i]);
      if (positional_required_[i]) {
        buf.append(" (required)");
      }
      buf.push_back('\n');
      positional_str.append(buf);
      buf.clear();
    }
    help_str.append(positional_str);
  }

  // options list
  if (!option_order_.empty()) {
    std::string options_str = category_style.build("Options:\n");
    std::unordered_map<std::string, std::vector<std::string>> reverse_aliases;

    for (const auto& pair : aliases_) {
      reverse_aliases[pair.second].push_back(pair.first);
    }

    for (const auto& name : option_order_) {
      const auto& opt = options_.at(name);

      // name
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

      buf.append("  ");
      buf.append(arg_style.build(display));

      constexpr std::size_t kPadFormatSize = 25;
      const std::size_t padding_size = kPadFormatSize - display.size();
      buf.append(padding_size, ' ');

      // description
      buf.append(opt->description());

      if (opt->has_default()) {
        buf.append(" (default: ");
        buf.append(opt->default_value_str());
        buf.append(")");
      }

      if (opt->is_required()) {
        buf.append(" (required)");
      }

      buf.push_back('\n');
      options_str.append(buf);
      buf.clear();
    }
    options_str.append("  ");
    const std::string_view v_str = "-v, --version";
    options_str.append(arg_style.build(v_str));  // name
    const std::size_t v_padding_size = 25 - v_str.size();
    options_str.append(std::string(v_padding_size, ' '));
    options_str.append("Show version information\n");  // desc

    options_str.append("  ");
    const std::string_view h_str = "-h, --help";  // name
    options_str.append(arg_style.build(h_str));
    const std::size_t h_padding_size = 25 - h_str.size();
    options_str.append(std::string(h_padding_size, ' '));
    options_str.append("Show this help message\n");  // desc

    help_str.push_back('\n');
    help_str.append(options_str);
  }
  core::glog.raw_ref<"{}">(help_str);
  core::glog.flush();
}

}  // namespace core
