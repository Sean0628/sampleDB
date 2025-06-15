#pragma once

#include <string>
#include <filesystem>
#include <iostream>
#include "file/FileMgr.h"
#include "logging/LogMgr.h"
#include "buffer/BufferMgr.h"
#include "tx/Transaction.h"
#include "meta/MetaDataMgr.h"

namespace app {
  class SampleDB {
    public:
      SampleDB() = default;
      SampleDB(std::string dirname, int blocksize, int buffsize);
      void tinySetup(const std::string& dirname, int blocksize, int buffsize);

      file::FileMgr& fileMgr();
      logging::LogMgr& logMgr();
      buffer::BufferMgr& bufferMgr();
      std::unique_ptr<tx::Transaction> newTransaction();
    private:
      std::unique_ptr<file::FileMgr> _fm;
      std::unique_ptr<logging::LogMgr> _lm;
      std::unique_ptr<buffer::BufferMgr> _bm;
      std::unique_ptr<meta::MetaDataMgr> _mdm;
      static const std::string LOGFILE;
  };
}
