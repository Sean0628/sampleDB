# pragma once

#include <string>
#include "tx/Transaction.h"
#include "file/BlockId.h"
#include "record/Layout.h"

namespace record {
  class RecordPage {
    public:
      RecordPage(tx::Transaction& tx, const file::BlockId& blk, const Layout& layout);

      int getInt(int slot, const std::string& fieldName) const;
      std::string getString(int slot, const std::string& fieldName) const;
      void setInt(int slot, const std::string& fieldName, int val);
      void setString(int slot, const std::string& fieldName, const std::string& val);
      void remove(int slot);
      void format();
      int nextAfter(int slot) const;
      int insertAfter(int slot);
      file::BlockId block() const;

      static const int EMPTY = 0, USED = 1;
    private:
      tx::Transaction& _tx;
      file::BlockId _blk;
      Layout _layout;

      void setFlag(int slot, int flag);
      int searchAfter(int slot, int flag) const;
      bool isValidSlot(int slot) const;
      int offset(int slot) const;
  };
}
