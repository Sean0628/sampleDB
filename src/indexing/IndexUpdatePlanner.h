#pragma once

#include <string>
#include <memory>
#include <vector>

#include "meta/MetaDataMgr.h"
#include "plan/Plan.h"
#include "plan/UpdatePlanner.h"
#include "plan/TablePlan.h"
#include "plan/SelectPlan.h"
#include "parse/CreateIndexData.h"
#include "parse/CreateTableData.h"
#include "parse/CreateViewData.h"
#include "parse/DeleteData.h"
#include "parse/InsertData.h"
#include "parse/ModifyData.h"
#include "scan/UpdateScan.h"

namespace indexing {
  class IndexUpdatePlanner : public plan::UpdatePlanner {
    public:
      IndexUpdatePlanner(meta::MetaDataMgr* metaDataMgr);
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
