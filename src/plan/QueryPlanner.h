#pragma once

#include <memory>

#include "parse/QueryData.h"
#include "tx/Transaction.h"
#include "Plan.h"

namespace plan {
  class QueryPlanner {
    public:
      ~QueryPlanner() = default;
      virtual std::shared_ptr<Plan> createPlan(const parse::QueryData& data, tx::Transaction* tx) = 0;
  };
}
