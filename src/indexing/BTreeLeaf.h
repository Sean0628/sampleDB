#pragma once

#include <string>

#include "indexing/BTreePage.h"
#include "indexing/DirEntry.h"
#include "tx/Transaction.h"
#include "record/Layout.h"
#include "scan/Constant.h"
#include "file/BlockId.h"
#include "record/RID.h"

namespace indexing {
  class BTreeLeaf {
    public:
      BTreeLeaf() = default;
      BTreeLeaf(tx::Transaction* tx, const file::BlockId& blk, const record::Layout& layout, const scan::Constant& key);
      void close();
      bool next();
      record::RID getDataRid() const;
      void remove(const record::RID& datarid);
      DirEntry insert(const record::RID& datarid);
      bool isNull() const;
    private:
      tx::Transaction* _tx;
      record::Layout _layout;
      scan::Constant _key;
      BTreePage _contents;
      int _currentSlot;
      std::string _fileName;
      bool tryOverflow();
  };
}
