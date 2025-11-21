#include "MultiBufferProductScan.h"

namespace multibuffer {
  MultiBufferProductScan::MultiBufferProductScan(const std::shared_ptr<scan::Scan>& scan1,
                                                 const std::string& fileName,
                                                 const record::Layout& layout,
                                                 tx::Transaction* tx)
    : _tx(tx), _scan1(scan1), _fileName(fileName), _layout(layout) {
      _fileSize = _tx->size(fileName);
      int avail = _tx->availableBuffers();
      _chunkSize = BufferNeeds::bestFactor(avail, _fileSize);
      beforeFirst();
  }

  void MultiBufferProductScan::beforeFirst() {
    _nextBlkNum = 0;
    useNextChunk();
  }

  bool MultiBufferProductScan::next() {
    while (!_productScan->next()) {
      if (!useNextChunk()) return false;
    }

    return true;
  }

  void MultiBufferProductScan::close() {
    _productScan->close();
  }

  int MultiBufferProductScan::getInt(const std::string& fldname) const {
    return _productScan->getInt(fldname);
  }

  std::string MultiBufferProductScan::getString(const std::string& fldname) const {
    return _productScan->getString(fldname);
  }

  scan::Constant MultiBufferProductScan::getVal(const std::string& fldname) const {
    return _productScan->getVal(fldname);
  }

  bool MultiBufferProductScan::hasField(const std::string& fldname) const {
    return _productScan->hasField(fldname);
  }

  bool MultiBufferProductScan::useNextChunk() {
    if (_scan2) {
      _scan2->close();
    }

    if (_nextBlkNum >= _fileSize) {
      return false;
    }

    int end = _nextBlkNum + _chunkSize - 1;
    if (end >= _fileSize) {
      end = _fileSize - 1;
    }

    _scan2 = std::static_pointer_cast<scan::Scan>(std::make_shared<ChunkScan>(_tx, _fileName, _layout, _nextBlkNum, end));
    _scan1->beforeFirst();
    _productScan = std::static_pointer_cast<scan::Scan>(std::make_shared<scan::ProductScan>(_scan1, _scan2));
    _nextBlkNum = end + 1;

    return true;
  }
}
