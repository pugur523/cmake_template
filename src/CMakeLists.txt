set(CMAKE_C_STANDARD ${PROJECT_C_VERSION_FROM_CONFIG})
set(CMAKE_CXX_STANDARD ${PROJECT_CXX_VERSION_FROM_CONFIG})
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(PROJECT_ROOT_DIR "${PROJECT_SOURCE_DIR}/..")
set(PROJECT_OUT_DIR "${PROJECT_ROOT_DIR}/out")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/build/cmake")
set(BUILD_RESOURCES_DIR "${PROJECT_SOURCE_DIR}/build/resources")
set(THIRD_PARTY_DIR "${PROJECT_SOURCE_DIR}/third_party")
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

option(DO_CLANG_TIDY "do clang tidy for linting" TRUE)

if(DO_CLANG_TIDY AND NOT MINGW_BUILD)
  set(CLANG_TIDY_COMMAND "clang-tidy;--config-file=${PROJECT_ROOT_DIR}/.clang-tidy;-p=${CMAKE_BINARY_DIR}")
endif()

option(BUILD_DEBUG "option for debug / release build." TRUE)

if(BUILD_DEBUG)
  set(CMAKE_BUILD_TYPE "Debug")
else()
  set(CMAKE_BUILD_TYPE "Release")
endif()

string(TOLOWER ${CMAKE_BUILD_TYPE} lower_build_type)
option(ENABLE_BUILD_SHARED "build libraries as shared" FALSE)
option(ENABLE_BUILD_PROGRAM "build standalone executable" TRUE)
option(ENABLE_BUILD_BENCHMARK "build benchmark module" TRUE)
option(ENABLE_BUILD_TESTING "build testing module" TRUE)
option(ENABLE_INSTALL_PROGRAM "install standalone executable" TRUE)
option(ENABLE_INSTALL_LIBS "install libraries" TRUE)
option(ENABLE_INSTALL_BENCHMARK "install benchmark module" FALSE)
option(ENABLE_INSTALL_TESTING "install testing module" FALSE)
option(ENABLE_RUN_PROGRAM_POST_BUILD "enable running program post build" FALSE)
option(ENABLE_RUN_BENCHMARK_POST_BUILD "enable running benchmarks post build" FALSE)
option(ENABLE_RUN_TESTING_POST_BUILD "enable running tests post build" FALSE)

string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} lower_arch)
string(TOLOWER ${CMAKE_SYSTEM_NAME} lower_os_name)

set(TARGET_ARCH_NAME ${lower_arch} CACHE STRING "target architecture name in lower case")
set(TARGET_OS_NAME ${lower_os_name} CACHE STRING "target os name in lower case")

string(TOLOWER ${CMAKE_HOST_SYSTEM_PROCESSOR} HOST_ARCH_NAME)
string(TOLOWER ${CMAKE_HOST_SYSTEM_NAME} HOST_OS_NAME)

option(ENABLE_VERBOSE "enable detailed build information" TRUE)
option(ENABLE_LTO "enable link time optimization" TRUE)
option(ENABLE_NATIVE_ARCH "enable optimization for native architecture" TRUE)

option(ENABLE_BUILD_REPORT "generate the report for optimization, build time, etc." TRUE)
option(ENABLE_COVERAGE "enable coverage using -fprofile-instr-generate and llvm-cov. runs main app and test on post build if available." FALSE)
option(ENABLE_OPTIMIZATION_REPORT "generate optimization report" FALSE)
option(ENABLE_XRAY "enable llvm-xray. not compatible with windows build, debug build, address sanitizers. runs main app and test on post build if available." FALSE)
option(ENABLE_SANITIZERS "enable address and undefined sanitizers" FALSE)
option(ENABLE_LLVM_UNWIND "enable llvm libunwind to fetch stacktrace" FALSE)
option(ENABLE_AVX2 "enable avx2 if available" TRUE)

option(ENABLE_WARNINGS_AS_ERRORS "treat warnings as errors" FALSE)

if(ENABLE_OPTIMIZATION_REPORT AND HOST_OS_NAME MATCHES "windows")
  message(WARNING "ENABLE_OPTIMIZATION_REPORT on Windows build is not supported.\n"
    "set ENABLE_OPTIMIZATION_REPORT to false forcely.")
  set(ENABLE_OPTIMIZATION_REPORT FALSE)
endif()

if(ENABLE_COVERAGE AND MINGW_BUILD)
  message(WARNING "ENABLE_COVERAGE for MinGW build is not supported.\n"
    "set ENABLE_COVERAGE to false forcely.")
  set(ENABLE_COVERAGE FALSE)
endif()

if(ENABLE_XRAY AND MINGW_BUILD)
  message(WARNING "ENABLE_XRAY for MinGW build is not supported.\n"
    "set ENABLE_XRAY to false forcely.")
  set(ENABLE_XRAY FALSE)
endif()

if(ENABLE_XRAY AND TARGET_OS_NAME MATCHES "windows")
  message(WARNING "ENABLE_XRAY for Windows build is not supported.\n"
    "set ENABLE_XRAY to false forcely.")
  set(ENABLE_XRAY FALSE)
endif()

if(ENABLE_LLVM_UNWIND AND TARGET_OS_NAME MATCHES "windows")
  message(WARNING "ENABLE_LLVM_UNWIND for Windows build is not supported.\n"
    "set ENABLE_LLVM_UNWIND to false forcely.")
  set(ENABLE_LLVM_UNWIND FALSE)
endif()

if(ENABLE_AVX2 AND TARGET_OS_NAME MATCHES "darwin")
  message(WARNING "ENABLE_AVX2 for Darwin build is not supported.\n"
    "set ENABLE_AVX2 to false forcely.")
  set(ENABLE_AVX2 FALSE)
endif()

if(ENABLE_XRAY AND ENABLE_SANITIZERS)
  message(WARNING "ENABLE_XRAY and ENABLE_SANITIZERS cannot be enabled at the same time.\n"
    "set ENABLE_SANITIZERS to false forcely.")
  set(ENABLE_SANITIZERS FALSE)
endif()

if(ENABLE_LTO AND BUILD_DEBUG)
  message(WARNING "ENABLE_LTO in Debug build is not recommended.\n"
    "set ENABLE_LTO to false forcely.")
  set(ENABLE_LTO FALSE)
endif()

if(ENABLE_XRAY AND NOT BUILD_DEBUG)
  message(WARNING "ENABLE_XRAY in Release build is not recommended.\n"
    "set ENABLE_XRAY to false forcely.")
  set(ENABLE_XRAY FALSE)
endif()

if(ENABLE_COVERAGE AND NOT BUILD_DEBUG)
  message(WARNING "ENABLE_COVERAGE in Release build is not recommended.\n"
    "set ENABLE_COVERAGE to false forcely.")
  set(ENABLE_COVERAGE FALSE)
endif()

set(ADDITIONAL_INCLUDE_DIRECTORIES "" CACHE STRING "additional include directories to apply whole the project")
set(ADDITIONAL_LINK_DIRECTORIES "" CACHE STRING "additional link directories to apply whole the project")
set(ADDITIONAL_LINK_LIBRARIES "" CACHE STRING "additional link libraries to apply whole the project")

include(utils)

include(flags)
setup_flags()

include(libraries)
setup_zlib()
setup_llvm()
setup_toml11()
setup_femtolog()

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

list(APPEND PROJECT_INCLUDE_DIRECTORIES
  ${PROJECT_SOURCE_DIR}
  ${LLVM_INCLUDE_DIRS}
  ${TOML11_INCLUDE_DIR}
  ${ZLIB_INCLUDE_DIR}
  ${FEMTOLOG_INCLUDE_DIR}
  ${ADDITIONAL_INCLUDE_DIRECTORIES}
  ${GTEST_INCLUDE_DIR}
  ${GMOCK_INCLUDE_DIR}
  ${GOOGLE_BENCHMARK_INCLUDE_DIR}
)

list(APPEND PROJECT_LINK_DIRECTORIES
  ${LLVM_LIBRARY_DIRS}
  ${ADDITIONAL_LINK_DIRECTORIES}
)

list(APPEND PROJECT_LINK_LIBRARIES
  ${MINGW_LINK_LIBRARIES}
  ${WINDOWS_LINK_LIBRARIES}
  ${FEMTOLOG_LIBRARIES}
  ${ADDITIONAL_LINK_LIBRARIES}
)

if(ENABLE_VERBOSE)
  message("${Green}
-=-=-=-=-=-= CMake build status -=-=-=-=-=-=-
cmake version : ${CMAKE_VERSION}
project name: ${CMAKE_PROJECT_NAME}, project version: ${CMAKE_PROJECT_VERSION}
main executable name: ${MAIN_EXECUTABLE_NAME_FROM_CONFIG}
c language version: ${CMAKE_C_STANDARD}, c++ language version: ${CMAKE_CXX_STANDARD}
c compiler: ${CMAKE_C_COMPILER} (ver: ${CMAKE_C_COMPILER_VERSION})
c++ compiler: ${CMAKE_CXX_COMPILER} (ver: ${CMAKE_CXX_COMPILER_VERSION})
build type: ${CMAKE_BUILD_TYPE}
host os name: ${HOST_OS_NAME}
host arch name: ${HOST_ARCH_NAME}
target os name: ${TARGET_OS_NAME}
target arch name: ${TARGET_ARCH_NAME}
project include directories: ${PROJECT_INCLUDE_DIRECTORIES}
project compile options: ${PROJECT_COMPILE_OPTIONS}
project compile definitions: ${PROJECT_COMPILE_DEFINITIONS}
project link options: ${PROJECT_LINK_OPTIONS}
project link directories: ${PROJECT_LINK_DIRECTORIES}
project link libraries: ${PROJECT_LINK_LIBRARIES}
install prefix: ${CMAKE_INSTALL_PREFIX}
build resources dir: ${BUILD_RESOURCES_DIR}
is mingw build: ${MINGW_BUILD}
toolchain file: ${CMAKE_TOOLCHAIN_FILE}
build debug: ${BUILD_DEBUG}
enable build shared: ${ENABLE_BUILD_SHARED}
enable build program: ${ENABLE_BUILD_PROGRAM}
enable build benchmark: ${ENABLE_BUILD_BENCHMARK}
enable build testing: ${ENABLE_BUILD_TESTING}
enable install program: ${ENABLE_INSTALL_PROGRAM}
enable install libs: ${ENABLE_INSTALL_LIBS}
enable install benchmark: ${ENABLE_INSTALL_BENCHMARK}
enable install testing: ${ENABLE_INSTALL_TESTING}
enable run program post build: ${ENABLE_RUN_PROGRAM_POST_BUILD}
enable run benchmark post build: ${ENABLE_RUN_BENCHMARK_POST_BUILD}
enable run testing post build: ${ENABLE_RUN_TESTING_POST_BUILD}
enable verbose: ${ENABLE_VERBOSE}
enable lto: ${ENABLE_LTO}
enable native arch: ${ENABLE_NATIVE_ARCH}
enable build report: ${ENABLE_BUILD_REPORT}
enable profile: ${ENABLE_COVERAGE}
enable optimization report: ${ENABLE_OPTIMIZATION_REPORT}
enable xray: ${ENABLE_XRAY}
enable sanitizers: ${ENABLE_SANITIZERS}
enable llvm unwind: ${ENABLE_LLVM_UNWIND}
enable avx2: ${ENABLE_AVX2}
enable warnings as errors: ${ENABLE_WARNINGS_AS_ERRORS}
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-${ColourReset}")
  print_all_build_flags()
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/build)

if(ENABLE_BUILD_PROGRAM)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core)

if(ENABLE_BUILD_TESTING)
  setup_gtest()
  enable_testing()
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/testing)
endif()

if(ENABLE_BUILD_BENCHMARK)
  setup_google_benchmark()
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/bench)
endif()
