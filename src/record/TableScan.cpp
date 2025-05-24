#include <memory>
#include "record/TableScan.h"

namespace record {
  TableScan::TableScan(tx::Transaction& tx, const std::string& tblname, const Layout& layout)
    : _tx(tx), _layout(layout) {
      _fileName = tblname + ".tbl";
      if (_tx.size(_fileName) == 0) {
        moveToNewBlock();
      } else {
        moveToBlock(0);
      }
  }

  void TableScan::close() {
    if (_recordPage) {
      _tx.unpin(_recordPage->block());
    }
  }

  void TableScan::beforeFirst() {
    moveToBlock(0);
  }

  bool TableScan::next() {
    _currentSlot = _recordPage->nextAfter(_currentSlot);
    while (_currentSlot < 0) {
      if (atEndOfBlock()) return false;

      moveToBlock(_recordPage->block().number()+1);
      _currentSlot = _recordPage->nextAfter(_currentSlot);
    }

    return true;
  }

  int TableScan::getInt(const std::string& fieldName) {
    return _recordPage->getInt(_currentSlot, fieldName);
  }

  std::string TableScan::getString(const std::string& fieldName) {
    return _recordPage->getString(_currentSlot, fieldName);
  }

  scan::Constant TableScan::getValue(const std::string& fieldName) {
    if (_layout.schema().fieldType(fieldName) == Schema::INTEGER) {
      return scan::Constant(getInt(fieldName));
    } else {
      return scan::Constant(getString(fieldName));
    }
  }

  bool TableScan::hasField(const std::string& fieldName) {
    return _layout.schema().hasField(fieldName);
  }

  void TableScan::setInt(const std::string& fieldName, int val) {
    _recordPage->setInt(_currentSlot, fieldName, val);
  }

  void TableScan::setString(const std::string& fieldName, const std::string& val) {
    _recordPage->setString(_currentSlot, fieldName, val);
  }

  void TableScan::setValue(const std::string& fieldName, const scan::Constant& val) {
    if (_layout.schema().fieldType(fieldName) == Schema::INTEGER) {
      setInt(fieldName, val.asInt());
    } else {
      setString(fieldName, val.asString());
    }
  }

  void TableScan::insert() {
    _currentSlot = _recordPage->insertAfter(_currentSlot);
    while (_currentSlot < 0) {
      if (atEndOfBlock()) {
        moveToNewBlock();
      } else {
        moveToBlock(_recordPage->block().number()+1);
      }

      _currentSlot = _recordPage->insertAfter(_currentSlot);
    }
  }

  void TableScan::remove() {
    _recordPage->remove(_currentSlot);
  }

  void TableScan::moveToRid(const record::RID& rid) {
    close();
    file::BlockId blockId(_fileName, rid.blockNumber());
    _recordPage = std::make_unique<RecordPage>(_tx, blockId, _layout);
    _currentSlot = rid.slotNumber();
  }

  RID TableScan::getRid() {
    RID rid(_recordPage->block().number(), _currentSlot);
    return rid;
  }

  void TableScan::moveToBlock(int blknum) {
    close();
    file::BlockId blockId(_fileName, blknum);
    _recordPage = std::make_unique<RecordPage>(_tx, blockId, _layout);
    _currentSlot = -1;
  }

  void TableScan::moveToNewBlock() {
    close();
    file::BlockId blockId = _tx.append(_fileName);
    _recordPage = std::make_unique<RecordPage>(_tx, blockId, _layout);
    _recordPage->format();
    _currentSlot = -1;
  }

  bool TableScan::atEndOfBlock() {
    return _recordPage->block().number() == _tx.size(_fileName) - 1;
  }
}
