#include "buffer/Buffer.h"

namespace buffer {
  Buffer::Buffer(file::FileMgr& fileMgr, logging::LogMgr& logMgr)
    : _fileMgr(fileMgr), _logMgr(logMgr), _page(std::make_unique<file::Page>(fileMgr.blockSize())), 
      _blk(file::BlockId("", -1)) {
  }

  void Buffer::setModified(int txnum, int lsn) {
    _txnum = txnum;
    if (lsn >= 0) _lsn = lsn;
  }

  bool Buffer::isPinned() {
    return _pins > 0;
  }

  int Buffer::modifyingTx() {
    return _txnum;
  }

  void Buffer::assignToBlock(const file::BlockId& blk) {
    flush();
    _blk = blk;
    _fileMgr.read(_blk, *_page);
    _pins = 0;
  }

  void Buffer::flush() {
    if (_txnum >= 0) {
      _logMgr.flush(_lsn);
      _fileMgr.write(_blk, *_page);
      _txnum = -1;
    }
  }

  void Buffer::pin() {
    _pins++;
  }

  void Buffer::unpin() {
    _pins--;
  }
}
