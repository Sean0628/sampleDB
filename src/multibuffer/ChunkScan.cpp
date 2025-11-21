#include "ChunkScan.h"

namespace multibuffer {
  ChunkScan::ChunkScan(tx::Transaction* tx, const std::string& fileName, const record::Layout& layout, int startBlkNum, int endBlkNum)
    : _tx(tx), _fileName(fileName), _layout(layout), _startBlkNum(startBlkNum), _endBlkNum(endBlkNum) {
      for (int i = startBlkNum; i <= endBlkNum; i++) {
        file::BlockId blk(fileName, i);
        _buffers.push_back(std::make_unique<record::RecordPage>(tx, blk, layout));
      }

      moveToBlock(startBlkNum);
  }

  void ChunkScan::close() {
    for (int i = 0; i < buffers.size(); i++) {
      file::BlockId blk(_fileName, _startBlkNum + i);
      _tx->unpin(blk);
    }
  }

  void ChunkScan::beforeFirst() {
    moveToBlock(_startBlkNum);
  }

  bool ChunkScan::next() {
    _currentSlot = _currentPage->nextAfter(_currentSlot);
    while (_currentSlot < 0) {
      if (_currentBlkNum == _endBlkNum) return false;

      moveToBlock(_currentPage->block().number() + 1);
      _currentSlot = _currentPage->nextAfter(_currentSlot);
    }

    return true;
  }

  int ChunkScan::getInt(const std::string& fldname) const {
    return _currentPage->getInt(_currentSlot, fldname);
  }

  std::string ChunkScan::getString(const std::string& fldname) const {
    return _currentPage->getString(_currentSlot, fldname);
  }

  scan::Constant ChunkScan::getVal(const std::string& fldname) const {
    if (_layout.schema().type(fldname) == record::Schema::Type::INTEGER) {
      return scan::Constant(getInt(fldname));
    } else {
      return scan::Constant(getString(fldname));
    }
  }

  bool ChunkScan::hasField(const std::string& fldname) const {
    return _layout.schema().hasField(fldname);
  }

  void ChunkScan::moveToBlock(int blkNum) {
    _currentBlkNum = blkNum;
    _currentPage = _buffers[_currentBlkNum - _startBlkNum].get();
    _currentSlot = -1;
  }
}
