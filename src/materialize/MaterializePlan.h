#pragma once

#include <cmath>
#include <memory>
#include <string>

#include "TempTable.h"
#include "plan/Plan.h"
#include "record/Layout.h"
#include "record/Schema.h"
#include "tx/Transaction.h"

namespace materialize {
  class MaterializePlan : public plan::Plan {
    public:
      MaterializePlan(tx::Transaction* tx, const std::shared_ptr<plan::Plan>& plan);
      std::shared_ptr<scan::Scan> open() override;
      int blocksAccessed() const override;
      int recordsOutput() const override;
      int distinctValues(const std::string& fieldName) const override;
      record::Schema schema() const override;
    private:
      tx::Transaction* _tx;
      std::shared_ptr<plan::Plan> _plan;
  };
}
