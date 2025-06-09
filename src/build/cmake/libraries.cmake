# Copyright 2025 pugur
# All rights reserved.

cmake_policy(SET CMP0177 NEW)
include(FetchContent)
include(GNUInstallDirs) # For ${CMAKE_INSTALL_BINDIR}

function(setup_zlib)
endfunction()

macro(setup_gtest)
  set(GTEST_ROOT_DIR ${THIRD_PARTY_DIR}/gtest)
  set(GTEST_DIR ${GTEST_ROOT_DIR}/googletest)
  set(GTEST_INCLUDE_DIR ${GTEST_DIR}/include)

  set(GMOCK_DIR ${GTEST_ROOT_DIR}/googlemock)
  set(GMOCK_INCLUDE_DIR ${GMOCK_DIR}/include)

  # https://google.github.io/googletest/quickstart-cmake.html
  set(INSTALL_GTEST FALSE)
  set(BUILD_GMOCK TRUE)

  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ${BUILD_SHARED})

  add_subdirectory(${GTEST_ROOT_DIR})

  set(GTEST_LIBRARIES gtest gtest_main)

  if(BUILD_GMOCK)
    set(GMOCK_LIBRARIES gmock gmock_main)
  endif()
endmacro()

macro(setup_llvm)
  # Windows: manually specify llvm paths
  if(NOT TARGET_OS_NAME MATCHES "windows")
    find_program(LLVM_CONFIG_EXECUTABLE NAMES llvm-config-21 llvm-config)

    if(NOT LLVM_CONFIG_EXECUTABLE)
      message(FATAL_ERROR "llvm-config not found. Please install LLVM development tools.")
    endif()

    execute_process(COMMAND ${LLVM_CONFIG_EXECUTABLE} --includedir
      OUTPUT_VARIABLE LLVM_INCLUDE_DIRS
      OUTPUT_STRIP_TRAILING_WHITESPACE)

    execute_process(COMMAND ${LLVM_CONFIG_EXECUTABLE} --libdir
      OUTPUT_VARIABLE LLVM_LIBRARY_DIRS
      OUTPUT_STRIP_TRAILING_WHITESPACE)

    execute_process(COMMAND ${LLVM_CONFIG_EXECUTABLE} --version
      OUTPUT_VARIABLE LLVM_VERSION
      OUTPUT_STRIP_TRAILING_WHITESPACE)

    message(STATUS "Found LLVM ${LLVM_VERSION} using ${LLVM_CONFIG_EXECUTABLE}")

    add_compile_definitions(USE_LLVM_UNWIND=1)

    list(APPEND PROJECT_LINK_OPTIONS -unwindlib=libunwind -rtlib=compiler-rt)
  endif()

  if(MINGW_BUILD)
    list(APPEND PROJECT_LINK_OPTIONS -Wl,-Bstatic -lc++ -lc++abi -lunwind -Wl,-Bdynamic)
  elseif(NOT TARGET_OS_NAME MATCHES "windows")
    list(APPEND LLVM_LINK_LIBRARIES c++ c++abi unwind)
  endif()

  message(STATUS "LLVM include dirs: ${LLVM_INCLUDE_DIRS}")
  message(STATUS "LLVM library dirs: ${LLVM_LIBRARY_DIRS}")
endmacro()

macro(setup_toml11)
  set(TOML11_DIR ${THIRD_PARTY_DIR}/toml11)
  set(TOML11_INCLUDE_DIR ${TOML11_DIR}/include)

  add_subdirectory(${TOML11_DIR})

  set(TOML11_LIBRARIES toml11)

  set_target_properties(${TOML11_LIBRARIES}
    PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE
  )
endmacro()

macro(setup_zlib)
  set(ZLIB_DIR ${THIRD_PARTY_DIR}/zlib)
  set(ZLIB_INCLUDE_DIR ${ZLIB_DIR})

  set(ZLIB_BUILD_TESTING FALSE)
  set(ZLIB_BUILD_SHARED FALSE)
  set(ZLIB_BUILD_STATIC TRUE)
  set(ZLIB_INSTALL FALSE)

  add_subdirectory(${ZLIB_DIR})

  set(ZLIB_LIBRARIES toml11)

  set_target_properties(${TOML11_LIBRARIES}
    PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE
  )
endmacro()
