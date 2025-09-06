#include "core/cli/console.h"

#include <locale>

#include "build/build_flag.h"

#if IS_WINDOWS
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace core {

namespace {

bool check_ansi_sequence_available() {
#if IS_WINDOWS
  // check if GetStdHandle() returns terminal handle
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE) {
    return false;
  }

  DWORD dwMode = 0;
  if (!GetConsoleMode(hOut, &dwMode)) {
    return false;
  }

  // enable virtual terminal processing
  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  if (!SetConsoleMode(hOut, dwMode)) {
    // cannot use ansi sequence if fail to enable virtual terminal
    return false;
  }
  return true;
#else
  return isatty(STDOUT_FILENO);
#endif
}

}  // namespace

bool can_use_ansi_escape_sequence() {
  static const bool result = check_ansi_sequence_available();
  return result;
}

void register_console() {
  // call it once to initialize the result
  can_use_ansi_escape_sequence();

  // set console mode to utf8
#if IS_WINDOWS
  SetConsoleCP(CP_UTF8);
  SetConsoleOutputCP(CP_UTF8);
#endif
}

}  // namespace core
