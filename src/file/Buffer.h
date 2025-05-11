#pragma once

#include <memory>
#include "file/FileMgr.h"
#include "file/BlockId.h"
#include "file/Page.h"
#include "file/LogMgr.h"

namespace file {
  class Buffer {
    public:
      Buffer(file::FileMgr& fileMgr, file::LogMgr& logMgr);
      file::Page* contents() {
        return _page.get();
      }
      file::BlockId block() {
        return _blk;
      }
      void setModified(int txnum, int lsn);
      bool isPinned();
      int modifyingTx();
      void assignToBlock(const file::BlockId& blk);
      void flush();
      void pin();
      void unpin();
    private:
      file::FileMgr& _fileMgr;
      file::LogMgr& _logMgr;
      std::unique_ptr<file::Page> _page;
      file::BlockId _blk;
      int _pins = 0;
      int _txnum = -1;
      int _lsn = -1;
  };
}
