#pragma once

#include <string>
#include <map>
#include <mutex>
#include <memory>
#include "tx/LockTable.h"
#include "file/BlockId.h"

namespace tx {
  class ConcurrencyMgr {
  public:
    ConcurrencyMgr();
    void sLock(const file::BlockId& blkId);
    void xLock(const file::BlockId& blkId);
    void release();
  private:
    std::map<file::BlockId, string> _locks;
    static LockTable _lockTable;
    bool hasXLock(const file::BlockId& blkId);
  };
}
