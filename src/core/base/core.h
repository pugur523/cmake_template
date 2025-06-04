// Copyright 2025 pugur
// All rights reserved.

#ifndef CORE_BASE_CORE_H_
#define CORE_BASE_CORE_H_

#include "build/component_export.h"

namespace core {

#define CORE_EXPORT COMPONENT_EXPORT(CORE)

CORE_EXPORT int sample(int a, int b);

}  // namespace core

#endif  // CORE_BASE_CORE_H_
