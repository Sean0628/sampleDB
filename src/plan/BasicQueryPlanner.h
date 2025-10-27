#pragma once

#include <string>
#include <vector>

#include "materialize/AggregationFn.h"
#include "materialize/CountFn.h"
#include "materialize/MaxFn.h"
#include "materialize/MinFn.h"
#include "materialize/SumFn.h"
#include "materialize/GroupByPlan.h"
#include "materialize/MaterializePlan.h"
#include "materialize/SortPlan.h"
#include "meta/MetaDataMgr.h"
#include "parse/Parser.h"
#include "parse/QueryData.h"
#include "SelectPlan.h"
#include "ProductPlan.h"
#include "ProjectPlan.h"
#include "TablePlan.h"
#include "QueryPlanner.h"
#include "tx/Transaction.h"

namespace plan {
  class BasicQueryPlanner : public QueryPlanner {
    public:
      BasicQueryPlanner(meta::MetaDataMgr* metaDataMgr);
      std::shared_ptr<Plan> createPlan(const parse::QueryData& data, tx::Transaction* tx) override;
    private:
      meta::MetaDataMgr* _metaDataMgr;
      std::vector<std::shared_ptr<materialize::AggregationFn>> _aggFns;
  };
}
