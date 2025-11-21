#pragma once

#include <memory>
#include <string>
#include <vector>

#include "file/BlockId.h"
#include "record/Layout.h"
#include "record/RecrodPage.h"
#include "scan/Scan.h"
#include "scan/Constant.h"
#include "tx/Transaction.h"

namespace multibuffer {
  class ChunkScan : public scan::Scan {
    public:
      ChunkScan(tx::Transaction* tx, const std::string& fileName, const record::Layout& layout, int startBlkNum, int endBlkNum);
      void close() override;
      void beforeFirst() override;
      bool next() override;
      int getInt(const std::string& fldname) const override;
      std::string getString(const std::string& fldname) const override;
      scan::Constant getVal(const std::string& fldname) const override;
      bool hasField(const std::string& fldname) const override;
    private:
      void moveToBlock(int blkNum);
      std::vector<std::unique_ptr<record::RecordPage>> _buffers;
      tx::Transaction* _tx;
      std::string _fileName;
      record::Layout _layout;
      int _startBlkNum, _endBlkNum, _currentBlkNum;
      record::RecordPage* _currentPage;
      int _currentSlot;
  };
}
