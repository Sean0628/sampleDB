#pragma once

#include <string>
#include <filesystem>
#include <iostream>
#include "file/FileMgr.h"
#include "indexing/IndexUpdatePlanner.h"
#include "logging/LogMgr.h"
#include "buffer/BufferMgr.h"
#include "tx/Transaction.h"
#include "meta/MetaDataMgr.h"
#include "plan/Planner.h"
#include "plan/BasicQueryPlanner.h"
#include "plan/UpdatePlanner.h"

namespace app {
  class SampleDB {
    public:
      SampleDB() = default;
      SampleDB(std::string dirname, int blocksize, int buffsize);
      SampleDB(std::string dirname);
      void tinySetup(const std::string& dirname, int blocksize, int buffsize);

      file::FileMgr& fileMgr();
      logging::LogMgr& logMgr();
      buffer::BufferMgr& bufferMgr();
      meta::MetaDataMgr& getMetaDataManager();
      plan::Planner& getPlanner();
      std::unique_ptr<tx::Transaction> newTransaction();
    private:
      std::unique_ptr<file::FileMgr> _fm;
      std::unique_ptr<logging::LogMgr> _lm;
      std::unique_ptr<buffer::BufferMgr> _bm;
      std::unique_ptr<meta::MetaDataMgr> _mdm;
      std::unique_ptr<plan::Planner> _planner;

      static const int _blocksize;
      static const int _buffsize;
      static const std::string LOGFILE;
  };
}
