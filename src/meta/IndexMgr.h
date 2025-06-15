#pragma once

#include <string>
#include <map>

#include "TableMgr.h"
#include "StatMgr.h"
#include "IndexInfo.h"
#include "tx/Transaction.h"
#include "record/Layout.h"

namespace meta {
  class IndexMgr {
    public:
      IndexMgr(bool isNew, TableMgr& tableMgr, StatMgr& statMgr, tx::Transaction& tx);
      void createIndex(const std::string& indexName, const std::string& tableName, const std::string& fieldName, tx::Transaction& tx);
      std::map<std::string, IndexInfo> getIndexInfo(const std::string& tableName, tx::Transaction& tx);
    private:
      record::Layout _layout;
      TableMgr& _tableMgr;
      StatMgr& _statMgr;
  };
}
