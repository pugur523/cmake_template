set(BENCHMARK_NAME ${MAIN_EXECUTABLE_NAME_FROM_CONFIG}_bench)

message(STATUS "Configuring ${BENCHMARK_NAME}...")

set(SOURCES
  bench_main.cc

  ${PROJECT_SOURCE_DIR}/core/base/file_util_bench.cc
  ${PROJECT_SOURCE_DIR}/core/base/string_util_bench.cc
)

add_executable(${BENCHMARK_NAME} ${SOURCES})

target_include_directories(${BENCHMARK_NAME} PRIVATE ${PROJECT_INCLUDE_DIRECTORIES})
target_compile_options(${BENCHMARK_NAME} PRIVATE ${PROJECT_COMPILE_OPTIONS})
target_compile_definitions(${BENCHMARK_NAME} PRIVATE ${PROJECT_COMPILE_DEFINITIONS})
target_link_options(${BENCHMARK_NAME} PRIVATE ${PROJECT_LINK_OPTIONS})
target_link_directories(${BENCHMARK_NAME} PRIVATE ${PROJECT_LINK_DIRECTORIES})
target_link_libraries(${BENCHMARK_NAME} PRIVATE core ${GOOGLE_BENCHMARK_LIBRARIES} ${PROJECT_LINK_LIBRARIES})

set_target_properties(${BENCHMARK_NAME} PROPERTIES
  POSITION_INDEPENDENT_CODE TRUE
)


if (ENABLE_RUN_BENCHMARK_POST_BUILD)
  set(NEED_RUN TRUE)
  
  if(MINGW_BUILD)
    set(EXE_COMMAND wine $<TARGET_FILE:${BENCHMARK_NAME}>)
  else()
    set(EXE_COMMAND $<TARGET_FILE:${BENCHMARK_NAME}>)
  endif()
  
  if(NEED_RUN AND ENABLE_COVERAGE)
    set(COVERAGE_DIR "${CMAKE_BINARY_DIR}/coverage/${BENCHMARK_NAME}")
    FILE(MAKE_DIRECTORY ${COVERAGE_DIR})
    add_custom_command(TARGET ${BENCHMARK_NAME} POST_BUILD
      COMMENT "Run & generate coverage report"
      COMMAND ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=${COVERAGE_DIR}/profile.profraw ${EXE_COMMAND}
      COMMAND llvm-profdata merge -output=${COVERAGE_DIR}/merged.profdata ${COVERAGE_DIR}/profile.profraw
      COMMAND llvm-cov show $<TARGET_FILE:${BENCHMARK_NAME}>
      -instr-profile=${COVERAGE_DIR}/merged.profdata
      -format=html
      -output-dir=${COVERAGE_DIR}/html
  
      WORKING_DIRECTORY ${PROJECT_ROOT_DIR}
    )
    set(NEED_RUN FALSE)
  endif()
  
  if(NEED_RUN)
    add_custom_command(TARGET ${BENCHMARK_NAME} POST_BUILD
      COMMENT "Run benchmarks if it has never been executed."
      COMMAND ${EXE_COMMAND}
    )
  endif()
endif()

if(ENABLE_INSTALL_BENCHMARK)
  install(
    TARGETS ${BENCHMARK_NAME}
    RUNTIME
    COMPONENT Runtime
  )

  install(
    FILES "${CMAKE_PDB_OUTPUT_DIRECTORY}/${BENCHMARK_NAME}.pdb"
    DESTINATION bin
    COMPONENT Runtime
    OPTIONAL
  )
endif()

if(ENABLE_VERBOSE)
  describe_target(${BENCHMARK_NAME})
endif()
