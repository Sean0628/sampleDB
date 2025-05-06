#include "file/BufferMgr.h"

file::BufferMgr::BufferMgr(file::FileMgr& fileMgr, file::LogMgr& logMgr, int buffSize) {
    _numAvailable = buffSize;
    for (int i = 0; i < buffSize; i++) {
        _bufferpool.push_back(std::make_unique<file::Buffer>(fileMgr, logMgr));
    }
}

int file::BufferMgr::available() {
    std::unique_lock<std::mutex> lock(_mutex);
    return _numAvailable;
}

void file::BufferMgr::flushAll(int txnum) {
    std::unique_lock<std::mutex> lock(_mutex);
    for (auto& buffer : _bufferpool) {
        if (buffer->modifyingTx() == txnum) {
            buffer->flush();
        }
    }
}

void file::BufferMgr::unpin(Buffer& buff) {
    std::unique_lock<std::mutex> lock(_mutex);
    buff.unpin();
    if (!buff.isPinned()) {
        _numAvailable++;
        _cv.notify_all();
    }
}

file::Buffer* file::BufferMgr::pin(BlockId& blk) {
    std::unique_lock<std::mutex> lock(_mutex);
    auto start = std::chrono::steady_clock::now();

    Buffer* buffer = tryToPin(blk);
    while (!buffer && !waitingTooLong(start)) {
        _cv.wait_for(lock, std::chrono::milliseconds(MAX_TIME));
        buffer = tryToPin(blk);
    }

    if (!buffer) {
        throw std::runtime_error("BufferMgr: No available buffer");
    }

    return buffer;
}

bool file::BufferMgr::waitingTooLong(std::chrono::steady_clock::time_point start) {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    return duration.count() > MAX_TIME;
}

file::Buffer* file::BufferMgr::tryToPin(BlockId& blk) {
  Buffer* buffer = findExistingBuffer(blk);
  if (!buffer) {
    buffer = chooseUnpinnedBuffer();
    if (!buffer) return nullptr;

    buffer->assignToBlock(blk);
  }

  if (!buffer->isPinned()) {
    _numAvailable--;
  }

  buffer->pin();
  return buffer;
}

file::Buffer* file::BufferMgr::findExistingBuffer(BlockId& blk) {
    for (auto& buffer : _bufferpool) {
        if (buffer->block().equals(blk)) {
            return buffer.get();
        }
    }
    return nullptr;
}

file::Buffer* file::BufferMgr::chooseUnpinnedBuffer() {
    for (auto& buffer : _bufferpool) {
        if (!buffer->isPinned()) {
            return buffer.get();
        }
    }
    return nullptr;
}
