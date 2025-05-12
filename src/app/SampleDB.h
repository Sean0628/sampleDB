#pragma once

#include <string>
#include <filesystem>
#include <iostream>
#include "file/FileMgr.h"
#include "logging/LogMgr.h"

namespace app {
  class SampleDB {
    public:
      SampleDB(std::string dirname, int blocksize, int buffsize);
      file::FileMgr& fileMgr();
      logging::LogMgr& logMgr();
    private:
      std::unique_ptr<file::FileMgr> _fm;
      std::unique_ptr<logging::LogMgr> _lm;
      static const std::string LOGFILE;
  };
}
