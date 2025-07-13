set(MAIN_EXECUTABLE_NAME ${MAIN_EXECUTABLE_NAME_FROM_CONFIG})

message(STATUS "Configuring ${MAIN_EXECUTABLE_NAME}...")

set(COMMON_SOURCES
  app.cc
  cli_handler.cc
  init_handler.cc
  preinit_handler.cc
)

set(WINDOWS_SOURCES
  exe.rc
  win_main.cc
)

set(UNIX_SOURCES
  main.cc
)

if(TARGET_OS_NAME MATCHES "windows")
  set(SOURCES
    ${COMMON_SOURCES}
    ${WINDOWS_SOURCES}
  )

  # If you don't want to show console, use WIN32 subsystem.
  # add_executable(${MAIN_EXECUTABLE_NAME} WIN32 ${SOURCES})
  add_executable(${MAIN_EXECUTABLE_NAME} ${SOURCES})

else()
  set(SOURCES
    ${COMMON_SOURCES}
    ${UNIX_SOURCES}
  )
  add_executable(${MAIN_EXECUTABLE_NAME} ${SOURCES})
endif()

target_include_directories(${MAIN_EXECUTABLE_NAME} PRIVATE ${PROJECT_INCLUDE_DIRECTORIES})
target_compile_options(${MAIN_EXECUTABLE_NAME} PRIVATE ${PROJECT_COMPILE_OPTIONS})
target_compile_definitions(${MAIN_EXECUTABLE_NAME} PRIVATE ${PROJECT_COMPILE_DEFINITIONS})
target_link_options(${MAIN_EXECUTABLE_NAME} PRIVATE ${PROJECT_LINK_OPTIONS})
target_link_directories(${MAIN_EXECUTABLE_NAME} PRIVATE ${PROJECT_LINK_DIRECTORIES})
target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE core ${PROJECT_LINK_LIBRARIES})

set_target_properties(${MAIN_EXECUTABLE_NAME} PROPERTIES
  POSITION_INDEPENDENT_CODE TRUE
)

if(DO_CLANG_TIDY)
  set_target_properties(${MAIN_EXECUTABLE_NAME} PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}"
  )
endif()

if(ENABLE_RUN_PROGRAM_POST_BUILD)
  set(NEED_RUN TRUE)
  
  if(MINGW_BUILD)
    set(EXE_COMMAND wine $<TARGET_FILE:${MAIN_EXECUTABLE_NAME}>)
  else()
    set(EXE_COMMAND $<TARGET_FILE:${MAIN_EXECUTABLE_NAME}>)
  endif()
  
  if(ENABLE_COVERAGE)
    set(COVERAGE_DIR "${CMAKE_BINARY_DIR}/coverage/${MAIN_EXECUTABLE_NAME}")
    FILE(MAKE_DIRECTORY ${COVERAGE_DIR})
    add_custom_command(TARGET ${MAIN_EXECUTABLE_NAME} POST_BUILD
      COMMENT "Run & generate coverage report"
      COMMAND ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=${COVERAGE_DIR}/profile.profraw ${EXE_COMMAND}
      COMMAND llvm-profdata merge -output=${COVERAGE_DIR}/merged.profdata ${COVERAGE_DIR}/profile.profraw
      COMMAND llvm-cov show $<TARGET_FILE:${MAIN_EXECUTABLE_NAME}>
      -instr-profile=${COVERAGE_DIR}/merged.profdata
      -format=html
      -output-dir=${COVERAGE_DIR}/html
  
      WORKING_DIRECTORY ${PROJECT_ROOT_DIR}
    )
    set(NEED_RUN FALSE)
  endif()
  
  if(ENABLE_XRAY)
    set(XRAY_OPTIONS "xray_mode=xray-basic:verbosity=1:patch_premain=true")
    set(XRAY_DIR ${CMAKE_BINARY_DIR}/xray/${MAIN_EXECUTABLE_NAME})
    FILE(MAKE_DIRECTORY ${XRAY_DIR})
    set(EXE_PATH $<TARGET_FILE:${MAIN_EXECUTABLE_NAME}>)
    add_custom_command(TARGET ${MAIN_EXECUTABLE_NAME} POST_BUILD
      COMMENT "Run xray instrumentation and generate function timing report"
      COMMAND rm -f ${XRAY_DIR}/xray-log.${MAIN_EXECUTABLE_NAME}.*
      COMMAND ${CMAKE_COMMAND} -E env XRAY_OPTIONS="${XRAY_OPTIONS}" ${CMAKE_COMMAND} -E time ${EXE_PATH}
  
      COMMAND llvm-xray convert --instr_map=${EXE_PATH} --sort --output-format=yaml --symbolize --demangle --output=${XRAY_DIR}/xray-report.yml ${XRAY_DIR}/xray-log.${MAIN_EXECUTABLE_NAME}.*
      COMMAND llvm-xray convert --instr_map=${EXE_PATH} --sort --output-format=trace_event --symbolize --demangle --output=${XRAY_DIR}/xray_trace.${MAIN_EXECUTABLE_NAME}.json ${XRAY_DIR}/xray-log.${MAIN_EXECUTABLE_NAME}.*
      COMMAND llvm-xray account --instr_map=${EXE_PATH} --sort=count --top=50 --format=text --output=${XRAY_DIR}/xray-summary.txt ${XRAY_DIR}/xray-log.${MAIN_EXECUTABLE_NAME}.*
      WORKING_DIRECTORY ${XRAY_DIR}
    )
    set(NEED_RUN FALSE)
  endif()
  
  if(NEED_RUN)
    add_custom_command(TARGET ${MAIN_EXECUTABLE_NAME} POST_BUILD
      COMMENT "Run program if it has never been executed."
      COMMAND ${EXE_COMMAND}
    )
  endif()
endif()
  
if(ENABLE_OPTIMIZATION_REPORT)
  add_custom_command(TARGET ${MAIN_EXECUTABLE_NAME} POST_BUILD
    COMMENT "Run opt-viewer to generate optimization report"
    COMMAND ${PROJECT_SOURCE_DIR}/build/scripts/opt_viewer/opt-viewer.py ${CMAKE_BINARY_DIR} -s ${CMAKE_CURRENT_BINARY_DIR} -o ${CMAKE_BINARY_DIR}/opt_report
    COMMAND ${PROJECT_SOURCE_DIR}/build/scripts/opt_viewer/opt-stats.py ${CMAKE_BINARY_DIR}

    WORKING_DIRECTORY ${PROJECT_ROOT_DIR}
  )
endif()

set(COMPILE_COMMANDS_OUTPUT_DIR "${PROJECT_ROOT_DIR}")
set(COMPILE_COMMANDS_FILE "${CMAKE_BINARY_DIR}/compile_commands.json")

add_custom_command(TARGET ${MAIN_EXECUTABLE_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  "${COMPILE_COMMANDS_FILE}"
  "${COMPILE_COMMANDS_OUTPUT_DIR}/compile_commands.json"

  WORKING_DIRECTORY ${PROJECT_ROOT_DIR}
)

if(ENABLE_INSTALL_PROGRAM)
  install(
    TARGETS ${MAIN_EXECUTABLE_NAME}
    RUNTIME
    COMPONENT Runtime
  )

  install(
    FILES "${CMAKE_PDB_OUTPUT_DIRECTORY}/${MAIN_EXECUTABLE_NAME}.pdb"
    DESTINATION bin
    COMPONENT Runtime
    OPTIONAL
  )

  # install(
  # TARGETS ${MAIN_EXECUTABLE_NAME}
  # RUNTIME_DEPENDENCY_SET "${MAIN_EXECUTABLE_NAME}_runtime_deps"
  # )
  # install(RUNTIME_DEPENDENCY_SET "${MAIN_EXECUTABLE_NAME}_runtime_deps")
endif()

if(ENABLE_VERBOSE)
  describe_target(${MAIN_EXECUTABLE_NAME})
endif()
