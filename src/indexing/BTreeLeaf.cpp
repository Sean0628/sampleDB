#include "BTreeLeaf.h"

namespace indexing {
  BTreeLeaf::BTreeLeaf(tx::Transaction* tx, const file::BlockId& blk, const record::Layout& layout, const scan::Constant& key)
      : _tx(tx), _layout(layout), _key(key) {
        _contents = BTreePage(_tx, blk, _layout);
        _currentSlot = _contents.findSlotBefore(_key);
        _fileName = blk.fileName();
      }

  void BTreeLeaf::close() {
    _contents.close();
  }

  bool BTreeLeaf::next() {
    _currentSlot++;
    if (_currentSlot >= _contents.getNumRecs()) {
      return tryOverflow();
    } else if (_contents.getDataVal(_currentSlot) == _key) {
      return true;
    }

    return tryOverflow();
  }

  record::RID BTreeLeaf::getDataRid() const {
    return _contents.getDataRid(_currentSlot);
  }

  void BTreeLeaf::remove(const record::RID& datarid) {
    while (next()) {
      if (getDataRid().equals(datarid)) {
        _contents.remove(_currentSlot);
        return;
      }
    }
  }

  DirEntry BTreeLeaf::insert(const record::RID& datarid) {
    if (_contents.getFlag() >= 0 && _contents.getDataVal(0) > _key) {
      // page is still empty
      scan::Constant firstVal = _contents.getDataVal(0);
      file::BlockId newBlk = _contents.split(0, _contents.getFlag());
      _currentSlot = 0;
      _contents.setFlag(-1);
      _contents.insertLeaf(_currentSlot, _key, datarid);
      DirEntry dirEntry(firstVal, newBlk.number());
      return dirEntry;
    }

    _currentSlot++;
    _contents.insertLeaf(_currentSlot, _key, datarid);
    if (!_contents.isFull()) {
      DirEntry dirEntry;
      return dirEntry;
    }

    // else page is full, so split it
    scan::Constant firstKey = _contents.getDataVal(0);
    scan::Constant lastKey = _contents.getDataVal(_contents.getNumRecs() - 1);
    if (lastKey == firstKey) {
      // create an overflow block to hold all but the first record
      file::BlockId newBlk = _contents.split(1, _contents.getFlag());
      _contents.setFlag(newBlk.number());
      DirEntry dirEntry;
      return dirEntry;
    } else {
      int splitPos = _contents.getNumRecs() / 2;
      scan::Constant splitKey = _contents.getDataVal(splitPos);
      if (splitKey == firstKey) {
        // move right, looking for the next key
        while (_contents.getDataVal(splitPos) == splitKey) {
          splitPos++;
        }
        splitKey = _contents.getDataVal(splitPos);
      } else {
        // move left, looking for first entry having that key
        while (_contents.getDataVal(splitPos - 1) == splitKey) {
          splitPos--;
        }
      }

      file::BlockId newBlk = _contents.split(splitPos, -1);
      DirEntry dirEntry(splitKey, newBlk.number());
      return dirEntry;
    }
  }

  bool BTreeLeaf::tryOverflow() {
    scan::Constant firstKey = _contents.getDataVal(0);
    int flag = _contents.getFlag();
    if (!(_key == firstKey) || flag < 0) {
      return false;
    }

    _contents.close();
    file::BlockId nextBlk(_fileName, flag);
    _contents = BTreePage(_tx, nextBlk, _layout);
    _currentSlot = 0;
    return true;
  }

  bool BTreeLeaf::isNull() const {
    return _contents.isNull();
  }
}
