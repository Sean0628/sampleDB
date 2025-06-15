#pragma once

#include <string>

#include "record/Layout.h"
#include "record/RID.h"
#include "file/BlockId.h"
#include "tx/Transaction.h"
#include "scan/Constant.h"

namespace indexing {
  class BTreePage {
    public:
      BTreePage() = default;
      BTreePage(tx::Transaction* tx, const file::BlockId& blk, const record::Layout& layout);
      int findSlotBefore(const scan::Constant& key);
      void close();
      bool isFull();
      file::BlockId split(int splitPos, int flag);
      scan::Constant getDataVal(int slot) const;
      int getFlag();
      void setFlag(int val);
      file::BlockId appendNew(int flag);
      void format(const file::BlockId& blk, int flag);
      void makeDefaultRecord(const file::BlockId& blk, int pos);
      int getChildNum(int slot);
      void insertDir(int slot, const scan::Constant& dataval, int blknum);
      record::RID getDataRid(int slot) const;
      void insertLeaf(int slot, const scan::Constant& dataval, const record::RID& datarid);
      void remove(int slot);
      int getNumRecs() const;
      bool isNull() const;
    private:
      tx::Transaction* _tx;
      file::BlockId _blk;
      record::Layout _layout;

      int getInt(int slot, const std::string& fieldName) const;
      std::string getString(int slot, const std::string& fieldName) const;
      scan::Constant getValue(int slot, const std::string& fieldName) const;
      void setInt(int slot, const std::string& fieldName, int value);
      void setString(int slot, const std::string& fieldName, const std::string& value);
      void setValue(int slot, const std::string& fieldName, const scan::Constant& value);
      void setNumRecs(int numRecs);
      void insert(int slot);
      void copyRecord(int from, int to);
      void transferRecs(int slot, BTreePage& dest);
      int fldPos(int slot, const std::string& fieldName) const;
      int slotPos(int slot) const;
  };
}
