#pragma once

#include <memory>
#include <string>
#include <vector>

#include "AggregationFn.h"
#include "GroupByScan.h"
#include "SortPlan.h"
#include "plan/Plan.h"
#include "record/Schema.h"
#include "scan/Scan.h"
#include "tx/Transaction.h"

namespace materialize {
  class GroupByPlan : public plan::Plan {
    public:
      GroupByPlan(tx::Transaction* tx,
          const std::shared_ptr<plan::Plan>& plan,
          const std::vector<std::string>& groupFields,
          const std::vector<std::shared_ptr<AggregationFn>>& aggFns);
      std::shared_ptr<scan::Scan> open() override;
      int blocksAccessed() const override;
      int recordsOutput() const override;
      int distinctValues(const std::string& fieldName) const override;
      record::Schema schema() const override;
    private:
      std::shared_ptr<plan::Plan> _plan;
      std::vector<std::string> _groupFields;
      std::vector<std::shared_ptr<AggregationFn>> _aggFns;
      record::Schema _schema;
  };
}
