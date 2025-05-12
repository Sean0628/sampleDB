#pragma once

#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>
#include "file/FileMgr.h"
#include "file/BlockId.h"
#include "logging/LogMgr.h"
#include "buffer/Buffer.h"

namespace buffer {
  class BufferMgr {
    public:
      BufferMgr(file::FileMgr& fileMgr, logging::LogMgr& logMgr, int buffSize);
      int available();
      void flushAll(int txnum);
      void unpin(Buffer& buff);
      Buffer* pin(const file::BlockId& blk);
    private:
      const int MAX_TIME = 10000; // 10 seconds
      std::vector<std::unique_ptr<Buffer>> _bufferpool;
      int _numAvailable;
      std::mutex _mutex;
      std::condition_variable _cv;
      bool waitingTooLong(std::chrono::steady_clock::time_point start);
      Buffer* tryToPin(const file::BlockId& blk);
      Buffer* findExistingBuffer(const file::BlockId& blk);
      Buffer* chooseUnpinnedBuffer();
  };
}
