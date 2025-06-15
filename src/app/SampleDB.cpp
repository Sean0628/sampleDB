#include "app/SampleDB.h"

namespace app {
  const std::string SampleDB::LOGFILE = "sampledb.log";

  SampleDB::SampleDB(std::string dirname, int blocksize, int buffsize) {
    auto path = std::filesystem::current_path() / dirname;
    _fm = std::make_unique<file::FileMgr>(path, blocksize);
    _lm = std::make_unique<logging::LogMgr>(*_fm, LOGFILE);
    _bm = std::make_unique<buffer::BufferMgr>(*_fm, *_lm, buffsize);

    auto txPtr = std::make_unique<tx::Transaction>(*_fm, *_lm, *_bm);
    bool isNew = _fm->isNew();
    if (isNew)
      std::cout << "creating new database" << std::endl;
    else {
      std::cout << "recovering existing database" << std::endl;
      txPtr->recover();
    }

    _mdm = std::make_unique<meta::MetaDataMgr>(isNew, *txPtr);
    txPtr->commit();
  }

  void SampleDB::tinySetup(const std::string& dirname, int blocksize, int buffsize) {
    auto path = std::filesystem::current_path() / dirname;

    _fm = std::make_unique<file::FileMgr>(path, blocksize);
    _lm = std::make_unique<logging::LogMgr>(*_fm, LOGFILE);
    _bm = std::make_unique<buffer::BufferMgr>(*_fm, *_lm, buffsize);
  }

  file::FileMgr& SampleDB::fileMgr() {
    return *_fm;
  }

  logging::LogMgr& SampleDB::logMgr() {
    return *_lm;
  }

  buffer::BufferMgr& SampleDB::bufferMgr() {
    return *_bm;
  }

  std::unique_ptr<tx::Transaction> SampleDB::newTransaction() {
    auto txPtr = std::make_unique<tx::Transaction>(*_fm, *_lm, *_bm);
    return txPtr;
  }
}
