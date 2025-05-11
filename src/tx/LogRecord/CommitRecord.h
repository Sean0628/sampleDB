# pragma once

#include <string>
#include <memory>
#include "file/Page.h"
#include "file/LogMgr.h"
#include "tx/Transaction.h"
#include "tx/LogRecord/LogRecord.h"

namespace tx {
  class CommitRecord : public LogRecord {
    public:
      CommitRecord(file::Page* p);
      int op() const override { return COMMIT; }
      int txNumber() const override { return _txNum; }
      std::string toString() const override {
        return "<COMMIT, " + std::to_string(_txNum) + ">";
      }
      void undo(Transaction& tx) override { return; }
      static int writeToLog(file::LogMgr& lm, int txNum);
    private:
      int _txNum;
  };
}
