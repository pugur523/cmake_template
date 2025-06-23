#include <fstream>
#include <string>
#include <vector>

#include "benchmark/benchmark.h"
#include "core/base/file_util.h"

namespace {

// Setup temporary test files/directories
template <typename F>
void with_temp_file(const std::string& content, const F& fn) {
  const std::string tmp_path = "./tmp_test_file.txt";
  std::ofstream ofs(tmp_path);
  ofs << content;
  ofs.close();

  fn(tmp_path);

  core::remove_file(tmp_path.c_str());
}

template <typename F>
void with_temp_dir(const F& fn) {
  const std::string tmp_dir = "./tmp_test_dir";
  core::create_directory(tmp_dir.c_str());
  fn(tmp_dir);
  core::remove_directory(tmp_dir.c_str());
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

    for (int i = 0; i < 10; ++i) {
      std::string file = dir + "/file" + std::to_string(i);
      core::remove_file(file.c_str());
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

static void file_util_remove_file(benchmark::State& state) {
  for (auto _ : state) {
    const std::string tmp = "./tmp_to_remove.txt";
    std::ofstream(tmp) << "delete me";
    benchmark::DoNotOptimize(core::remove_file(tmp.c_str()));
  }
}
BENCHMARK(file_util_remove_file);

static void file_util_rename_file(benchmark::State& state) {
  for (auto _ : state) {
    const std::string old_name = "./tmp_old_name.txt";
    const std::string new_name = "./tmp_new_name.txt";
    std::ofstream(old_name) << "rename me";
    core::rename_file(old_name.c_str(), new_name.c_str());
    core::remove_file(new_name.c_str());
  }
}
BENCHMARK(file_util_rename_file);

static void file_util_create_directory(benchmark::State& state) {
  for (auto _ : state) {
    const std::string dir = "./tmp_bench_dir";
    core::create_directory(dir.c_str());
    core::remove_directory(dir.c_str());
  }
}
BENCHMARK(file_util_create_directory);

}  // namespace
