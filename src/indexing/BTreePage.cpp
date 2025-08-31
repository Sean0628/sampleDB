#include "record/Schema.h"
#include "indexing/BTreePage.h"

namespace indexing {
  BTreePage::BTreePage(tx::Transaction* tx, const file::BlockId& blk, const record::Layout& layout)
      : _tx(tx), _blk(blk), _layout(layout) {
        _tx->pin(_blk);
  }

  int BTreePage::findSlotBefore(const scan::Constant& key) {
    int slot = 0;
    while (slot < getNumRecs() && getDataVal(slot) < key) {
      slot++;
    }

    return slot - 1;
  }

  void BTreePage::close() {
    if (!_blk.isNull()) {
      _tx->unpin(_blk);
    }

    _blk = file::BlockId();
  }

  bool BTreePage::isFull() {
    return slotPos(getNumRecs() + 1) >= _tx->blockSize();
  }

  file::BlockId BTreePage::split(int splitPos, int flag) {
    file::BlockId newBlk = appendNew(flag);
    BTreePage newPage(_tx, newBlk, _layout);
    transferRecs(splitPos, newPage);
    newPage.setFlag(flag); // set layer number
    newPage.close();
    return newBlk;
  }

  scan::Constant BTreePage::getDataVal(int slot) const {
    return getValue(slot, "dataval");
  }

  int BTreePage::getFlag() {
    return _tx->getInt(_blk, 0);
  }

  void BTreePage::setFlag(int val) {
    _tx->setInt(_blk, 0, val, true);
  }

  file::BlockId BTreePage::appendNew(int flag) {
    file::BlockId newBlk = _tx->append(_blk.fileName());
    _tx->pin(newBlk);
    format(newBlk, flag);
    return newBlk;
  }

  void BTreePage::format(const file::BlockId& blk, int flag) {
    _tx->setInt(blk, 0, flag, false); // layer number
    _tx->setInt(blk, sizeof(int), 0, false); // number of records
    int recSize = _layout.slotSize();
    for (int pos = 2 * sizeof(int); pos + recSize <= _tx->blockSize(); pos += recSize) {
      makeDefaultRecord(blk, pos);
    }
  }

  void BTreePage::makeDefaultRecord(const file::BlockId& blk, int pos) {
    for (const auto& fieldName : _layout.schema().fieldNames()) {
      int offset = _layout.offset(fieldName);
      if (_layout.schema().fieldType(fieldName) == record::Schema::INTEGER) {
        _tx->setInt(blk, pos + offset, 0, false);
      } else {
        _tx->setString(blk, pos + offset, "", false);
      }
    }
  }

  // Method called only from BTreeDir
  int BTreePage::getChildNum(int slot) {
    return getInt(slot, "block");
  }

  void BTreePage::insertDir(int slot, const scan::Constant& dataval, int blknum) {
    insert(slot);
    setValue(slot, "dataval", dataval);
    setInt(slot, "block", blknum);
  }

  // Method called only from BTreeLeaf
  record::RID BTreePage::getDataRid(int slot) const {
    return record::RID(getInt(slot, "block"), getInt(slot, "id"));
  }

  void BTreePage::insertLeaf(int slot, const scan::Constant& dataval, const record::RID& datarid) {
    insert(slot);
    setValue(slot, "dataval", dataval);
    setInt(slot, "block", datarid.blockNumber());
    setInt(slot, "id", datarid.slotNumber());
  }

  void BTreePage::remove(int slot) {
    for (int i = slot + 1; i < getNumRecs(); i++) {
      copyRecord(i, i - 1);
    }

    setNumRecs(getNumRecs() - 1);
    return;
  }

  int BTreePage::getNumRecs() const {
    return _tx->getInt(_blk, sizeof(int));
  }

  int BTreePage::getInt(int slot, const std::string& fieldName) const {
    int pos = fldPos(slot, fieldName);
    return _tx->getInt(_blk, pos);
  }

  std::string BTreePage::getString(int slot, const std::string& fieldName) const {
    int pos = fldPos(slot, fieldName);
    return _tx->getString(_blk, pos);
  }

  scan::Constant BTreePage::getValue(int slot, const std::string& fieldName) const {
    int type = _layout.schema().fieldType(fieldName);
    if (type == record::Schema::INTEGER) {
      return scan::Constant(getInt(slot, fieldName));
    } else if (type == record::Schema::VARCHAR) {
      return scan::Constant(getString(slot, fieldName));
    } else {
      throw std::runtime_error("Unsupported field type in BTreePage");
    }
  }

  void BTreePage::setInt(int slot, const std::string& fieldName, int value) {
    int pos = fldPos(slot, fieldName);
    _tx->setInt(_blk, pos, value, true);
  }
  
  void BTreePage::setString(int slot, const std::string& fieldName, const std::string& value) {
    int pos = fldPos(slot, fieldName);
    _tx->setString(_blk, pos, value, true);
  }

  void BTreePage::setValue(int slot, const std::string& fieldName, const scan::Constant& value) {
    int type = _layout.schema().fieldType(fieldName);
    if (type == record::Schema::INTEGER) {
      setInt(slot, fieldName, value.asInt());
    } else if (type == record::Schema::VARCHAR) {
      setString(slot, fieldName, value.asString());
    } else {
      throw std::runtime_error("Unsupported field type in BTreePage");
    }
  }

  void BTreePage::setNumRecs(int numRecs) {
    _tx->setInt(_blk, sizeof(int), numRecs, true);
  }

  void BTreePage::insert(int slot) {
    for (int i = getNumRecs(); i > slot; i--) {
      copyRecord(i - 1, i);
    }
    setNumRecs(getNumRecs() + 1);
  }

  void BTreePage::copyRecord(int from, int to) {
    record::Schema schema = _layout.schema();
    for (const auto& fieldName : schema.fieldNames()) {
      setValue(to, fieldName, getValue(from, fieldName));
    }
  }

  void BTreePage::transferRecs(int slot, BTreePage& dest) {
    int destSlot = 0;
    while (slot < getNumRecs()) {
      dest.insert(destSlot);
      record::Schema schema = _layout.schema();
      for (const auto& fieldName : schema.fieldNames()) {
        dest.setValue(destSlot, fieldName, getValue(slot, fieldName));
      }

      remove(slot);
      destSlot++;
    }
  }

  int BTreePage::fldPos(int slot, const std::string& fieldName) const {
    int offset = _layout.offset(fieldName);
    return slotPos(slot) + offset;
  }

  int BTreePage::slotPos(int slot) const {
    int slotSize = _layout.slotSize();
    return sizeof(int) + sizeof(int)  + (slot * slotSize);
  }

  bool BTreePage::isNull() const {
    return _tx == nullptr;
  }
}
