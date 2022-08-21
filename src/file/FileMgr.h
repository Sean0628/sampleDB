#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <map>
#include <cstring>
#include "file/BlockId.h"
#include "file/Page.h"

using namespace std;

namespace file {
  class FileMgr {
    public:
      FileMgr(string dbDirectory, int blocksize);
      int blockSize();
      void write(BlockId& blk, Page& p);
      BlockId append(const string& fileName);
      void read(BlockId& blk, Page& p);
      bool isNew();
      int length(const string& fileName);
    private:
      filesystem::path _dbDirectory;
      int _blocksize;
      mutex _mutex;
      bool _isNew;
      map<filesystem::path, shared_ptr<fstream>> _openFiles;
      shared_ptr<fstream> getFile(const string& fileName);
  };
}
