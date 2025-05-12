#include "file/FileMgr.h"

namespace file {
  FileMgr::FileMgr(std::string dbDirectory, int blocksize) {
    _blocksize = blocksize;
    _dbDirectory = dbDirectory;
    std::filesystem::directory_entry dir(dbDirectory);

    _isNew = !dir.exists();

    // create the directory if the database is new
    if (_isNew) std::filesystem::create_directories(dbDirectory);

    // remove any leftover temporary tables
    for (auto& p : std::filesystem::directory_iterator(dbDirectory)) {
      if (p.path().filename().string().find("temp") != 0) continue;

      std::filesystem::remove(p.path());
    }
  }

  int FileMgr::blockSize() {
    return _blocksize;
  }

  void FileMgr::write(BlockId& blk, Page& p) {
    std::unique_lock<std::mutex> lock(_mutex);
    const std::string fileName = blk.fileName();
    std::shared_ptr<std::fstream> fileIO = getFile(fileName);

    int offset = blk.number() * _blocksize;
    fileIO->seekp(offset, std::ios::beg);
    fileIO->write(&((*p.contents())[0]), _blocksize);
    if (fileIO->bad()) exit(1);

    fileIO->flush();
  }

  BlockId FileMgr::append(const std::string& fileName) {
    std::unique_lock<std::mutex> lock(_mutex);
    std::shared_ptr<std::fstream> fileIO = getFile(fileName);

    int newBlkNum = length(fileName);
    BlockId blkId(fileName, newBlkNum);

    std::vector<char> byte_vector(_blocksize);
    fileIO->seekp(newBlkNum * _blocksize, std::ios::beg);
    fileIO->write(&byte_vector[0], _blocksize);
    if (fileIO->bad()) exit(1);

    fileIO->flush();

    return blkId;
  }

  void FileMgr::read(BlockId& blk, Page& p) {
    std::unique_lock<std::mutex> lock(_mutex);
    const std::string fileName = blk.fileName();
    std::shared_ptr<std::fstream> fileIO = getFile(fileName);

    int offset = blk.number() * _blocksize;
    fileIO->seekp(offset, std::ios::beg);
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

  std::shared_ptr<std::fstream> FileMgr::getFile(const std::string& fileName) {
    auto fileIO = std::make_shared<std::fstream>();
    std::filesystem::path path = _dbDirectory / fileName;

    if (_openFiles.find(path) != _openFiles.end()) {
      fileIO = _openFiles[path];
      if (fileIO->is_open()) return fileIO;
    }

    fileIO->open(path.string(), std::ios::binary | std::ios::in | std::ios::out);
    if (!fileIO->is_open()) {
      fileIO->clear();
      fileIO->open(path.string(), std::ios::binary | std::ios::trunc | std::ios::in | std::ios::out);
      fileIO->close();
      fileIO->open(path.string(), std::ios::binary | std::ios::in | std::ios::out);

      if (!fileIO->is_open()) exit(1);
    }

    _openFiles[path] = fileIO;
    return fileIO;
  }

  int FileMgr::length(const std::string& fileName) {
    std::filesystem::path path = _dbDirectory / fileName;
    int size;

    try {
      size = static_cast<int>(std::filesystem::file_size(path));
    } catch (std::exception &e) {
      size = 0;
    }

    return size / _blocksize;
  }
}
