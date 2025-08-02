#pragma once

#include <string>
#include <vector>

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
  };
}

