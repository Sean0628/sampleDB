#include "app/SampleDB.h"

namespace app {
  const std::string SampleDB::LOGFILE = "sampledb.log";

  SampleDB::SampleDB(std::string dirname, int blocksize, int buffsize) {
    auto path = std::filesystem::current_path() / dirname;
    _fm = std::make_unique<file::FileMgr>(path, blocksize);
    _lm = std::make_unique<logging::LogMgr>(*_fm, LOGFILE);

    bool isNew = _fm->isNew();
    if (isNew)
      std::cout << "creating new database" << std::endl;
    else {
      std::cout << "recovering existing database" << std::endl;
    }
  }

  file::FileMgr& SampleDB::fileMgr() {
    return *_fm;
  }

  logging::LogMgr& SampleDB::logMgr() {
    return *_lm;
  }
}
