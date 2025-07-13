#ifndef CORE_BASE_LOGGER_H_
#define CORE_BASE_LOGGER_H_

#include "core/base/core_export.h"
#include "femtolog/logger.h"

namespace core {

CORE_EXPORT extern femtolog::Logger& glog;

CORE_EXPORT void register_glog();

}  // namespace core

#endif  // CORE_BASE_LOGGER_H_
