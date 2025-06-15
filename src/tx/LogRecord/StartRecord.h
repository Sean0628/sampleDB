#pragma once

#include <string>
#include "logging/LogMgr.h"
#include "tx/LogRecord/LogRecord.h"
#include "tx/Transaction.h"

namespace tx {
  class StartRecord : public LogRecord {
    public:
      StartRecord(file::Page* p);
      int op() const override { return START; }
      int txNumber() const override { return _txNum; }
      std::string toString() const override {
        return "<START, " + std::to_string(_txNum) + ">";
      }
      void undo(Transaction& tx) override { return; }
      static int writeToLog(logging::LogMgr& lm, int txNum);
    private:
      int _txNum;
  };
}
