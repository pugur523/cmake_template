#include "app/cli_handler.h"

#include <iostream>
#include <string>
#include <vector>

#include "build/build_config.h"
#include "core/base/logger.h"
#include "core/cli/arg_parser.h"
#include "core/diagnostics/stack_trace.h"
#include "core/location.h"

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
    std::string includes_str = "[";
    for (std::size_t i = 0; i < includes.size(); i++) {
      includes_str.append(includes[i]);

      if (i < includes.size() - 1) {
        includes_str.append(", ");
      }
    }
    includes_str.append("]");

    core::glog.info<R"(Parsed arguments:
        input: {}    
        output: {} 
        count: {} 
        mode: {} 
        verbose: {} 
        stacktrace: {}
        includes: {}
)">(input_file, output_file, count, mode, verbose, stacktrace, includes_str);
  }

  if (stacktrace) {
    constexpr std::size_t kBufSize = 4096;

    char location_buf[kBufSize];
    FROM_HERE.to_string(location_buf, kBufSize);

    std::string stacktrace = core::stack_trace_from_current_context();
    core::glog.info<"{}\n{}">(location_buf, stacktrace);
  }

  return 0;
}

}  // namespace app

