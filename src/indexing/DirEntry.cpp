#include "indexing/DirEntry.h"

namespace indexing {
  DirEntry::DirEntry(const scan::Constant& dataval, int blknum)
    : _dataVal(dataval), _blockNum(blknum) {}

  scan::Constant DirEntry::dataVal() const {
    return _dataVal;
  }

  int DirEntry::blockNum() const {
    return _blockNum;
  }

  bool DirEntry::isNull() const {
    return _dataVal.isNull();
  }
}
