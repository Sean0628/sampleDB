#pragma once

#include <memory>
#include <string>
#include <vector>

#include "BufferNeeds.h"
#include "ChunkScan.h"
#include "record/Layout.h"
#include "record/Schema.h"
#include "tx/Transaction.h"
#include "scan/Constant.h"
#include "scan/Scan.h"
#include "scan/ProductScan.h"

namespace multibuffer {
  class MultiBufferProductScan : public scan::Scan {
    public:
      MultiBufferProductScan(const std::shared_ptr<scan::Scan>& scan1,
                              const std::string& fileName,
                              const record::Layout& layout,
                              tx::Transaction* tx);
      void beforeFirst() override;
      bool next() override;
      void close() override;
      scan::Constant getVal(const std::string& fldname) const override;
      int getInt(const std::string& fldname) const override;
      std::string getString(const std::string& fldname) const override;
      bool hasField(const std::string& fldname) const override;
    private:
      bool useNextChunk();
      tx::Transaction* _tx;
      std::shared_ptr<scan::Scan> _scan1, _scan2, _productScan;
      std::string _fileName;
      record::Layout _layout;
      int _chunkSize, _nextBlkNum, _fileSize;
  };
}
