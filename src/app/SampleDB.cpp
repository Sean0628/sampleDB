#include "app/SampleDB.h"

using namespace app;
using namespace std;

const string SampleDB::LOGFILE = "sampledb.log";

SampleDB::SampleDB(string dirname, int blocksize, int buffsize) {
  auto path = filesystem::current_path() / dirname;
  _fm = make_unique<file::FileMgr>(path, blocksize);
  _lm = make_unique<file::LogMgr>(*_fm, LOGFILE);

  bool isNew = _fm->isNew();
  if (isNew)
    cout << "creating new database" << endl;
  else {
    cout << "recovering existing database" << endl;
  }
}

file::FileMgr& SampleDB::fileMgr() {
  return *_fm;
}

file::LogMgr& SampleDB::logMgr() {
  return *_lm;
}
