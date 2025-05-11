#pragma once

#include <string>
#include <vector>
#include <map>
#include "file/BlockId.h"
#include "file/Buffer.h"
#include "file/BufferMgr.h"

namespace tx {
  class BufferList {
    public:
      BufferList(file::BufferMgr& bm) : _bm(bm) {}
      file::Buffer& getBuffer(const file::BlockId& blkId);
      void pin(const file::BlockId& blkId);
      void unpin(const file::BlockId& blkId);
      void unpinAll();
    private:
      file::BufferMgr& _bm;
      std::vector<file::BlockId> _pinned;
      std::map<file::BlockId, file::Buffer*> _buffers;
  };
}
