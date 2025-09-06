#ifndef CORE_CLI_ANSI_PROGRESS_BAR_H_
#define CORE_CLI_ANSI_PROGRESS_BAR_H_

#include <algorithm>
#include <chrono>
#include <deque>
#include <string>
#include <utility>

#include "core/base/core_export.h"
#include "core/check.h"

namespace core {

class CORE_EXPORT ProgressBar {
 public:
  explicit ProgressBar(std::size_t width = 40,
                       std::string&& in_progress_message = "Processing",
                       bool enable_color = true)
      : in_progress_message_(std::move(in_progress_message)),
        width_(width),
        start_time_(std::chrono::steady_clock::now()),
        enable_color_(enable_color) {}

  ~ProgressBar() = default;

  ProgressBar(const ProgressBar&) = delete;
  ProgressBar& operator=(const ProgressBar&) = delete;

  ProgressBar(ProgressBar&&) noexcept = default;
  ProgressBar& operator=(ProgressBar&&) noexcept = default;

  // update progress in the range from 0 to 1
  [[nodiscard]] inline std::string update(double progress,
                                          bool remove_line = false) {
    return update_impl(progress, in_progress_message_, remove_line);
  }

  [[nodiscard]] inline std::string finish(
      const std::string& msg = "Completed") {
    return update_impl(1.0, msg, true);
  }

 private:
  std::string update_impl(double progress,
                          const std::string& prefix,
                          bool remove_line);

  std::string create_bar(double progress) const;

  inline static std::string format_time(double total_seconds);

  std::string in_progress_message_;
  std::size_t width_;
  std::chrono::steady_clock::time_point start_time_;
  std::deque<double> eta_history_;
  bool enable_color_ : 1 = true;
};

}  // namespace core

#endif  // CORE_CLI_ANSI_PROGRESS_BAR_H_
