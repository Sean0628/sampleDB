#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <map>
#include <cstring>
#include "file/BlockId.h"
#include "file/Page.h"

namespace file {
  class FileMgr {
    public:
      FileMgr(std::string dbDirectory, int blocksize);
      int blockSize();
      void write(BlockId& blk, Page& p);
      BlockId append(const std::string& fileName);
      void read(BlockId& blk, Page& p);
      bool isNew();
      int length(const std::string& fileName);
    private:
      std::filesystem::path _dbDirectory;
      int _blocksize;
      std::mutex _mutex;
      bool _isNew;
      std::map<std::filesystem::path, std::shared_ptr<std::fstream>> _openFiles;
      std::shared_ptr<std::fstream> getFile(const std::string& fileName);
  };
}
