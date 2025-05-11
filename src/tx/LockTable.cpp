#include "tx/LockTable.h"

namespace tx {
  LockTable::LockTable() {}

  void LockTable::sLock(const file::BlockId& blkId) {
    std::unique_lock<std::mutex> lock(_mutex);
    auto startTime = std::chrono::steady_clock::now();

    while (hasXLock(blkId) && !waitingTooLong(startTime)) {
      _cond.wait_for(lock, std::chrono::milliseconds(MAX_LOCKS));
    }

    if (hasXLock(blkId)) {
      throw std::runtime_error("Deadlock detected");
    }

    int lockVal = getLockVal(blkId);
    _lockTable[blkId] = lockVal + 1;
  }

  void LockTable::xLock(const file::BlockId& blkId) {
    std::unique_lock<std::mutex> lock(_mutex);
    auto startTime = std::chrono::steady_clock::now();

    while (hasOtherSLocks(blkId) && !waitingTooLong(startTime)) {
      _cond.wait_for(lock, std::chrono::milliseconds(MAX_LOCKS));
    }

    if (hasOtherSLocks(blkId)) {
      throw std::runtime_error("Deadlock detected");
    }

    _lockTable[blkId] = -1;
  }

  void LockTable::unlock(const file::BlockId& blkId) {
    std::unique_lock<std::mutex> lock(_mutex);

    int lockVal = getLockVal(blkId);
    if (lockVal > 1) {
      _lockTable[blkId] = lockVal - 1;
    } else {
      _lockTable.erase(blkId);
      _cond.notify_all();
    }
  }

  bool LockTable::hasXLock(const file::BlockId& blkId) {
    return getLockVal(blkId) < 0;
  }

  bool LockTable::hasOtherSLocks(const file::BlockId& blkId) {
    return getLockVal(blkId) > 1;
  }

  bool LockTable::waitingTooLong(std::chrono::steady_clock::time_point startTime) {
    auto currentTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
    return duration.count() > MAX_LOCKS;
  }

  int LockTable::getLockVal(const file::BlockId& blkId) {
    auto it = _lockTable.find(blkId);
    if (it != _lockTable.end()) {
      return it->second;
    }
    return 0;
  }
}
