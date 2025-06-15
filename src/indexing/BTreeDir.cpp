#include "indexing/BTreeDir.h"

namespace indexing {
  BTreeDir::BTreeDir(tx::Transaction& tx, file::BlockId& blk, const record::Layout& layout)
      : _tx(tx), _layout(layout) {
        _contents = BTreePage(&_tx, blk, _layout);
        _fileName = blk.fileName();
  }

  void BTreeDir::close() {
    _contents.close();
  }

  int BTreeDir::search(const scan::Constant& key) {
    file::BlockId childBlk = findChildBlock(key);
    while (_contents.getFlag() > 0) {
      _contents.close();
      _contents = BTreePage(&_tx, childBlk, _layout);
      childBlk = findChildBlock(key);
    }

    return childBlk.number();
  }

  void BTreeDir::makeNewRoot(const DirEntry& dirEntry) {
    scan::Constant firstVal = _contents.getDataVal(0);
    int level = _contents.getFlag();
    file::BlockId newBlk = _contents.split(0, level);
    DirEntry oldRoot(firstVal, newBlk.number());
    insertEntry(oldRoot);
    insertEntry(dirEntry);
    _contents.setFlag(level + 1);
  }

  DirEntry BTreeDir::insert(const DirEntry& dirEntry) {
    if (_contents.getFlag() == 0) return insertEntry(dirEntry);

    file::BlockId childBlk = findChildBlock(dirEntry.dataVal());
    BTreeDir child(_tx, childBlk, _layout);
    DirEntry newEntry = child.insert(dirEntry);
    child.close();
    DirEntry de;
    return (!newEntry.isNull()) ? insertEntry(newEntry) : de;
  }

  DirEntry BTreeDir::insertEntry(const DirEntry& dirEntry) {
    int newSlot = 1 + _contents.findSlotBefore(dirEntry.dataVal());
    _contents.insertDir(newSlot, dirEntry.dataVal(), dirEntry.blockNum());
    if (!_contents.isFull()) {
      DirEntry de;
      return de;
    }

    int level = _contents.getFlag();
    int splitPos = _contents.getNumRecs() / 2;
    scan::Constant splitVal = _contents.getDataVal(splitPos);
    file::BlockId newblk = _contents.split(splitPos, level);
    DirEntry newEntry(splitVal, newblk.number());

    return newEntry;
  }

  file::BlockId BTreeDir::findChildBlock(const scan::Constant& key) {
    int slot = _contents.findSlotBefore(key);
    if (_contents.getDataVal(slot + 1) == key) slot++;

    int blockNum = _contents.getChildNum(slot);
    file::BlockId blk(_fileName, blockNum);
    return blk;
  }
}
