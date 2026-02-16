#include "record/RecordPage.h"

namespace record {
  RecordPage::RecordPage(tx::Transaction& tx, const file::BlockId& blk, const Layout& layout)
    : _tx(tx), _blk(blk), _layout(layout) {
    _tx.pin(_blk);
    }

  int RecordPage::getInt(int slot, const std::string& fieldName) const {
    int fieldPos = offset(slot) + _layout.offset(fieldName);
    return _tx.getInt(_blk, fieldPos);
  }

  std::string RecordPage::getString(int slot, const std::string& fieldName) const {
    int fieldPos = offset(slot) + _layout.offset(fieldName);
    return _tx.getString(_blk, fieldPos);
  }

  void RecordPage::setInt(int slot, const std::string& fieldName, int val) {
    int fieldPos = offset(slot) + _layout.offset(fieldName);
    _tx.setInt(_blk, fieldPos, val, true);
  }

  void RecordPage::setString(int slot, const std::string& fieldName, const std::string& val) {
    int fieldPos = offset(slot) + _layout.offset(fieldName);
    _tx.setString(_blk, fieldPos, val, true);
  }

  void RecordPage::remove(int slot) {
    setFlag(slot, EMPTY);
  }

  void RecordPage::format() {
    int slot = 0;
    while (isValidSlot(slot)) {
      _tx.setInt(_blk, offset(slot), EMPTY);
      const Schema& schema = _layout.schema();

      for (const auto& fieldName : schema.fieldNames()) {
        int fieldPos = offset(slot) + _layout.offset(fieldName);

        if (schema.fieldType(fieldName) == Schema::INTEGER || schema.fieldType(fieldName) == Schema::BOOLEAN) {
          _tx.setInt(_blk, fieldPos, 0, false);
        } else if (schema.fieldType(fieldName) == Schema::VARCHAR) {
          _tx.setString(_blk, fieldPos, "", false);
        }
      }

      slot++;
    }
  }

  int RecordPage::nextAfter(int slot) const {
    return searchAfter(slot, USED);
  }

  int RecordPage::insertAfter(int slot) {
    int newSlot = searchAfter(slot, EMPTY);
    if (newSlot >= 0) setFlag(newSlot, USED);

    return newSlot;
  }

  file::BlockId RecordPage::block() const {
    return _blk;
  }

  void RecordPage::setFlag(int slot, int flag) {
    _tx.setInt(_blk, offset(slot), flag, true);
  }

  int RecordPage::searchAfter(int slot, int flag) const {
    slot++;
    while (isValidSlot(slot)) {
      if (_tx.getInt(_blk, offset(slot)) == flag) {
        return slot;
      }
      slot++;
    }

    return -1;
  }

  bool RecordPage::isValidSlot(int slot) const {
    return offset(slot+1) <= _tx.blockSize();
  }

  int RecordPage::offset(int slot) const {
    return slot * _layout.slotSize();
  }
}
