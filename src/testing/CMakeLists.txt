set(TEST_NAME ${MAIN_EXECUTABLE_NAME_FROM_CONFIG}_test)

message(STATUS "Configuring ${TEST_NAME}...")

set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cc
  ${PROJECT_SOURCE_DIR}/core/location_test.cc
  ${PROJECT_SOURCE_DIR}/core/base/file_util_test.cc
  ${PROJECT_SOURCE_DIR}/core/base/range_test.cc
  ${PROJECT_SOURCE_DIR}/core/base/string_util_test.cc
  ${PROJECT_SOURCE_DIR}/core/base/vec_test.cc
  ${PROJECT_SOURCE_DIR}/core/diagnostics/system_info_test.cc
)

add_executable(${TEST_NAME} ${SOURCES})

target_include_directories(${TEST_NAME} PRIVATE ${PROJECT_INCLUDE_DIRECTORIES})
target_compile_options(${TEST_NAME} PRIVATE ${PROJECT_COMPILE_OPTIONS})
target_compile_definitions(${TEST_NAME} PRIVATE ${PROJECT_COMPILE_DEFINITIONS})
target_link_options(${TEST_NAME} PRIVATE ${PROJECT_LINK_OPTIONS})
target_link_directories(${TEST_NAME} PRIVATE ${PROJECT_LINK_DIRECTORIES})
target_link_libraries(${TEST_NAME} PRIVATE core ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} ${PROJECT_LINK_LIBRARIES})

set_target_properties(${TEST_NAME} PROPERTIES
  POSITION_INDEPENDENT_CODE TRUE
)

if(ENABLE_RUN_TESTING_POST_BUILD)
  set(NEED_RUN TRUE)
  
  if(MINGW_BUILD)
    set(EXE_COMMAND wine $<TARGET_FILE:${TEST_NAME}>)
  else()
    set(EXE_COMMAND $<TARGET_FILE:${TEST_NAME}>)
  endif()
  
  if(NEED_RUN AND ENABLE_COVERAGE)
    set(COVERAGE_DIR "${CMAKE_BINARY_DIR}/coverage/${TEST_NAME}")
    FILE(MAKE_DIRECTORY ${COVERAGE_DIR})
    add_custom_command(TARGET ${TEST_NAME} POST_BUILD
      COMMENT "Run & generate coverage report"
      COMMAND ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=${COVERAGE_DIR}/profile.profraw ${EXE_COMMAND}
      COMMAND llvm-profdata merge -output=${COVERAGE_DIR}/merged.profdata ${COVERAGE_DIR}/profile.profraw
      COMMAND llvm-cov show $<TARGET_FILE:${TEST_NAME}>
      -instr-profile=${COVERAGE_DIR}/merged.profdata
      -format=html
      -output-dir=${COVERAGE_DIR}/html
  
      WORKING_DIRECTORY ${PROJECT_ROOT_DIR}
    )
    set(NEED_RUN FALSE)
  endif()
  
  if(NEED_RUN)
    add_custom_command(TARGET ${TEST_NAME} POST_BUILD
      COMMENT "Run tests if it has never been executed."
      COMMAND ${EXE_COMMAND}
    )
  endif()
endif()

if(ENABLE_INSTALL_TESTING)
  install(
    TARGETS ${TEST_NAME}
    RUNTIME
    COMPONENT Runtime
  )

  install(
    FILES "${CMAKE_PDB_OUTPUT_DIRECTORY}/${TEST_NAME}.pdb"
    DESTINATION bin
    COMPONENT Runtime
    OPTIONAL
  )
endif()

# add_test(
#   NAME ${TEST_NAME}
#   COMMAND ${TEST_NAME}
# )

if(ENABLE_VERBOSE)
  describe_target(${TEST_NAME})
endif()

# include(GoogleTest)
# gtest_discover_tests(${TEST_NAME})
