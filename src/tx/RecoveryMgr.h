#pragma once

#include <string>
#include <vector>
#include "tx/Transaction.h"
#include "file/LogMgr.h"
#include "file/Buffer.h"
#include "file/BufferMgr.h"

namespace tx {
  class Transaction; // Forward declaration

  class RecoveryMgr {
    public:
      RecoveryMgr(Transaction& tx, int txNum, file::LogMgr& lm, file::BufferMgr& bm);
      void commit();
      void rollback();
      void recover();
      int setInt(file::Buffer& buf, int offset, int val);
      int setString(file::Buffer& buf, int offset, const std::string& val);
    private:
      file::LogMgr& _lm;
      file::BufferMgr& _bm;
      Transaction& _tx;
      int _txNum;
      void doRollback();
      void doRecover();
  };
}
