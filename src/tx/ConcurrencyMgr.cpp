#include "tx/ConcurrencyMgr.h"

namespace tx {
  LockTable ConcurrencyMgr::_lockTable;

  ConcurrencyMgr::ConcurrencyMgr() {}

  void ConcurrencyMgr::sLock(const file::BlockId& blkId) {
    if (auto it = _locks.find(blkId); it == _locks.end()) {
      _locks[blkId] = "S";
      _lockTable.sLock(blkId);
    }
  }

  void ConcurrencyMgr::xLock(const file::BlockId& blkId) {
    if (!hasXLock(blkId)) {
      sLock(blkId);
      _locks[blkId] = "X";
      _lockTable.xLock(blkId);
    }
  }

  void ConcurrencyMgr::release() {
    for (const auto& [blkId, _] : _locks) {
      _lockTable.unlock(blkId);
    }

    _locks.clear();
  }

  bool ConcurrencyMgr::hasXLock(const file::BlockId& blkId) {
    auto it = _locks.find(blkId);
    if (it == _locks.end()) return false;

    std::string lockType = it->second;
    return lockType == "X";
  }
}
