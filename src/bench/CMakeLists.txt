set(BENCHMARK_NAME ${MAIN_EXECUTABLE_NAME_FROM_CONFIG}_bench)

message(STATUS "Configuring ${BENCHMARK_NAME}...")

set(SOURCES
  bench_main.cc

  ${PROJECT_SOURCE_DIR}/core/base/file_util_bench.cc
  ${PROJECT_SOURCE_DIR}/core/base/string_util_bench.cc
)

add_executable(${BENCHMARK_NAME} ${SOURCES})

target_include_directories(${BENCHMARK_NAME} PRIVATE ${PROJECT_INCLUDE_DIRECTORIES})
target_link_directories(${BENCHMARK_NAME} PRIVATE ${PROJECT_LINK_DIRECTORIES})
target_link_libraries(${BENCHMARK_NAME} PRIVATE core ${GOOGLE_BENCHMARK_LIBRARIES} ${PROJECT_LINK_LIBRARIES})
target_compile_options(${BENCHMARK_NAME} PRIVATE ${PROJECT_COMPILE_OPTIONS})
target_link_options(${BENCHMARK_NAME} PRIVATE ${PROJECT_LINK_OPTIONS})

set_target_properties(${BENCHMARK_NAME} PROPERTIES
  POSITION_INDEPENDENT_CODE TRUE
)

set(NEED_RUN TRUE)

if(MINGW_BUILD)
  set(EXE_COMMAND wine $<TARGET_FILE:${BENCHMARK_NAME}>)
else()
  set(EXE_COMMAND $<TARGET_FILE:${BENCHMARK_NAME}>)
endif()

if(ENABLE_COVERAGE AND ENABLE_RUN_BENCHMARKS_POST_BUILD)
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

if(ENABLE_XRAY AND ENABLE_RUN_BENCHMARKS_POST_BUILD)
  set(XRAY_OPTIONS "xray_mode=xray-basic:verbosity=1:patch_premain=true")
  set(XRAY_DIR ${CMAKE_BINARY_DIR}/xray/${BENCHMARK_NAME})
  FILE(MAKE_DIRECTORY ${XRAY_DIR})
  set(EXE_PATH $<TARGET_FILE:${BENCHMARK_NAME}>)
  add_custom_command(TARGET ${BENCHMARK_NAME} POST_BUILD
    COMMENT "Run xray instrumentation and generate function timing report"
    COMMAND rm -f ${XRAY_DIR}/xray-log.${BENCHMARK_NAME}.*
    COMMAND ${CMAKE_COMMAND} -E env XRAY_OPTIONS="${XRAY_OPTIONS}" ${CMAKE_COMMAND} -E time ${EXE_PATH}

    COMMAND llvm-xray convert --instr_map=${EXE_PATH} --sort --output-format=yaml --symbolize --demangle --output=${XRAY_DIR}/xray-report.yml ${XRAY_DIR}/xray-log.${BENCHMARK_NAME}.*
    COMMAND llvm-xray convert --instr_map=${EXE_PATH} --sort --output-format=trace_event --symbolize --demangle --output=${XRAY_DIR}/xray_trace.${BENCHMARK_NAME}.json ${XRAY_DIR}/xray-log.${BENCHMARK_NAME}.*
    COMMAND llvm-xray account --instr_map=${EXE_PATH} --sort=count --top=50 --format=text --output=${XRAY_DIR}/xray-summary.txt ${XRAY_DIR}/xray-log.${BENCHMARK_NAME}.*

    WORKING_DIRECTORY ${XRAY_DIR}
  )
  set(NEED_RUN FALSE)
endif()

if(NEED_RUN AND ENABLE_RUN_BENCHMARKS_POST_BUILD)
  add_custom_command(TARGET ${BENCHMARK_NAME} POST_BUILD
    COMMENT "Run benchmarks if it has never been executed."
    COMMAND ${EXE_COMMAND}
  )
endif()

if(INSTALL_BENCHMARK)
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
