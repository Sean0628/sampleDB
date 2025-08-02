#pragma once

#include <string>
#include <memory>

#include "meta/StatMgr.h"
#include "meta/MetaDataMgr.h"
#include "record/Layout.h"
#include "record/TableScan.h"
#include "tx/Transaction.h"
#include "Plan.h"

namespace plan {
  class TablePlan : public Plan {
    public:
      TablePlan(tx::Transaction* tx, const std::string& tableName, meta::MetaDataMgr* metaDataMgr);
      std::shared_ptr<scan::Scan> open() override;
      int blocksAccessed() const override;
      int recordsOutput() const override;
      int distinctValues(const std::string& fieldName) const override;
      record::Schema schema() const override;
    private:
      tx::Transaction* _tx;
      std::string _tableName;
      record::Layout _layout;
      meta::StatInfo _statInfo;
  };
}
