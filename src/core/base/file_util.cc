#include "core/base/file_util.h"

#include <limits.h>

#include "build/build_flag.h"

#if IS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#undef APIENTRY
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace core {

bool file_exists(const char* file_name) {
#if IS_WINDOWS
  DWORD attributes = GetFileAttributesA(file_name);
  return (attributes != INVALID_FILE_ATTRIBUTES &&
          !(attributes & FILE_ATTRIBUTE_DIRECTORY));
#else
  struct stat buffer;
  return (stat(file_name, &buffer) == 0) && S_ISREG(buffer.st_mode);
#endif
}

bool dir_exists(const char* dir_name) {
#if IS_WINDOWS
  DWORD attributes = GetFileAttributesA(dir_name);
  return (attributes != INVALID_FILE_ATTRIBUTES &&
          (attributes & FILE_ATTRIBUTE_DIRECTORY));
#else
  struct stat buffer;
  return stat(dir_name, &buffer) == 0 && S_ISDIR(buffer.st_mode);
#endif
}

const std::string& get_exe_path() {
  static const std::string cached_path = []() -> std::string {
#if IS_WINDOWS
    char path[MAX_PATH] = {};
    DWORD len = GetModuleFileNameA(nullptr, path, MAX_PATH);
    if (len == 0 || len >= MAX_PATH) {
      return "";
    }
    return (len == 0 || len >= MAX_PATH) ? "" : std::string(path, len);
#else
    char path[kPathMaxLength] = {};
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    return len == -1 ? "" : std::string(path, len);
#endif
  }();

  return cached_path;
}

const std::string& get_exe_dir() {
  static const std::string cached_dir = []() -> std::string {
    std::string exe_path = get_exe_path();
    if (exe_path.empty()) {
      return "";
    }

    std::size_t pos = exe_path.find_last_of(DIR_SEPARATOR);
    if (pos == std::string::npos) {
      return "";
    }

    return exe_path.substr(0, pos);
  }();
  return cached_dir;
}

const std::string& get_resources_dir() {
  static const std::string cached_resources_dir =
      join_path(get_exe_dir(), "resources");
  return cached_resources_dir;
}

std::vector<std::string> list_files(const std::string& path) {
  std::vector<std::string> files;
  files.reserve(kPredictedFilesNbPerDir);

#if IS_WINDOWS
  std::string search_path = path + "\\*";
  WIN32_FIND_DATAA find_data;
  HANDLE hFind = FindFirstFileA(search_path.c_str(), &find_data);

  if (hFind == INVALID_HANDLE_VALUE) {
    std::cerr << "Cannot open directory: " << path << std::endl;
    return files;
  }

  do {
    std::string name = find_data.cFileName;
    if (name != "." && name != "..") {
      files.emplace_back(name);
    }
  } while (FindNextFileA(hFind, &find_data) != 0);

  FindClose(hFind);
  return files;

#else
  DIR* dir = opendir(path.c_str());
  if (!dir) {
    std::cerr << "Cannot open directory: " << path << std::endl;
    return files;
  }

  struct dirent* entry;
  while ((entry = readdir(dir)) != nullptr) {
    std::string name = entry->d_name;
    if (name != "." && name != "..") {
      files.emplace_back(name);
    }
  }

  closedir(dir);
#endif
  return files;
}

int create_directory(const char* path) {
#if IS_WINDOWS
  return CreateDirectoryA(path, nullptr);
#else
  return mkdir(path, 0755);
#endif
}

void create_directories(const std::string& path) {
  std::string sanitized_path = sanitize_component(path, true);
  std::size_t pos = 0;

  while ((pos = sanitized_path.find(DIR_SEPARATOR, pos)) != std::string::npos) {
    std::string dir = sanitized_path.substr(0, pos);
    if (!dir.empty() && !dir_exists(dir.c_str())) {
      if (create_directory(dir.c_str()) != 0) {
        std::cerr << "Failed to create directory: " << dir << std::endl;
      }
    }
    pos++;
  }
}

int write_binary_to_file(const void* binary_data,
                         std::size_t binary_size,
                         const std::string& output_path) {
  if (!binary_data || binary_size == 0) {
    std::cerr << "Invalid data or size (null or zero) for file: " << output_path
              << std::endl;
    return 3;
  }

  std::unique_ptr<FILE, decltype(&fclose)> fp(fopen(output_path.c_str(), "wb"),
                                              &fclose);
  if (!fp) {
    std::cerr << "Failed to open file for writing: " << output_path
              << std::endl;
    std::perror("fopen");
    return 2;
  }

  std::size_t written = fwrite(binary_data, 1, binary_size, fp.get());
  if (written != binary_size) {
    std::cerr << "Failed to write all data to file: " << output_path
              << " (written " << written << " / " << binary_size << ")"
              << std::endl;
    std::perror("fwrite");
    return 1;
  }

  return 0;
}

std::string file_extension(const std::string& path) {
  std::size_t last_slash = path.find_last_of("/\\");
  std::size_t last_dot = path.find_last_of('.');

  if (last_dot == std::string::npos ||
      (last_slash != std::string::npos && last_dot < last_slash)) {
    return "";  // No extension or dot is part of directory
  }

  // Handle hidden files like ".gitignore"
  if (last_dot == 0 ||
      (last_slash != std::string::npos && last_dot == last_slash + 1)) {
    return "";
  }

  return path.substr(last_dot + 1);
}

std::string sanitize_component(const std::string& part, bool is_first) {
  std::string result = part;

  // Remove leading separator if not first
  if (!is_first && !result.empty() && result.front() == DIR_SEPARATOR) {
    result.erase(0, 1);
  }

  // Remove trailing separator
  if (!result.empty() && result.back() == DIR_SEPARATOR) {
    result.pop_back();
  }

  return result;
}

}  // namespace core
