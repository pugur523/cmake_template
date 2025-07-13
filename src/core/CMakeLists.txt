set(MODULE_NAME "core")
set(MODULE_OBJECTS_NAME "core_objects")

message(STATUS "Configuring ${MODULE_NAME} module...")

set(SOURCES
  check.cc
  location.cc
  base/file_manager.cc
  base/file_util.cc
  base/logger.cc
  base/source_location.cc
  base/source_range.cc
  base/string_util.cc
  cli/arg_parser.cc
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
  COMPILE_OPTIONS ${PROJECT_COMPILE_OPTIONS}
  COMPILE_DEFINITIONS ${PROJECT_COMPILE_DEFINITIONS}
  LINK_OPTIONS ${PROJECT_LINK_OPTIONS}
  LINK_DIRS ${PROJECT_LINK_DIRECTORIES}
  LINK_LIBS ${ZLIB_LIBRARIES} ${PROJECT_LINK_LIBRARIES}
)

if(ENABLE_VERBOSE)
  describe_target(${MODULE_NAME})
endif()
