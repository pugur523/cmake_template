#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "benchmark/benchmark.h"
#include "core/base/file_util.h"

namespace fs = std::filesystem;

namespace {

// Setup temporary test files/directories
template <typename F>
void with_temp_file(const std::string& content, const F& fn) {
  const std::string tmp_path = "./tmp_test_file.txt";
  std::ofstream ofs(tmp_path);
  ofs << content;
  ofs.close();

  fn(tmp_path);

  std::remove(tmp_path.c_str());
}

template <typename F>
void with_temp_dir(const F& fn) {
  const std::string tmp_dir = "./tmp_test_dir";
  fs::create_directory(tmp_dir);
  fn(tmp_dir);
  fs::remove_all(tmp_dir);
}

static void file_util_file_exists(benchmark::State& state) {
  with_temp_file("hello", [&](const std::string& path) {
    for (auto _ : state) {
      benchmark::DoNotOptimize(core::file_exists(path.c_str()));
    }
  });
}
BENCHMARK(file_util_file_exists);

static void file_util_dir_exists(benchmark::State& state) {
  with_temp_dir([&](const std::string& path) {
    for (auto _ : state) {
      benchmark::DoNotOptimize(core::dir_exists(path.c_str()));
    }
  });
}
BENCHMARK(file_util_dir_exists);

static void file_util_get_exe_path(benchmark::State& state) {
  for (auto _ : state) {
    std::string exe_path = core::get_exe_path();
    benchmark::DoNotOptimize(exe_path);
  }
}
BENCHMARK(file_util_get_exe_path);

static void file_util_get_exe_dir(benchmark::State& state) {
  for (auto _ : state) {
    std::string exe_dir = core::get_exe_dir();
    benchmark::DoNotOptimize(exe_dir);
  }
}
BENCHMARK(file_util_get_exe_dir);

static void file_util_list_files(benchmark::State& state) {
  with_temp_dir([&](const std::string& dir) {
    for (int i = 0; i < 10; ++i) {
      std::ofstream(dir + "/file" + std::to_string(i)) << "test";
    }
    for (auto _ : state) {
      benchmark::DoNotOptimize(core::list_files(dir));
    }
  });
}
BENCHMARK(file_util_list_files);

static void file_util_write_binary_to_file(benchmark::State& state) {
  const std::string path = "/dev/null";
  std::vector<char> data(4096, 'A');
  for (auto _ : state) {
    core::write_binary_to_file(data.data(), data.size(), path);
  }
  std::remove(path.c_str());
}
BENCHMARK(file_util_write_binary_to_file);

static void file_util_file_extension(benchmark::State& state) {
  const std::string path = "/path/to/file.name.ext";
  for (auto _ : state) {
    benchmark::DoNotOptimize(core::file_extension(path));
  }
}
BENCHMARK(file_util_file_extension);

static void file_util_sanitize_component(benchmark::State& state) {
  const std::string path = "/foo/bar/";
  for (auto _ : state) {
    benchmark::DoNotOptimize(core::sanitize_component(path, false));
  }
}
BENCHMARK(file_util_sanitize_component);

}  // namespace
