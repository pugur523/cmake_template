#include "build/build_flag.h"

#if IS_WINDOWS

// clang-format off
#include <windows.h>
#include <consoleapi.h>
#include <shellapi.h>
// clang-format on

#include <cstdio>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "app/app.h"

// main function for /SUBSYSTEM:CONSOLE
int main(int argc, char** argv) {
  return app::start(argc, argv);
}

// main function for /SUBSYSTEM:WINDOWS
int WINAPI WinMain(HINSTANCE /* hInstance */,
                   HINSTANCE /* hPrevInstance */,
                   PSTR /* lpCmdLine */,
                   int /* nCmdShow */) {
  int argc_wide;
  LPWSTR* argv_wide = CommandLineToArgvW(GetCommandLineW(), &argc_wide);
  if (!argv_wide) {
    return 1;
  }

  std::vector<std::string> args_utf8;
  std::vector<char*> argv_utf8;

  args_utf8.reserve(argc_wide);
  argv_utf8.reserve(argc_wide);
  for (int i = 0; i < argc_wide; ++i) {
    // UTF-16 (wchar_t*) → UTF-8 (char*)
    int required_size = WideCharToMultiByte(CP_UTF8, 0, argv_wide[i], -1,
                                            nullptr, 0, nullptr, nullptr);
    if (required_size > 0) {
      std::string utf8_arg(required_size - 1,
                           0);  // -1 because it includes null terminator
      WideCharToMultiByte(CP_UTF8, 0, argv_wide[i], -1, utf8_arg.data(),
                          required_size, nullptr, nullptr);
      args_utf8.push_back(std::move(utf8_arg));
    }
  }

  for (auto& s : args_utf8) {
    argv_utf8.push_back(const_cast<char*>(s.c_str()));
  }

  int argc = static_cast<int>(argv_utf8.size());
  char** argv = argv_utf8.data();

  if (AttachConsole(ATTACH_PARENT_PROCESS)) {
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    std::ios::sync_with_stdio(true);
    std::wcout.clear();
    std::cout.clear();
    std::wcerr.clear();
    std::cerr.clear();
    std::wcin.clear();
    std::cin.clear();
  }

  int result = app::start(argc, argv);

  std::cout << std::flush;
  std::cerr << std::flush;
  FreeConsole();
  fclose(stdout);
  fclose(stderr);

  return result;
}

#endif  // IS_WINDOWS
