#include "core/base/source_location.h"

namespace core {

SourceLocation::SourceLocation(std::size_t line,
                               std::size_t column,
                               FileId file_id)
    : internal_vec_({line, column}), file_id_(file_id) {}

}  // namespace core
