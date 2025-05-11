# pragma once

#include <string>
#include <vector>
#include <memory>
#include "file/Page.h"
#include "file/LogMgr.h"
#include "file/BlockId.h"
#include "tx/Transaction.h"
#include "tx/LogRecord/LogRecord.h"

namespace tx {
  class SetIntRecord : public LogRecord {
    public:
      SetIntRecord(file::Page* p);
      int op() const override { return SETINT; }
      int txNumber() const override { return _txNum; }
      std::string toString() const override;
      void undo(Transaction& tx) override;
      static int writeToLog(file::LogMgr& lm, int txNum, file::BlockId& blkId, int offset, int val);
    private:
      int _txNum;
      file::BlockId _blkId;
      int _offset;
      int _val;
  };
}
