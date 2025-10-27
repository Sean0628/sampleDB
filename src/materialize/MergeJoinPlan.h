#pragma once

#include <cmath>
#include <string>
#include <memory>
#include <vector>

#include "MergeJoinScan.h"
#include "SortPlan.h"
#include "plan/Plan.h"
#include "record/Schema.h"
#include "tx/Transaction.h"

namespace materialize {
  class MergeJoinPlan : public plan::Plan {
    public:
      MergeJoinPlan(tx::Transaction* tx,
          const std::shared_ptr<plan::Plan>& p1,
          const std::shared_ptr<plan::Plan>& p2,
          const std::string& fld1,
          const std::string& fld2);
      std::shared_ptr<scan::Scan> open() override;
      int blocksAccessed() const override;
      int recordsOutput() const override;
      int distinctValues(const std::string& fieldName) const override;
      record::Schema schema() const override;
    private:
      std::shared_ptr<plan::Plan> _p1, _p2;
      std::string _fld1, _fld2;
      record::Schema _schema;
  };
}
