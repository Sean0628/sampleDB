#include "file/FileMgr.h"

using namespace file;
using namespace std;

FileMgr::FileMgr(string dbDirectory, int blocksize) {
  _blocksize = blocksize;
  _dbDirectory = dbDirectory;
  filesystem::directory_entry dir(dbDirectory);

  _isNew = !dir.exists();

  // create the directory if the database is new
  if (_isNew) filesystem::create_directories(dbDirectory);

  // remove any leftover temporary tables
  for (auto& p : filesystem::directory_iterator(dbDirectory)) {
    if (p.path().filename().string().find("temp") != 0) continue;

    filesystem::remove(p.path());
  }
}

int FileMgr::blockSize() {
  return _blocksize;
}

void FileMgr::write(BlockId& blk, Page& p) {
  unique_lock<mutex> lock(_mutex);
  const string fileName = blk.fileName();
  shared_ptr<fstream> fileIO = getFile(fileName);

  int offset = blk.number() * _blocksize;
  fileIO->seekp(offset, ios::beg);
  fileIO->write(&((*p.contents())[0]), _blocksize);
  if (fileIO->bad()) exit(1);

  fileIO->flush();
}

BlockId FileMgr::append(const string& fileName) {
  unique_lock<mutex> lock(_mutex);
  shared_ptr<fstream> fileIO = getFile(fileName);

  int newBlkNum = length(fileName);
  BlockId blkId(fileName, newBlkNum);

  vector<char> byte_vector(_blocksize);
  fileIO->seekp(newBlkNum * _blocksize, ios::beg);
  fileIO->write(&byte_vector[0], _blocksize);
  if (fileIO->bad()) exit(1);

  fileIO->flush();

  return blkId;
}

void FileMgr::read(BlockId& blk, Page& p) {
  unique_lock<mutex> lock(_mutex);
  const string fileName = blk.fileName();
  shared_ptr<fstream> fileIO = getFile(fileName);

  int offset = blk.number() * _blocksize;
  fileIO->seekp(offset, ios::beg);
  fileIO->read(&((*p.contents())[0]), _blocksize);
  if (fileIO->bad()) exit(1);

  int readCnt = fileIO->gcount();
  if (readCnt < _blocksize) {
    fileIO->clear();
    memset(&((*p.contents())[0]), 0, _blocksize - readCnt);
  }
}

bool FileMgr::isNew() {
  return _isNew;
}

shared_ptr<fstream> FileMgr::getFile(const string& fileName) {
  auto fileIO = make_shared<fstream>();
  filesystem::path path = _dbDirectory / fileName;

  if (_openFiles.find(path) != _openFiles.end()) {
    fileIO = _openFiles[path];
    if (fileIO->is_open()) return fileIO;
  }

  fileIO->open(path.string(), ios::binary | ios::in | ios::out);
  if (!fileIO->is_open()) {
    fileIO->clear();
    fileIO->open(path.string(), ios::binary | ios::trunc | ios::in | ios::out);
    fileIO->close();
    fileIO->open(path.string(), ios::binary | ios::in | ios::out);

    if (!fileIO->is_open()) exit(1);
  }

  _openFiles[path] = fileIO;
  return fileIO;
}

int FileMgr::length(const string& fileName) {
  filesystem::path path = _dbDirectory / fileName;
  int size;

  try {
    size = static_cast<int>(filesystem::file_size(path));
  } catch (exception &e) {
    size = 0;
  }

  return size / _blocksize;
}
