set(MODULE_NAME "core")
set(MODULE_OBJECTS_NAME "core_objects")

message(STATUS "Configuring ${MODULE_NAME} module...")

set(SOURCES
  check.cc
  location.cc
  base/file_manager.cc
  base/file_util.cc
  base/file_util_build_info.cc
  base/logger.cc
  base/source_location.cc
  base/source_range.cc
  base/string_util.cc
  cli/ansi/progress_bar.cc
  cli/ansi/style_builder.cc
  cli/arg_parser.cc
  cli/arg_parser_build_info.cc
  cli/console.cc
  diagnostics/signal_handler.cc
  diagnostics/stack_trace_entry.cc
  diagnostics/stack_trace.cc
  diagnostics/system_info.cc
  diagnostics/terminate_handler.cc
  time/time_util.cc
)

add_library(${MODULE_OBJECTS_NAME} OBJECT ${SOURCES})

setup_module(${MODULE_NAME} ${MODULE_OBJECTS_NAME}
  INCLUDE_DIRS ${PROJECT_INCLUDE_DIRECTORIES}
  C_CXX_COMPILE_OPTIONS ${PROJECT_C_CXX_COMPILE_OPTIONS}
  NASM_COMPILE_OPTIONS ${PROJECT_NASM_COMPILE_OPTIONS}
  COMPILE_DEFINITIONS ${PROJECT_COMPILE_DEFINITIONS}
  LINK_OPTIONS ${PROJECT_LINK_OPTIONS}
  LINK_DIRS ${PROJECT_LINK_DIRECTORIES}
  LINK_LIBS ${ZLIB_LIBRARIES} ${PROJECT_LINK_LIBRARIES}
)

if(ENABLE_VERBOSE)
  describe_target(${MODULE_NAME})
endif()
