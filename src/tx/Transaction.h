#pragma once

#include <string>
#include <memory>
#include <mutex>
#include "tx/BufferList.h"
#include "tx/ConcurrencyMgr.h"
#include "tx/RecoveryMgr.h"
#include "buffer/BufferMgr.h"
#include "file/FileMgr.h"
#include "logging/LogMgr.h"

namespace tx {
  class Transaction {
    public:
      Transaction(file::FileMgr& fm, logging::LogMgr& lm, buffer::BufferMgr& bm);
      void commit();
      void rollback();
      void recover();
      void pin(const file::BlockId& blkId);
      void unpin(const file::BlockId& blkId);
      int getInt(file::BlockId& blkId, int offset);
      std::string getString(file::BlockId& blkId, int offset);
      void setInt(const file::BlockId& blkId, int offset, int val, bool okToLog = true);
      void setString(const file::BlockId& blkId, int offset, const std::string& val, bool okToLog = true);
      int size(const std::string& fileName);
      file::BlockId append(const std::string& fileName);
      int blockSize();
      int availableBuffers();
      int getTransactionNumber() const { return _txNum; }
      void forceConcurrencyRelease(); 
    private:
      static int _nextTxNum;
      static const int END_OF_FILE = -1;
      std::unique_ptr<RecoveryMgr> _recoveryMgr;
      std::unique_ptr<ConcurrencyMgr> _concurrencyMgr;
      file::FileMgr& _fm;
      logging::LogMgr& _lm;
      buffer::BufferMgr& _bm;
      int _txNum;
      std::unique_ptr<BufferList> _buffers;
      static std::mutex _mutex;
      static int newTransactionNumber();
  };
}
