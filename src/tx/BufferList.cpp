#include <iostream>
#include <algorithm>
#include "BufferList.h"

namespace tx {
  buffer::Buffer& BufferList::getBuffer(const file::BlockId& blkId) {
    return *(_buffers[blkId]);
  }

  void BufferList::pin(const file::BlockId& blkId) {
    buffer::Buffer* buf = _bm.pin(blkId);
    _buffers[blkId] = buf;
    _pinned.push_back(blkId);
  }

  void BufferList::unpin(const file::BlockId& blkId) {
    auto it = _buffers.find(blkId);
    if (it == _buffers.end()) return;  // block was already unpinned

    _bm.unpin(*(it->second));

    auto pinIt = std::find(_pinned.begin(), _pinned.end(), blkId);
    if (pinIt != _pinned.end()) {
      _pinned.erase(pinIt);
    }

    // Only erase if no more pins for this block
    if (std::find(_pinned.begin(), _pinned.end(), blkId) == _pinned.end()) {
      _buffers.erase(it);
    }
  }

  void BufferList::unpinAll() {
    for (const auto& blkId : _pinned) {
      buffer::Buffer* buf = _buffers[blkId];
      _bm.unpin(*buf);
    }
    _buffers.clear();
    _pinned.clear();
  }
}
