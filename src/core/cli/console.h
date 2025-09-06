#ifndef CORE_CLI_CONSOLE_H_
#define CORE_CLI_CONSOLE_H_

#include "core/base/core_export.h"

namespace core {

CORE_EXPORT bool can_use_ansi_escape_sequence();

CORE_EXPORT void register_console();

}  // namespace core

#endif  // CORE_CLI_CONSOLE_H_
