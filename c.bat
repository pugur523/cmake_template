@echo off
setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"

set "BUILD_SCRIPTS_DIR=%SCRIPT_DIR%\src\build\scripts"
set "DOCKER_DIR=%BUILD_SCRIPTS_DIR%\..\docker"
set "DEBUG_OUT_BIN_DIR=%SCRIPT_DIR%\out\build\windows\amd64\debug\bin"
set "RELEASE_OUT_BIN_DIR=%SCRIPT_DIR%\out\build\windows\amd64\release\bin"
set "PROJECT_NAME=sample_project"

set "ACTION=%~1"

if not "%ACTION%"=="" (
    shift
    set REMAIN_ARGS=%*
) else (
    set "ACTION=build"
    set "REMAIN_ARGS=--build_mode=all"
)

if /I "%ACTION%"=="build" (
    python "%BUILD_SCRIPTS_DIR%\build.py" %REMAIN_ARGS%
) else if /I "%ACTION%"=="run" (
    "%DEBUG_OUT_BIN_DIR%\%PROJECT_NAME%.exe" %REMAIN_ARGS%
) else if /I "%ACTION%"=="test" (
    "%DEBUG_OUT_BIN_DIR%\%PROJECT_NAME%_test.exe" %REMAIN_ARGS%
) else if /I "%ACTION%"=="bench" (
    "%RELEASE_OUT_BIN_DIR%\%PROJECT_NAME%_bench.exe" %REMAIN_ARGS%
) else if /I "%ACTION%"=="docker" (
    echo docker build on windows is currently not supported
) else (
    echo unknown option specified; aborted
    exit /b 1
)

exit /b 0
