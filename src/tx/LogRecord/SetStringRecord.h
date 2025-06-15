#pragma once

#include <string>
#include <vector>
#include <memory>
#include "file/Page.h"
#include "logging/LogMgr.h"
#include "tx/Transaction.h"
#include "tx/LogRecord/LogRecord.h"

namespace tx {
  class SetStringRecord : public LogRecord {
    public:
      SetStringRecord(file::Page* p);
      int op() const override { return SETSTRING; }
      int txNumber() const override { return _txNum; }
      std::string toString() const override;
      void undo(Transaction& tx) override;
      static int writeToLog(logging::LogMgr& lm, int txNum, file::BlockId& blkId, int offset, const std::string& str);
    private:
      int _txNum;
      file::BlockId _blkId;
      int _offset;
      std::string _str;
  };
}
