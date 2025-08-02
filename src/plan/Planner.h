#pragma once

#include <string>
#include <memory>

#include "parse/ObjectID.h"
#include "parse/Parser.h"
#include "plan/QueryPlanner.h"
#include "plan/UpdatePlanner.h"
#include "tx/Transaction.h"

namespace plan {
  class Planner {
    public:
      Planner(std::unique_ptr<QueryPlanner> queryPlanner, std::unique_ptr<UpdatePlanner> updatePlanner);
      std::shared_ptr<Plan> createQueryPlan(const std::string& query, tx::Transaction* tx);
      int executeUpdate(const std::string& update, tx::Transaction* tx);
    private:
      std::unique_ptr<QueryPlanner> _queryPlanner;
      std::unique_ptr<UpdatePlanner> _updatePlanner;
  };
}
