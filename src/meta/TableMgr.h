# pragma once

#include <string>

#include "record/Schema.h"
#include "record/Layout.h"
#include "tx/Transaction.h"

namespace meta {
  class TableMgr {
    public:
      TableMgr(bool isNew, tx::Transaction& tx);
      void createTable(const std::string& tableName, const record::Schema& schema, tx::Transaction& tx);
      record::Layout getLayout(const std::string& tableName, tx::Transaction& tx);
      const static int MAX_NAME = 16; // table or field name length
    private:
      record::Layout _tcatLayout;
      record::Layout _fcatLayout;
  };
}
