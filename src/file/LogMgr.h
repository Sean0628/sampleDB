# pragma once

#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include "file/FileMgr.h"
#include "file/BlockId.h"
#include "file/Page.h"
#include "file/LogIterator.h"

namespace file {
  class LogMgr {
    public:
      LogMgr(file::FileMgr& fileMgr, const std::string& logFileName);
      void flush(int lsn);
      file::LogIterator iterator();
      int append(const std::vector<char>& logRec);
      int getLastSavedLSN() const { return _lastSaveLsn; }
    private:
      file::FileMgr& _fileMgr;
      std::string _logFileName;
      std::shared_ptr<file::Page> _logPage;
      file::BlockId _currBlk;
      int _latestLsn = 0;
      int _lastSaveLsn = 0;
      std::mutex _mutex;

      file::BlockId appendNewBlock();
      void flush();
  };
}
