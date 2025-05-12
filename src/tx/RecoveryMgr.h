#pragma once

#include <string>
#include <vector>
#include "tx/Transaction.h"
#include "logging/LogMgr.h"
#include "buffer/Buffer.h"
#include "buffer/BufferMgr.h"

namespace tx {
  class Transaction; // Forward declaration

  class RecoveryMgr {
    public:
      RecoveryMgr(Transaction& tx, int txNum, logging::LogMgr& lm, buffer::BufferMgr& bm);
      void commit();
      void rollback();
      void recover();
      int setInt(buffer::Buffer& buf, int offset, int val);
      int setString(buffer::Buffer& buf, int offset, const std::string& val);
    private:
      logging::LogMgr& _lm;
      buffer::BufferMgr& _bm;
      Transaction& _tx;
      int _txNum;
      void doRollback();
      void doRecover();
  };
}
