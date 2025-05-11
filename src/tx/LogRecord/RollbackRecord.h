# pragma once

#include <string>
#include "file/LogMgr.h"
#include "tx/LogRecord/LogRecord.h"
#include "tx/Transaction.h"

namespace tx {
  class RollbackRecord : public LogRecord {
    public:
      RollbackRecord(file::Page* p);
      int op() const override { return ROLLBACK; }
      int txNumber() const override { return _txNum; }
      std::string toString() const override {
        return "<ROLLBACK, " + std::to_string(_txNum) + ">";
      }
      void undo(Transaction& tx) override { return; }
      static int writeToLog(file::LogMgr& lm, int txNum);
    private:
      int _txNum;
  };
}
