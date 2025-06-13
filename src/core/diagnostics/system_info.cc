#include "core/diagnostics/system_info.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include "build/build_flag.h"

#if IS_WINDOWS
#include <ntstatus.h>
#include <windows.h>
#elif IS_UNIX
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#if IS_MAC
#include <sys/sysctl.h>
#include <sys/types.h>
#endif  // IS_MAC
#endif  // IS_WINDOWS

namespace core {

#if IS_WINDOWS
typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(
    PRTL_OSVERSIONINFOW lpVersionInformation);
RTL_OSVERSIONINFOW get_windows_version_info() {
  auto osvi = RTL_OSVERSIONINFOW{sizeof(RTL_OSVERSIONINFOW), 0, 0, 0, 0, {}};
  HMODULE h_module = GetModuleHandle("ntdll.dll");

  if (h_module) {
    auto RtlGetVersion =
        (RtlGetVersionPtr)GetProcAddress(h_module, "RtlGetVersion");
    if (RtlGetVersion != nullptr) {
      if (RtlGetVersion(&osvi) == STATUS_SUCCESS) {
        return osvi;
      }
    }
  }

  return osvi;
}
#endif  // IS_WINDOWS

SystemInfo::SystemInfo() {
  if (!init()) {
    std::cerr << "core::SystemInfo::init() failed" << std::endl;
  }
}

SystemInfo::~SystemInfo() = default;

bool SystemInfo::init() {
  bool result = init_platform();
#if IS_WINDOWS
  auto osvi = get_windows_version_info();

  os_str_ = "Windows ";

  if (osvi.dwMajorVersion == 10) {
    if (osvi.dwBuildNumber >= 22000) {
      os_str_ += "11";
    } else {
      os_str_ += "10";
    }
  } else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3) {
    os_str_ += "8.1";
  } else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2) {
    os_str_ += "8";
  } else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1) {
    os_str_ += "7";
  } else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0) {
    os_str_ += "Vista";
  } else {
    os_str_ += "Unknown";
    result = false;
  }

  os_str_ += " build " + std::to_string(osvi.dwBuildNumber);

  SYSTEM_INFO sys_info;
  GetSystemInfo(&sys_info);
  switch (sys_info.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_AMD64: cpu_arch_ = "x64"; break;
    case PROCESSOR_ARCHITECTURE_INTEL: cpu_arch_ = "x86"; break;
    case PROCESSOR_ARCHITECTURE_ARM: cpu_arch_ = "ARM"; break;
    case PROCESSOR_ARCHITECTURE_ARM64: cpu_arch_ = "ARM64"; break;
    case PROCESSOR_ARCHITECTURE_IA64: cpu_arch_ = "IA64"; break;
    default:
      cpu_arch_ = "Unknown";
      result = false;
      break;
  }

  MEMORYSTATUSEX mem_status;
  mem_status.dwLength = sizeof(mem_status);
  if (GlobalMemoryStatusEx(&mem_status)) {
    total_ram_ = mem_status.ullTotalPhys;
  } else {
    result = false;
  }
#elif IS_UNIX
  struct utsname uts;
  if (uname(&uts) == 0) {
    os_str_ = std::string(uts.sysname) + " " + uts.release;
    cpu_arch_ = uts.machine;
  } else {
    os_str_ = "Unknown OS";
    cpu_arch_ = "Unknown architecture";
    result = false;
  }
#if IS_MAC
  std::size_t size = sizeof(total_ram_);

  if (sysctlbyname("hw.memsize", &total_ram_, &size, nullptr, 0) != 0) {
    result = false;
  }
#elif IS_LINUX
  struct sysinfo mem_info;
  if (sysinfo(&mem_info) == 0) {
    total_ram_ = mem_info.totalram;
    // Does not contain swap.
    // total_ram_ += mem_info.totalswap;
    total_ram_ *= mem_info.mem_unit;
  } else {
    result = false;
  }
#endif
#endif
  return result;
}

bool SystemInfo::init_platform() {
#if IS_WINDOWS
  platform_ = Platform::kWindows;
#elif IS_MAC
  platform_ = Platform::kMac;
#elif IS_LINUX
  platform_ = Platform::kLinux;
#elif IS_ANDROID
  platform_ = Platform::kAndroid;
#endif

  return platform_ != Platform::kUnknown;
}

Platform SystemInfo::platform() const {
  return platform_;
}

const std::string& SystemInfo::os() const {
  return os_str_;
}

const std::string& SystemInfo::cpu_arch() const {
  return cpu_arch_;
}

uint64_t SystemInfo::total_ram_raw() const {
  return total_ram_;
}

std::string SystemInfo::total_ram() const {
  return format_bytes(total_ram_);
}

uint64_t SystemInfo::ram_usage_raw() const {
#if IS_WINDOWS
  MEMORYSTATUSEX memory_status;
  memory_status.dwLength = sizeof(MEMORYSTATUSEX);

  if (!GlobalMemoryStatusEx(&memory_status)) {
    std::cerr << "Couldn't get ram usage" << std::endl;
    return 0;
  }
  return static_cast<uint64_t>(memory_status.ullTotalPhys -
                               memory_status.ullAvailPhys);
#elif defined(__APPLE__) && defined(__MACH__)
  int64_t used = 0;
  int64_t free;
  if (sysctlbyname("vm.page_free_count", &free, &sizeof(free), nullptr, 0) !=
      0) {
    std::cerr << "Couldn't get free ram amount" << std::endl;
    return 0;
  }
  return static_cast<uint64_t>(total_ - free);
#elif IS_UNIX
  struct sysinfo mem_info;
  uint64_t used = 0;
  if (sysinfo(&mem_info) == 0) {
    used = mem_info.totalram - mem_info.freeram;
  } else {
    std::cerr << "Couldn't get ram usage" << std::endl;
  }
  return used;
#endif  // IS_WINDOWS
}

std::string SystemInfo::ram_usage() const {
  return format_bytes(ram_usage_raw());
}

std::string SystemInfo::to_string() const {
  std::string result;
  result.reserve(kSystemInfoStringPredictedSize);
  result.append("Operating System: ");
  result.append(os());
  result.append("\nCPU Architecture: ");
  result.append(cpu_arch());
  result.append("\nTotal RAM: ");
  result.append(total_ram());
  result.append("\nRAM Usage: ");
  result.append(ram_usage());
  return result;
}

std::ostream& operator<<(std::ostream& os, const SystemInfo* sys_info) {
  return os << sys_info->to_string();
}

std::string format_bytes(const uint64_t bytes, const std::size_t precision) {
  std::stringstream ss;

  constexpr uint64_t KiB = 1024;
  constexpr uint64_t MiB = KiB * 1024;
  constexpr uint64_t GiB = MiB * 1024;
  constexpr uint64_t TiB = GiB * 1024;
  constexpr uint64_t PiB = TiB * 1024;

  if (bytes >= PiB) {
    ss.precision(precision);
    ss << std::fixed << (static_cast<double>(bytes) / PiB) << " PiB";
  } else if (bytes >= TiB) {
    ss.precision(precision);
    ss << std::fixed << (static_cast<double>(bytes) / TiB) << " TiB";
  } else if (bytes >= GiB) {
    ss.precision(precision);
    ss << std::fixed << (static_cast<double>(bytes) / GiB) << " GiB";
  } else if (bytes >= MiB) {
    ss.precision(precision);
    ss << std::fixed << (static_cast<double>(bytes) / MiB) << " MiB";
  } else if (bytes >= KiB) {
    ss.precision(precision);
    ss << std::fixed << (static_cast<double>(bytes) / KiB) << " KiB";
  } else {
    ss << bytes << " B";
  }

  return ss.str();
}

}  // namespace core
