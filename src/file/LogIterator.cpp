#include <memory>
#include <vector>

#include "file/LogIterator.h"

file::LogIterator::LogIterator(file::FileMgr& fileMgr, const file::BlockId& blk)
  : _fileMgr(fileMgr), _blk(blk) {
  _page = std::make_unique<file::Page>(_fileMgr.blockSize());
  moveToBlock(_blk);
}

bool file::LogIterator::hasNext() {
  return _currPos < _fileMgr.blockSize() || _blk.number() > 0;
}

std::vector<char> file::LogIterator::next() {
  if (_currPos == _fileMgr.blockSize()) {
    _blk = file::BlockId(_blk.fileName(), _blk.number() - 1);
    moveToBlock(_blk);
  }

  std::vector<char> logRec = _page->getBytes(_currPos);
  _currPos += sizeof(int) + logRec.size();
  return logRec;
}

void file::LogIterator::moveToBlock(file::BlockId& blk) {
  _fileMgr.read(blk, *_page);
  _boundary = _page->getInt(0);
  _currPos = _boundary;
}
