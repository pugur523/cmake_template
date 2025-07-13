#include "core/base/logger.h"

#include "femtolog/logger.h"
#include "femtolog/sinks/file_sink.h"
#include "femtolog/sinks/stdout_sink.h"

namespace core {

// extern
// femtolog::Logger& glog = []() -> femtolog::Logger& {
//   femtolog::Logger& logger = femtolog::Logger::global_logger();
//   return logger;
// }();

// extern
femtolog::Logger& glog = femtolog::Logger::global_logger();

void register_glog() {
  femtolog::FemtologOptions options;
  options.spsc_queue_size = 1024 * 1024 * 8;       // 8MiB
  options.backend_format_buffer_size = 1024 * 8;   // 8KiB
  options.backend_dequeue_buffer_size = 1024 * 8;  // 8KiB
  options.backend_worker_cpu_affinity = 5;  // pin backend thread to core 5
  glog.init();
  glog.register_sink<femtolog::StdoutSink<>>();
  glog.register_sink<femtolog::FileSink>();
  glog.start_worker();
}

}  // namespace core
