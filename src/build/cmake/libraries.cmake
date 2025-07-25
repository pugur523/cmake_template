macro(setup_gtest)
  set(GTEST_ROOT_DIR ${THIRD_PARTY_DIR}/gtest)
  set(GTEST_DIR ${GTEST_ROOT_DIR}/googletest)
  set(GTEST_INCLUDE_DIR ${GTEST_DIR}/include)

  set(GMOCK_DIR ${GTEST_ROOT_DIR}/googlemock)
  set(GMOCK_INCLUDE_DIR ${GMOCK_DIR}/include)

  # https://google.github.io/googletest/quickstart-cmake.html
  set(INSTALL_GTEST FALSE CACHE BOOL "" FORCE)
  set(BUILD_GMOCK TRUE CACHE BOOL "" FORCE)

  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt TRUE CACHE BOOL "" FORCE)

  add_subdirectory(${GTEST_ROOT_DIR})

  # Do not link `gtest_main` and `gmock_main` otherwise the testing will contain duplicate entrypoints.
  set(GTEST_LIBRARIES gtest)

  if(BUILD_GMOCK)
    set(GMOCK_LIBRARIES gmock)
  endif()
endmacro()

macro(setup_google_benchmark)
  set(GOOGLE_BENCHMARK_DIR ${THIRD_PARTY_DIR}/google_benchmark)
  set(GOOGLE_BENCHMARK_INCLUDE_DIR ${GOOGLE_BENCHMARK_DIR}/include)

  set(BENCHMARK_ENABLE_TESTING FALSE CACHE BOOL "" FORCE)
  set(BENCHMARK_ENABLE_EXCEPTIONS TRUE CACHE BOOL "" FORCE)
  set(BENCHMARK_ENABLE_LTO ${ENABLE_LTO} CACHE BOOL "" FORCE)
  set(BENCHMARK_USE_LIBCXX TRUE CACHE BOOL "" FORCE)
  set(BENCHMARK_ENABLE_WERROR ${ENABLE_WARNINGS_AS_ERRORS} CACHE BOOL "" FORCE)
  set(BENCHMARK_FORCE_WERROR ${ENABLE_WARNINGS_AS_ERRORS} CACHE BOOL "" FORCE)

  set(BENCHMARK_ENABLE_INSTALL FALSE CACHE BOOL "" FORCE)
  set(BENCHMARK_INSTALL_DOCS FALSE CACHE BOOL "" FORCE)

  if(MINGW_BUILD)
    set(HAVE_STD_REGEX FALSE CACHE BOOL "" FORCE)
    set(HAVE_STEADY_CLOCK FALSE CACHE BOOL "" FORCE)
    set(HAVE_THREAD_SAFETY_ATTRIBUTES FALSE CACHE BOOL "" FORCE)
  endif()

  if(HOST_OS_NAME MATCHES "darwin")
    set(HAVE_STD_REGEX TRUE CACHE BOOL "" FORCE)  
    set(HAVE_STEADY_CLOCK TRUE CACHE BOOL "" FORCE)
    set(HAVE_THREAD_SAFETY_ATTRIBUTES FALSE CACHE BOOL "" FORCE)
  endif()
  # clang-cl's `/GL` option will compete with `/clang:-flto=thin`
  if(HOST_OS_NAME MATCHES "windows" AND ENABLE_LTO)
    set(BENCHMARK_ENABLE_LTO FALSE CACHE BOOL "" FORCE)
  endif()

  add_subdirectory(${GOOGLE_BENCHMARK_DIR})

  set(GOOGLE_BENCHMARK_LIBRARIES benchmark::benchmark)
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
  endif()

  if(ENABLE_LLVM_UNWIND)
    list(APPEND PROJECT_COMPILE_DEFINITIONS ENABLE_LLVM_UNWIND=1)
  else()
    list(APPEND PROJECT_COMPILE_DEFINITIONS ENABLE_LLVM_UNWIND=0)
  endif()

  if(NOT APPLE)
    if(ENABLE_LLVM_UNWIND)
      list(APPEND PROJECT_LINK_OPTIONS -unwindlib=libunwind -rtlib=compiler-rt)
    endif()
  endif()

  if(MINGW_BUILD)
    list(APPEND PROJECT_LINK_OPTIONS -Wl,-Bstatic -lc++ -lc++abi -lunwind -Wl,-Bdynamic)
  endif()
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

  set(ZLIB_ENABLE_BUILD_TESTING FALSE CACHE BOOL "" FORCE)
  set(ZLIB_BUILD_STATIC TRUE CACHE BOOL "" FORCE)
  set(ZLIB_ENABLE_BUILD_SHARED FALSE CACHE BOOL "" FORCE)
  set(ZLIB_BUILD_MINIZIP FALSE CACHE BOOL "" FORCE)
  set(ZLIB_BUILD_TESTING FALSE CACHE BOOL "" FORCE)
  set(ZLIB_INSTALL FALSE CACHE BOOL "" FORCE)

  set(ZLIB_LIBRARIES zlibstatic)

  add_subdirectory(${ZLIB_DIR})

  target_compile_options(${ZLIB_LIBRARIES} PRIVATE "-Wno-implicit-function-declaration")

  set_target_properties(${ZLIB_LIBRARIES}
    PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE
  )
endmacro()

macro(setup_femtolog)
  set(FEMTOLOG_DIR ${THIRD_PARTY_DIR}/femtolog)
  set(FEMTOLOG_INCLUDE_DIR ${FEMTOLOG_DIR}/src/include)

  set(FEMTOLOG_LIBRARIES femtolog)

  set(FEMTOLOG_BUILD_SHARED FALSE CACHE BOOL "" FORCE)
  set(FEMTOLOG_ENABLE_VERBOSE FALSE CACHE BOOL "" FORCE) 

  set(FEMTOLOG_ENABLE_LTO ${PROJECT_ENABLE_LTO} CACHE BOOL "" FORCE) 
  set(FEMTOLOG_ENABLE_NATIVE_ARCH ${PROJECT_ENABLE_NATIVE_ARCH} CACHE BOOL "" FORCE) 

  set(FEMTOLOG_ENABLE_BUILD_REPORT ${PROJECT_ENABLE_BUILD_REPORT} CACHE BOOL "" FORCE)
  set(FEMTOLOG_ENABLE_COVERAGE ${PROJECT_ENABLE_COVERAGE} CACHE BOOL "" FORCE)
  set(FEMTOLOG_ENABLE_OPTIMIZATION_REPORT ${PROJECT_ENABLE_OPTIMIZATION_REPORT} CACHE BOOL "" FORCE)
  set(FEMTOLOG_ENABLE_XRAY ${PROJECT_ENABLE_XRAY} CACHE BOOL "" FORCE)
  set(FEMTOLOG_ENABLE_SANITIZERS ${PROJECT_ENABLE_SANITIZERS} CACHE BOOL "" FORCE)
  set(FEMTOLOG_ENABLE_LLVM_UNWIND ${PROJECT_ENABLE_LLVM_UNWIND} CACHE BOOL "" FORCE)
  set(FEMTOLOG_ENABLE_AVX2 ${PROJECT_ENABLE_AVX2} CACHE BOOL "" FORCE)

  set(FEMTOLOG_ENABLE_WARNINGS_AS_ERRORS FALSE CACHE BOOL "" FORCE)

  set(FEMTOLOG_USE_EXTERNAL_ZLIB TRUE CACHE BOOL "" FORCE)
  set(FEMTOLOG_USE_EXTERNAL_FMTLIB FALSE CACHE BOOL "" FORCE)
  set(FEMTOLOG_USE_EXTERNAL_GTEST TRUE CACHE BOOL "" FORCE)
  set(FEMTOLOG_USE_EXTERNAL_GOOGLE_BENCHMARK TRUE CACHE BOOL "" FORCE)
  add_subdirectory(${FEMTOLOG_DIR})

  set_target_properties(${FEMTOLOG_LIBRARIES}
    PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE
  )
endmacro()
