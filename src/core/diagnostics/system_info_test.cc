#include "core/diagnostics/system_info.h"

#include "gtest/gtest.h"

namespace core {

TEST(SystemInfoTest, BasicSystemInfo) {
  SystemInfo info;
  EXPECT_FALSE(info.os().empty());
  EXPECT_FALSE(info.cpu_arch().empty());
  EXPECT_NE(info.platform(), Platform::kUnknown);
  EXPECT_FALSE(info.ram_usage().empty());
  EXPECT_GT(info.ram_usage_raw(), 0);
  EXPECT_FALSE(info.total_ram().empty());
  EXPECT_GT(info.total_ram_raw(), 0);
  EXPECT_FALSE(info.to_string().empty());
}

}  // namespace core

