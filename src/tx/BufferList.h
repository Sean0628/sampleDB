#pragma once

#include <string>
#include <vector>
#include <map>
#include "file/BlockId.h"
#include "buffer/Buffer.h"
#include "buffer/BufferMgr.h"

namespace tx {
  class BufferList {
    public:
      BufferList(buffer::BufferMgr& bm) : _bm(bm) {}
      buffer::Buffer& getBuffer(const file::BlockId& blkId);
      void pin(const file::BlockId& blkId);
      void unpin(const file::BlockId& blkId);
      void unpinAll();
    private:
      buffer::BufferMgr& _bm;
      std::vector<file::BlockId> _pinned;
      std::map<file::BlockId, buffer::Buffer*> _buffers;
  };
}
