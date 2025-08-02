#pragma once

#include <string>
#include <vector>

#include "meta/MetaDataMgr.h"
#include "Plan.h"
#include "SelectPlan.h"
#include "UpdatePlanner.h"
#include "TablePlan.h"
#include "tx/Transaction.h"
#include "scan/Constant.h"
#include "scan/UpdateScan.h"

namespace plan {
  class BasicUpdatePlanner : public UpdatePlanner {
    public:
      BasicUpdatePlanner(meta::MetaDataMgr* metaDataMgr);
      int executeDelete(const parse::DeleteData* data, tx::Transaction* tx) override;
      int executeModify(const parse::ModifyData* data, tx::Transaction* tx) override;
      int executeInsert(const parse::InsertData* data, tx::Transaction* tx) override;
      int executeCreateTable(const parse::CreateTableData* data, tx::Transaction* tx) override;
      int executeCreateView(const parse::CreateViewData* data, tx::Transaction* tx) override;
      int executeCreateIndex(const parse::CreateIndexData* data, tx::Transaction* tx) override;
    private:
      meta::MetaDataMgr* _metaDataMgr;
  };
}
