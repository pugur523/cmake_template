#ifndef CORE_BASE_SOURCE_LOCATION_H_
#define CORE_BASE_SOURCE_LOCATION_H_

#include "core/base/core_export.h"
#include "core/base/file_manager.h"
#include "core/base/vec.h"

namespace core {

class CORE_EXPORT SourceLocation {
 public:
  SourceLocation(std::size_t line, std::size_t column, FileId file_id);
  ~SourceLocation() = default;

  SourceLocation(const SourceLocation&) = delete;
  SourceLocation& operator=(const SourceLocation&) = delete;

  SourceLocation(SourceLocation&&) noexcept = default;
  SourceLocation& operator=(SourceLocation&&) noexcept = default;

  inline constexpr std::size_t line() const { return internal_vec_[0]; }
  inline constexpr std::size_t column() const { return internal_vec_[1]; }
  inline constexpr FileId file_id() const { return file_id_; }

 private:
  core::Vec2<std::size_t> internal_vec_;
  FileId file_id_;
};

}  // namespace core

#endif  // CORE_BASE_SOURCE_LOCATION_H_
