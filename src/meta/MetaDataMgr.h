#pragma once

#include <string>
#include <map>
#include <memory>

#include "tx/Transaction.h"
#include "record/Schema.h"
#include "record/Layout.h"
#include "meta/TableMgr.h"
#include "meta/ViewMgr.h"
#include "meta/StatMgr.h"
#include "meta/IndexMgr.h"
#include "meta/IndexInfo.h"
#include "meta/StatInfo.h"

namespace meta {
  class MetaDataMgr {
    public:
      MetaDataMgr(bool isNew, tx::Transaction& tx);
      void createTable(const std::string& tableName, const record::Schema& schema, tx::Transaction& tx);
      record::Layout getLayout(const std::string& tableName, tx::Transaction& tx);
      void createView(const std::string& viewName, const std::string& viewDef, tx::Transaction& tx);
      std::string getViewDef(const std::string& viewName, tx::Transaction& tx);
      void createIndex(const std::string& indexName, const std::string& tableName, const std::string& fieldName, tx::Transaction& tx);
      std::map<std::string, IndexInfo> getIndexInfo(const std::string& tableName, tx::Transaction& tx);
      StatInfo getStatInfo(const std::string& tableName, const record::Layout& layout, tx::Transaction& tx);
    private:
      static std::unique_ptr<TableMgr> _tableMgr;
      static std::unique_ptr<ViewMgr> _viewMgr;
      static std::unique_ptr<StatMgr> _statMgr;
      static std::unique_ptr<IndexMgr> _indexMgr;
  };
}
