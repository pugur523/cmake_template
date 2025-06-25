#ifndef CORE_BASE_SOURCE_RANGE_H_
#define CORE_BASE_SOURCE_RANGE_H_

#include "core/base/core_export.h"
#include "core/base/file_manager.h"
#include "core/base/source_location.h"

namespace core {

class CORE_EXPORT SourceRange {
 public:
  SourceRange(SourceLocation start, SourceLocation end);

  SourceRange(std::size_t line,
              std::size_t column,
              std::size_t length,
              FileId file_id);

  ~SourceRange() = default;

  SourceRange(const SourceRange&) = delete;
  SourceRange& operator=(const SourceRange&) = delete;

  SourceRange(SourceRange&&) noexcept = default;
  SourceRange& operator=(SourceRange&&) noexcept = default;

  inline constexpr const SourceLocation& start() { return start_; }
  inline constexpr const SourceLocation& end() { return end_; }
  inline constexpr FileId file_id() { return start_.file_id(); }

 private:
  SourceLocation start_;
  SourceLocation end_;
};

}  // namespace core

#endif  // CORE_BASE_SOURCE_RANGE_H_
