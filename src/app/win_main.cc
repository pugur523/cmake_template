// Copyright 2025 pugur
// All rights reserved.

#include "build/build_flag.h"

#if IS_WINDOWS

#include <windows.h>
#include "app/app.h"

// main function for /SUBSYSTEM:CONSOLE
int main(int /* argc */, char** /* argv */) {
  return app::start();
}

// main function for /SUBSYSTEM:WINDOWS
int WINAPI WinMain(HINSTANCE /* hInstance */,
                   HINSTANCE /* hPrevInstance */,
                   PSTR /* lpCmdLine */,
                   int /* nCmdShow */) {
  // MessageBoxW(
  //     NULL,
  //     L"hello, world",
  //     L"Program Started Successfully",
  //     MB_OK | MB_ICONQUESTION
  // );
  return app::start();
}

#endif  // IS_WINDOWS
