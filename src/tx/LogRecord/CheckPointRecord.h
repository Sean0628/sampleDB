# pragma once

#include <string>
#include "file/LogMgr.h"
#include "tx/LogRecord/LogRecord.h"
#include "tx/Transaction.h"

namespace tx {
  class CheckpointRecord : public LogRecord {
    public:
      CheckpointRecord();
      int op() const override { return CHECKPOINT; }
      int txNumber() const override { return -1; }
      std::string toString() const override { return "<CHECKPOINT>"; }
      void undo(Transaction& tx) override { return; }
      static int writeToLog(file::LogMgr& lm);
  };
}
