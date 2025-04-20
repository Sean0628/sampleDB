# pragma once

#include <vector>
#include <memory>
#include "file/FileMgr.h"
#include "file/BlockId.h"

namespace file {
  class LogIterator {
    public:
      LogIterator(file::FileMgr& fileMgr, const file::BlockId& blk);
      bool hasNext();
      std::vector<char> next();
    private:
      file::FileMgr& _fileMgr;
      std::unique_ptr<file::Page> _page;
      file::BlockId _blk;
      int _currPos;
      int _boundary;
      void moveToBlock(file::BlockId& blk);
  };
}
