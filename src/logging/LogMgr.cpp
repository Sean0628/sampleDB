#include "logging/LogMgr.h"

namespace logging {
  LogMgr::LogMgr(file::FileMgr& fileMgr, const std::string& logFileName)
    : _fileMgr(fileMgr), _logFileName(logFileName), _currBlk(file::BlockId(logFileName, 0)) {
    auto buffer = std::make_shared<std::vector<char>>(_fileMgr.blockSize(), 0);
    _logPage = std::make_shared<file::Page>(buffer);

    int logFileLength = fileMgr.length(logFileName);
    if (logFileLength == 0) {
      _currBlk = appendNewBlock();
    } else {
      _currBlk = file::BlockId(logFileName, logFileLength - 1);
      _fileMgr.read(_currBlk, *_logPage);
    }
  }

  void LogMgr::flush(int lsn) {
    std::unique_lock<std::mutex> lock(_mutex);
    if (lsn >= _lastSaveLsn) this->flush();
  }

  LogIterator LogMgr::iterator() {
    flush();
    return LogIterator(_fileMgr, _currBlk);
  }

  int LogMgr::append(const std::vector<char>& logRec) {
    std::unique_lock<std::mutex> lock(_mutex);
    int boundary = _logPage->getInt(0);
    int recSize = logRec.size();
    int bytesNeeded = recSize + sizeof(uint32_t);

    if (boundary - bytesNeeded < static_cast<int>(sizeof(uint32_t))) {
      flush();
      _currBlk = appendNewBlock();
      boundary = _logPage->getInt(0);
    }

    int recPos = boundary - bytesNeeded;
    _logPage->setBytes(recPos, logRec);
    _logPage->setInt(0, recPos);
    _latestLsn++;
    return _latestLsn;
  }

  file::BlockId LogMgr::appendNewBlock() {
    file::BlockId blk = _fileMgr.append(_logFileName);
    _logPage->setInt(0, _fileMgr.blockSize());
    _fileMgr.write(blk, *_logPage);
    return blk;
  }

  void LogMgr::flush() {
    _fileMgr.write(_currBlk, *_logPage);
    _lastSaveLsn = _latestLsn;
  }
}
