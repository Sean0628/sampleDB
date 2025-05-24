#include "tx/Transaction.h"

namespace tx {
  int Transaction::_nextTxNum = 0;
  std::mutex Transaction::_mutex;

  Transaction::Transaction(file::FileMgr& fm, logging::LogMgr& lm, buffer::BufferMgr& bm)
    : _fm(fm), _lm(lm), _bm(bm) {
    _txNum = newTransactionNumber();
    _recoveryMgr = std::make_unique<RecoveryMgr>(*this, _txNum, lm, bm);
    _concurrencyMgr = std::make_unique<ConcurrencyMgr>();
    _buffers = std::make_unique<BufferList>(bm);
  }

  void Transaction::commit() {
    _recoveryMgr->commit();
    _concurrencyMgr->release();
    _buffers->unpinAll();
  }

  void Transaction::rollback() {
    _recoveryMgr->rollback();
    _concurrencyMgr->release();
    _buffers->unpinAll();
  }

  void Transaction::recover() {
    _bm.flushAll(_txNum);
    _recoveryMgr->recover();
    _concurrencyMgr->release();
    _buffers->unpinAll();
  }

  void Transaction::pin(const file::BlockId& blkId) {
    _buffers->pin(blkId);
  }

  void Transaction::unpin(const file::BlockId& blkId) {
    _buffers->unpin(blkId);
  }

  int Transaction::getInt(const file::BlockId& blkId, int offset) {
    _concurrencyMgr->sLock(blkId);
    buffer::Buffer& buf = _buffers->getBuffer(blkId);
    return buf.contents()->getInt(offset);
  }

  std::string Transaction::getString(const file::BlockId& blkId, int offset) {
    _concurrencyMgr->sLock(blkId);
    buffer::Buffer& buf = _buffers->getBuffer(blkId);
    return buf.contents()->getString(offset);
  }

  void Transaction::setInt(const file::BlockId& blkId, int offset, int val, bool okToLog) {
    _concurrencyMgr->xLock(blkId);
    buffer::Buffer& buf = _buffers->getBuffer(blkId);
    int lsn = -1;
    if (okToLog) {
      lsn = _recoveryMgr->setInt(buf, offset, val);
    }

    auto page = buf.contents();
    page->setInt(offset, val);
    buf.setModified(_txNum, lsn);
  }

  void Transaction::setString(const file::BlockId& blkId, int offset, const std::string& val, bool okToLog) {
    _concurrencyMgr->xLock(blkId);
    buffer::Buffer& buf = _buffers->getBuffer(blkId);
    int lsn = -1;
    if (okToLog) {
      lsn = _recoveryMgr->setString(buf, offset, val);
    }

    auto page = buf.contents();
    page->setString(offset, val);
    buf.setModified(_txNum, lsn);
  }

  int Transaction::size(const std::string& fileName) {
    file::BlockId blkId(fileName, END_OF_FILE);
    _concurrencyMgr->sLock(blkId);
    return _fm.length(fileName);
  }

  file::BlockId Transaction::append(const std::string& fileName) {
    file::BlockId blkId(fileName, END_OF_FILE);
    _concurrencyMgr->xLock(blkId);
    return _fm.append(fileName);
  }

  int Transaction::blockSize() {
    return _fm.blockSize();
  }

  int Transaction::availableBuffers() {
    return _bm.available();
  }

  void Transaction::forceConcurrencyRelease() {
    _concurrencyMgr->release();
    _buffers->unpinAll();
  }

  int Transaction::newTransactionNumber() {
    std::lock_guard<std::mutex> lock(_mutex);
    _nextTxNum++;
    return _nextTxNum;
  }
}
