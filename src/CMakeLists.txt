# Copyright 2025 pugur
# All rights reserved.

set(CMAKE_C_STANDARD ${PROJECT_C_VERSION_FROM_CONFIG})
set(CMAKE_CXX_STANDARD ${PROJECT_CXX_VERSION_FROM_CONFIG})
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

set(PROJECT_ROOT_DIR "${CMAKE_SOURCE_DIR}")
set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build/cmake")
set(BUILD_RESOURCES_DIR "${PROJECT_SOURCE_DIR}/build/resources")
set(THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party")
set(THIRD_PARTY_INSTALL_DIR "${THIRD_PARTY_DIR}/install")
set(CMAKE_INSTALL_RPATH "$ORIGIN:$ORIGIN/../lib")
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_BUILD_RPATH "${CMAKE_BINARY_DIR}:${CMAKE_BINARY_DIR}/lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_DEBUG_POSTFIX "d")

set(MINGW_BUILD FALSE)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(MINGW_BUILD TRUE)
endif()

option(DO_CLANG_TIDY "whether do clang tidy or not" TRUE)

if(DO_CLANG_TIDY)
  set(CLANG_TIDY_COMMAND "clang-tidy;--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy;-p=${CMAKE_BINARY_DIR}")
endif()

option(BUILD_DEBUG "option for debug / release build." TRUE)

if(BUILD_DEBUG)
  set(CMAKE_BUILD_TYPE "Debug")
else()
  set(CMAKE_BUILD_TYPE "Release")
endif()

string(TOLOWER ${CMAKE_BUILD_TYPE} lower_build_type)
option(BUILD_SHARED "build .so/.dll/.dylib - shared libraries" TRUE)
option(BUILD_TESTING "whether build testing module or not" TRUE)

string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} lower_arch)
string(TOLOWER ${CMAKE_SYSTEM_NAME} lower_os_name)

set(TARGET_ARCH_NAME ${lower_arch} CACHE STRING "target architecture name in lower case")
set(TARGET_OS_NAME ${lower_os_name} CACHE STRING "target os name in lower case")

option(ENABLE_LTO "enable link time optimization" TRUE)
option(ENABLE_NATIVE_ARCH "enable optimization for native architecture" TRUE)

option(ENABLE_BUILD_REPORT "generate the report for optimization, build time, etc." TRUE)
option(ENABLE_PROFILE "enable profile using -fprofile-instr-generate and llvm-cov. runs main app and test on post build if available." FALSE)
option(ENABLE_OPTIMIZATION_REPORT "generate optimization report" FALSE)
option(ENABLE_XRAY "enable llvm-xray. not compatible with windows build, debug build, address sanitizers" FALSE)
option(ENABLE_SANITIZERS "enable address and undefined sanitizers" TRUE)

option(ENABLE_RUN_APP_POST_BUILD "enable running app post build" FALSE)
option(ENABLE_RUN_TESTS_POST_BUILD "enable running tests post build" TRUE)

if(ENABLE_XRAY AND MINGW_BUILD)
  message(WARNING "ENABLE_XRAY for mingw build is not supported.")
  message(WARNING "set ENABLE_XRAY to false forcely.")
  set(ENABLE_XRAY FALSE)
endif()

if(ENABLE_XRAY AND TARGET_OS_NAME MATCHES "windows")
  message(WARNING "ENABLE_XRAY for windows build is not supported.")
  message(WARNING "set ENABLE_XRAY to false forcely.")
  set(ENABLE_XRAY FALSE)
endif()

if(ENABLE_XRAY AND ENABLE_SANITIZERS)
  message(WARNING "ENABLE_XRAY and ENABLE_SANITIZERS cannot be enabled at the same time.")
  message(WARNING "set ENABLE_SANITIZERS to false forcely.")
  set(ENABLE_SANITIZERS FALSE)
endif()

option(WARNINGS_AS_ERRORS "treat warnings as errors" TRUE)

set(ADDITIONAL_INCLUDE_DIRECTORIES "" CACHE STRING "additional include directories to apply whole the project")
set(ADDITIONAL_LINK_DIRECTORIES "" CACHE STRING "additional link directories to apply whole the project")
set(ADDITIONAL_LINK_LIBRARIES "" CACHE STRING "additional link libraries to apply whole the project")

include(utils)

include(flags)
setup_flags()

include(libraries)
setup_llvm()
setup_toml11()
setup_zlib()

include(package)
setup_package()

include(setup_module)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

file(REMOVE_RECURSE ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
file(REMOVE_RECURSE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
file(REMOVE_RECURSE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(REMOVE_RECURSE ${CMAKE_PDB_OUTPUT_DIRECTORY})

set(PROJECT_INCLUDE_DIRECTORIES
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${LLVM_INCLUDE_DIRS}
  ${TOML11_INCLUDE_DIR}
  ${ZLIB_INCLUDE_DIR}
  ${ADDITIONAL_INCLUDE_DIRECTORIES}
)

set(PROJECT_LINK_DIRECTORIES
  ${LLVM_LIBRARY_DIRS}
  ${ADDITIONAL_LINK_DIRECTORIES}
)

set(PROJECT_LINK_LIBRARIES
  ${LLVM_LINK_LIBRARIES}
  ${ADDITIONAL_LINK_LIBRARIES}
)

message("${Green}
-=-=-=-=-=-= CMake build status -=-=-=-=-=-=-
cmake version : ${CMAKE_VERSION}
project name: ${CMAKE_PROJECT_NAME}, project version: ${CMAKE_PROJECT_VERSION}
main executable name: ${MAIN_EXECUTABLE_NAME_FROM_CONFIG}
c language version: ${CMAKE_C_STANDARD}, c++ language version: ${CMAKE_CXX_STANDARD}
c compiler: ${CMAKE_C_COMPILER} (ver: ${CMAKE_C_COMPILER_VERSION})
c++ compiler: ${CMAKE_CXX_COMPILER} (ver: ${CMAKE_CXX_COMPILER_VERSION})
build type: ${CMAKE_BUILD_TYPE}
host system name: ${CMAKE_HOST_SYSTEM_NAME}
target system name: ${CMAKE_SYSTEM_NAME}
target arch: ${CMAKE_SYSTEM_PROCESSOR}
include directories: ${PROJECT_INCLUDE_DIRECTORIES}
link directories: ${PROJECT_LINK_DIRECTORIES}
link libraries: ${PROJECT_LINK_LIBRARIES}
project compile options: ${PROJECT_COMPILE_OPTIONS}
project link options: ${PROJECT_LINK_OPTIONS}
install prefix: ${CMAKE_INSTALL_PREFIX}
build resources dir: ${BUILD_RESOURCES_DIR}
is mingw build: ${MINGW_BUILD}
toolchain file: ${CMAKE_TOOLCHAIN_FILE}
build debug: ${BUILD_DEBUG}
build shared: ${BUILD_SHARED}
build testing: ${BUILD_TESTING}
enable lto: ${ENABLE_LTO}
enable native arch: ${ENABLE_NATIVE_ARCH}
generate build report: ${ENABLE_BUILD_REPORT}
enable profile: ${ENABLE_PROFILE}
enable optimization report: ${ENABLE_OPTIMIZATION_REPORT}
enable xray: ${ENABLE_XRAY}
enable sanitizers: ${ENABLE_SANITIZERS}
enable run app post build: ${ENABLE_RUN_APP_POST_BUILD}
enable run tests post build: ${ENABLE_RUN_TESTS_POST_BUILD}
warnings as errors: ${WARNINGS_AS_ERRORS}
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-${ColourReset}")
print_all_build_flags()

add_subdirectory(app)
add_subdirectory(core)

if(BUILD_TESTING)
  setup_gtest()
  enable_testing()
  add_subdirectory(testing)
endif()
