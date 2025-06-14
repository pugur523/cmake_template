#ifndef CORE_CHECK_H_
#define CORE_CHECK_H_

#include <iostream>

#include "build/build_flag.h"
#include "core/base/core_export.h"

#define NOOP           \
  if constexpr (false) \
  ::std::cout

#define CHECK_IMPL(condition, type) \
  if (!(condition))                 \
  ::core::CheckFailureStream(#type, __FILE__, __LINE__, #condition).stream()

#define CHECK_W_OP_IMPL(val1, val2, op, type)           \
  if (!((val1)op(val2)))                                \
  ::core::CheckFailureStream(#type, __FILE__, __LINE__, \
                             (#val1 " " #op " " #val2)) \
      .stream()

#define CHECK(condition) CHECK_IMPL(condition, CHECK)
#define CHECK_EQ(val1, val2) CHECK_W_OP_IMPL(val1, val2, ==, CHECK)
#define CHECK_NE(val1, val2) CHECK_W_OP_IMPL(val1, val2, !=, CHECK)
#define CHECK_GT(val1, val2) CHECK_W_OP_IMPL(val1, val2, >, CHECK)
#define CHECK_GE(val1, val2) CHECK_W_OP_IMPL(val1, val2, >=, CHECK)
#define CHECK_LT(val1, val2) CHECK_W_OP_IMPL(val1, val2, <, CHECK)
#define CHECK_LE(val1, val2) CHECK_W_OP_IMPL(val1, val2, <=, CHECK)

#if IS_RELEASE
#define DCHECK(condition) NOOP
#define DCHECK_EQ(val1, val2) NOOP
#define DCHECK_NE(val1, val2) NOOP
#define DCHECK_GT(val1, val2) NOOP
#define DCHECK_GE(val1, val2) NOOP
#define DCHECK_LT(val1, val2) NOOP
#define DCHECK_LE(val1, val2) NOOP
#else
#define DCHECK(condition) CHECK_IMPL(condition, DCHECK)
#define DCHECK_EQ(val1, val2) CHECK_W_OP_IMPL(val1, val2, ==, DCHECK)
#define DCHECK_NE(val1, val2) CHECK_W_OP_IMPL(val1, val2, !=, DCHECK)
#define DCHECK_GT(val1, val2) CHECK_W_OP_IMPL(val1, val2, >, DCHECK)
#define DCHECK_GE(val1, val2) CHECK_W_OP_IMPL(val1, val2, >=, DCHECK)
#define DCHECK_LT(val1, val2) CHECK_W_OP_IMPL(val1, val2, <, DCHECK)
#define DCHECK_LE(val1, val2) CHECK_W_OP_IMPL(val1, val2, <=, DCHECK)
#endif  // IS_RELEASE

namespace core {

class CORE_EXPORT CheckFailureStream {
 public:
  CheckFailureStream(const char* type,
                     const char* file,
                     int line,
                     const char* condition);
  ~CheckFailureStream();
  std::ostream& stream();

 private:
  const char* type_;
  const char* file_;
  int line_;
  const char* condition_;
};

}  // namespace core

#endif  // CORE_CHECK_H_
