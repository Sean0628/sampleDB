#pragma once

#include <memory>
#include <mutex>
#include <string>

#include "record/Layout.h"
#include "record/Schema.h"
#include "record/TableScan.h"
#include "tx/Transaction.h"
#include "scan/UpdateScan.h"

namespace materialize {
  class TempTable {
    public:
      TempTable(tx::Transaction* tx, const record::Schema& schema);
      std::shared_ptr<scan::UpdateScan> open();
      std::string tableName() const;
      record::Layout getLayout() const;
      static std::string nextTableName();
    private:
      static std::mutex _mtx;
      static int _nextTableNum;
      tx::Transaction* _tx;
      std::string _tableName;
      record::Layout _layout;
  };
}
