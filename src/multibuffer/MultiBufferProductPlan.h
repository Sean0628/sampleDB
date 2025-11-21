#pragma once

#include <memory>
#include <string>
#include <vector>

#include "materialize/MaterializePlan.h"
#include "materialize/TempTable.h"
#include "BufferNeeds.h"
#include "MultiBufferProductScan.h"
#include "plan/Plan.h"
#include "record/Schema.h"
#include "record/TableScan.h"
#include "scan/Scan.h"
#include "scan/UpdateScan.h"
#include "tx/Transaction.h"

namespace multibuffer {
  class MultiBufferProductPlan : public plan::Plan {
    public:
      MultiBufferProductScan(const std::shared_ptr<plan::Plan>& p1,
                             const std::shared_ptr<plan::Plan>& p2,
                             tx::Transaction* tx);
      std::shared_ptr<scan::Scan> open() override;
      int blocksAccessed() override;
      int recordsOutput() override;
      int distinctValues(const std::string& fldname) override;
      record::Schema schema() override;
    private:
      tx::Transaction* _tx;
      std::shared_ptr<plan::Plan> _p1, _p2;
      record::Schema _schema;

      std::shared_ptr<materialize::TempTable> copyRecordFrom(const std::shared_ptr<plan::Plan>& p);

  };
}
