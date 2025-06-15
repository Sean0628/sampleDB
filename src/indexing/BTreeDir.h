#pragma once

#include <string>

#include "file/BlockId.h"
#include "record/Layout.h"
#include "tx/Transaction.h"
#include "indexing/BTreePage.h"
#include "indexing/DirEntry.h"

namespace indexing {
  class BTreeDir {
    public:
      BTreeDir(tx::Transaction& tx, file::BlockId& blk, const record::Layout& layout);
      void close();
      int search(const scan::Constant& key);
      void makeNewRoot(const DirEntry& dirEntry);
      DirEntry insert(const DirEntry& dirEntry);
    private:
      tx::Transaction& _tx;
      record::Layout _layout;
      BTreePage _contents;
      std::string _fileName;

      DirEntry insertEntry(const DirEntry& dirEntry);
      file::BlockId findChildBlock(const scan::Constant& key);
  };
}
