#include "core/base/source_range.h"

#include <utility>

namespace core {

SourceRange::SourceRange(SourceLocation start, SourceLocation end)
    : start_(std::move(start)), end_(std::move(end)) {}

SourceRange::SourceRange(std::size_t line,
                         std::size_t column,
                         std::size_t length,
                         FileId file_id)
    : SourceRange(SourceLocation(line, column, file_id),
                  SourceLocation(line, column + length, file_id)) {}

}  // namespace core
