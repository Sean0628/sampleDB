# pragma once

#include <string>
#include <vector>
#include <memory>
#include "file/Page.h"
#include "file/LogMgr.h"
#include "tx/Transaction.h"

namespace tx {
  class LogRecord {
    public:
      virtual ~LogRecord() = default;
      virtual int op() const = 0;
      virtual int txNumber() const = 0;
      virtual std::string toString() const = 0;
      virtual void undo(tx::Transaction& tx) = 0;
      static std::shared_ptr<LogRecord> createLogRecord(const std::vector<char>& bytes);
      static const int CHECKPOINT = 0, START = 1, COMMIT = 2, ROLLBACK = 3, SETINT = 4, SETSTRING = 5;

  };
}
