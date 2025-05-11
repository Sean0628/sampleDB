#pragma once

#include <string>
#include <map>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include "file/BlockId.h"

namespace tx {
  class LockTable {
  public:
    LockTable();
    void sLock(const file::BlockId& blkId);
    void xLock(const file::BlockId& blkId);
    void unlock(const file::BlockId& blkId);
  private:
    const int MAX_LOCKS = 10000; // 10 seconds
    std::mutex _mutex;
    std::map<file::BlockId, int> _lockTable;
    std::condition_variable _cond;

    bool hasXLock(const file::BlockId& blkId);
    bool hasOtherSLocks(const file::BlockId& blkId);
    bool waitingTooLong(std::chrono::steady_clock::time_point startTime);
    int getLockVal(const file::BlockId& blkId);
  };
}
