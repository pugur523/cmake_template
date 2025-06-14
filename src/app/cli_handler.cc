#include "app/cli_handler.h"

#include <iostream>
#include <string>
#include <vector>

#include "build/build_config.h"
#include "core/cli/arg_parser.h"
#include "core/diagnostics/stack_trace.h"

namespace app {

int handle_arguments(int argc, char** argv) {
  core::ArgumentParser parser(BUILD_NAME, BUILD_DESCRIPTION);

  std::string input_file;
  std::string output_file;
  int count = 1;
  std::vector<std::string> includes;
  std::string mode;
  bool verbose;
  bool stacktrace;

  parser.add_option(&input_file, "input", "input file path", true);
  parser.add_option(&output_file, "output", "output file path", true,
                    {"output.txt"});
  parser.add_option(&count, "count", "number of iterations", true, {1});
  parser.add_list(&includes, "include", "include directories list");
  parser.add_flag(&verbose, "verbose", "verbose mode", false, {false});
  parser.add_flag(&stacktrace, "stacktrace",
                  "print stacktrace for some reasons", false, {false});

  parser.add_alias("i", "input");
  parser.add_alias("o", "output");
  parser.add_alias("c", "count");
  parser.add_alias("I", "include");
  parser.add_alias("V", "verbose");
  parser.add_alias("s", "stacktrace");

  parser.add_positional(&mode, "mode", "Processing mode", true);

  if (parser.parse(argc, argv) != core::ParseResult::kSuccess) {
    return 1;
  }

  if (verbose) {
    std::cout << "Parsed arguments:" << "\n"
              << "  input: " << input_file << "\n"
              << "  output: " << output_file << "\n"
              << "  count: " << count << "\n"
              << "  mode: " << mode << "\n"
              << "  verbose: " << verbose << "\n"
              << "  stacktrace: " << stacktrace << "\n"
              << "  includes: [";
    for (std::size_t i = 0; i < includes.size(); i++) {
      std::cout << includes[i];

      if (i < includes.size() - 1) {
        std::cout << ", ";
      }
    }
    std::cout << "]";
    std::cout << std::endl;
  }

  if (stacktrace) {
    constexpr std::size_t kBufSize = 16384;
    char buf[kBufSize];
    core::stack_trace_from_current_context(buf, kBufSize);
    std::cout << buf << std::endl;
  }

  return 0;
}

}  // namespace app

