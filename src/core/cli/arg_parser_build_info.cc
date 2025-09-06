#include "build/build_time_info.h"
#include "build/project_config.h"
#include "core/base/file_util.h"
#include "core/base/logger.h"
#include "core/cli/arg_parser.h"

namespace core {

// static
void ArgParser::print_version() {
  core::glog.raw_ref<
      "{} version {} ({})\n"
      "Build Platform: {} - {}\n"
      "Target Platform: {} - {}\n"
      "Target Bits: {}\n"
      "Build Compiler: {}\n"
      "Installed Directory: {}\n"
      "Build Time: {}\n"
      "Commit Hash: {}\n">(
      build::kBuildName, build::kBuildVersion, build::kBuildType,
      build::kBuildPlatform, build::kBuildArch, build::kTargetPlatform,
      build::kTargetArch, build::kTargetBits, build::kBuildCompiler, exe_dir(),
      build::kBuildTime, build::kBuildGitCommitHash);
  core::glog.flush();
}

}  // namespace core
