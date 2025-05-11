#include <algorithm>
#include "tx/Transaction.h"
#include "tx/RecoveryMgr.h"
#include "tx/LogRecord/StartRecord.h"
#include "tx/LogRecord/SetStringRecord.h"
#include "tx/LogRecord/SetIntRecord.h"
#include "tx/LogRecord/LogRecord.h"
#include "tx/LogRecord/CommitRecord.h"
#include "tx/LogRecord/RollbackRecord.h"
#include "tx/LogRecord/CheckPointRecord.h"

namespace tx {
  RecoveryMgr::RecoveryMgr(Transaction& tx, int txNum, file::LogMgr& lm, file::BufferMgr& bm)
    : _tx(tx), _txNum(txNum), _lm(lm), _bm(bm) {
    StartRecord::writeToLog(_lm, _txNum);
  }
  void RecoveryMgr::commit() {
    _bm.flushAll(_txNum);
    int lsn = CommitRecord::writeToLog(_lm, _txNum);
    _lm.flush(lsn);
  }

  void RecoveryMgr::rollback() {
    doRollback();
    _bm.flushAll(_txNum);
    int lsn = RollbackRecord::writeToLog(_lm, _txNum);
    _lm.flush(lsn);
  }

  void RecoveryMgr::recover() {
    doRecover();
    _bm.flushAll(_txNum);
    int lsn = CheckpointRecord::writeToLog(_lm);
    _lm.flush(lsn);
  }

  int RecoveryMgr::setInt(file::Buffer& buf, int offset, int val) {
    int oldVal = buf.contents()->getInt(offset);
    file::BlockId blkId = buf.block();
    return SetIntRecord::writeToLog(_lm, _txNum, blkId, offset, oldVal);
  }

  int RecoveryMgr::setString(file::Buffer& buf, int offset, const std::string& val) {
    std::string oldVal = buf.contents()->getString(offset);
    file::BlockId blkId = buf.block();
    return SetStringRecord::writeToLog(_lm, _txNum, blkId, offset, oldVal);
  }

  void RecoveryMgr::doRollback() {
    auto it = _lm.iterator();
    while (it.hasNext()) {
      auto bytes = it.next();
      std::shared_ptr<LogRecord> logRec = LogRecord::createLogRecord(bytes);
      if (logRec->txNumber() == _txNum) {
        if (logRec->op() == LogRecord::START) {
          return;
        }

        logRec->undo(_tx);
      }
    }
  }

  void RecoveryMgr::doRecover() {
    std::vector<int> finishedTransactions;
    auto it = _lm.iterator();
    while (it.hasNext()) {
      auto bytes = it.next();
      auto logRec = LogRecord::createLogRecord(bytes);
      if (logRec->op() == LogRecord::CHECKPOINT) return;

      if (logRec->op() == LogRecord::COMMIT || logRec->op() == LogRecord::ROLLBACK) {
        finishedTransactions.push_back(logRec->txNumber());
      } else if (std::find(finishedTransactions.begin(), finishedTransactions.end(), logRec->txNumber()) == finishedTransactions.end()) {
        logRec->undo(_tx);
      }
    }
  }
}
