#include "core/cli/ansi/progress_bar.h"

#include <algorithm>
#include <chrono>
#include <format>
#include <string>

#include "core/cli/ansi/style_builder.h"
#include "core/cli/ansi/style_util.h"
#include "core/cli/console.h"

namespace core {

std::string ProgressBar::update_impl(double progress,
                                     const std::string& prefix,
                                     bool remove_line) {
  using duration = std::chrono::duration<double>;

  progress = std::clamp(progress, 0.0, 1.0);

  const auto now = std::chrono::steady_clock::now();
  const duration elapsed = now - start_time_;
  const double elapsed_sec =
      std::chrono::duration_cast<duration>(elapsed).count();

  // estimate the remaining time
  double eta_sec = 0.0;
  if (progress > 0.0 && progress < 1.0) {
    const double est = elapsed_sec * (1.0 - progress) / progress;
    eta_history_.push_back(est);
    if (eta_history_.size() > 10) {
      eta_history_.pop_front();
    }
    double sum = 0.0;
    for (double v : eta_history_) {
      sum += v;
    }
    eta_sec = sum / eta_history_.size();
  }

  std::string bar = create_bar(progress);

  // prefix: [ progress_bar ] n% (elapsed: 1:23 eta: 4:56)
  if (enable_color_ && can_use_ansi_escape_sequence()) {
    constexpr const char* kFormatStr =
        "{}\r{:<50} {}[ {} ]{} {:>6.2f}%    "
        "({}elapsed: {}{} | {}eta: {}{})";
    return std::format(kFormatStr, remove_line ? "\033[2K" : "", prefix,
                       color_str(Color::kBrightGreen), bar,
                       style_str(Style::kReset), progress * 100.0,
                       color_str(Color::kBrightMagenta),
                       format_time(elapsed_sec), style_str(Style::kReset),
                       color_str(Color::kBrightCyan),
                       progress == 1.0 ? "--:--" : format_time(eta_sec),
                       style_str(Style::kReset));
  } else {
    constexpr const char* kFormatStr =
        "{:<40} [ {} ] {:>6.2f}%    "
        "(elapsed: {} | eta: {})";
    return std::format(kFormatStr, prefix, bar, progress * 100.0,
                       format_time(elapsed_sec),
                       progress == 1.0 ? "--:--" : format_time(eta_sec));
  }
}

std::string ProgressBar::create_bar(double progress) const {
  std::size_t total_blocks = width_;
  double scaled = progress * total_blocks;
  std::size_t full = static_cast<std::size_t>(scaled);
  double frac = scaled - full;

  // utf-8
  std::string bar;
  bar.reserve(total_blocks * sizeof(char32_t));
  for (std::size_t i = 0; i < full; ++i) {
    bar.append("█");
  }
  if (full < total_blocks) {
    if (frac >= 0.75) {
      bar.append("▓");
    } else if (frac >= 0.5) {
      bar.append("▒");
    } else if (frac >= 0.25) {
      bar.append("░");
    } else {
      bar.append(" ");
    }
  }
  for (std::size_t i = full + 1; i < total_blocks; ++i) {
    bar.append(" ");
  }
  return bar;
}

// static
inline std::string ProgressBar::format_time(double total_seconds) {
  const auto total_secondsl = static_cast<uint64_t>(total_seconds);
  const auto hours = total_secondsl / 3600;
  const auto minutes = (total_secondsl % 3600) / 60;
  const auto seconds = total_secondsl % 60;

  std::string result;
  if (hours > 0) {
    result.append(std::to_string(hours));
    result.append("h ");
  }
  if (minutes > 0 || hours > 0) {
    result.append(std::to_string(minutes));
    result.append("m ");
  }
  result.append(std::to_string(seconds));
  result.append("s");

  return result;
}

}  // namespace core
