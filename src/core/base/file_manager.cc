#include "core/base/file_manager.h"

#include <format>
#include <string>
#include <utility>

#include "core/base/file_util.h"
#include "core/check.h"

namespace core {

FileId FileManager::add_file(std::string&& source, std::string&& file_name) {
  DCHECK(!file_name.empty())
      << "file name is empty. use `add_virtual_file` for testing purposes.";

  files_.emplace_back(std::move(file_name), std::move(source));
  return files_.size() - 1;
}

FileId FileManager::add_file(std::string&& file_name) {
  return add_file(read_file(file_name.c_str()), std::move(file_name));
}

FileId FileManager::add_virtual_file(std::string&& source) {
  static std::size_t virtual_file_count = 0;
  std::string virtual_name =
      std::format("virtual_file_{}", virtual_file_count++);

  return add_file(std::move(source), std::move(virtual_name));
}

const File& FileManager::file(FileId id) const {
  DCHECK_GE(id, 0);
  DCHECK_LE(id, files_.size());
  return files_[id];
}

}  // namespace core

