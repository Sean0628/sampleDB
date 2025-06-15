# pragma once

#include <string>
#include <memory>
#include "record/Schema.h"
#include "record/Layout.h"
#include "tx/Transaction.h"
#include "meta/StatInfo.h"
#include "indexing/Index.h"

namespace meta {
  class IndexInfo {
    public:
      IndexInfo() = default;
      IndexInfo(const std::string& indexName, const std::string& fieldName, const record::Schema& schema, tx::Transaction* tx, const StatInfo& statInfo);
      std::shared_ptr<indexing::Index> open();
      int blocksAccessed() const;
      int recordOutput() const;
      int distinctValues(const std::string& fieldName) const;
      bool isNull() const;
    private:
      std::string _indexName;
      std::string _fieldName;
      tx::Transaction* _tx;
      record::Schema _schema;
      record::Layout _layout;
      StatInfo _statInfo;
      record::Layout createIndexLayout();
  };
}
