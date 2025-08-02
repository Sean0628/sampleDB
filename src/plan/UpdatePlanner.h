#pragma once

#include "parse/CreateTableData.h"
#include "parse/CreateViewData.h"
#include "parse/CreateIndexData.h"
#include "parse/DeleteData.h"
#include "parse/InsertData.h"
#include "parse/ModifyData.h"
#include "Plan.h"
#include "tx/Transaction.h"

namespace plan {
  class UpdatePlanner {
    public:
      ~UpdatePlanner() = default;
      virtual int executeDelete(const parse::DeleteData* data, tx::Transaction* tx) = 0;
      virtual int executeModify(const parse::ModifyData* data, tx::Transaction* tx) = 0;
      virtual int executeInsert(const parse::InsertData* data, tx::Transaction* tx) = 0;
      virtual int executeCreateTable(const parse::CreateTableData* data, tx::Transaction* tx) = 0;
      virtual int executeCreateView(const parse::CreateViewData* data, tx::Transaction* tx) = 0;
      virtual int executeCreateIndex(const parse::CreateIndexData* data, tx::Transaction* tx) = 0;
  };
}
