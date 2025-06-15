#pragma once

#include "scan/Constant.h"

namespace indexing {
  class DirEntry {
    public:
      DirEntry() = default;
      DirEntry(const scan::Constant& dataval, int blknum);
      scan::Constant dataVal() const;
      int blockNum() const;
      bool isNull() const;
    private:
      scan::Constant _dataVal;
      int _blockNum;
  };
}
