#include "BasicQueryPlanner.h"

namespace plan {
  BasicQueryPlanner::BasicQueryPlanner(meta::MetaDataMgr* metaDataMgr)
      : _metaDataMgr(metaDataMgr) {
  }

  std::shared_ptr<Plan> BasicQueryPlanner::createPlan(const parse::QueryData& data, tx::Transaction* tx) {
    // Step 1: Create a plan for each mentioned table or view
    std::vector<std::shared_ptr<Plan>> plans;
    for (const auto& tableName : data.getTables()) {
      std::string viewDefinition = _metaDataMgr->getViewDef(tableName, *tx);

      if (!viewDefinition.empty()) {
        parse::Parser parser(viewDefinition);
        parse::QueryData viewData = parser.query();
        plans.push_back(createPlan(viewData, tx));
      } else {
        auto tablePlan = std::make_shared<TablePlan>(tx, tableName, _metaDataMgr);
        plans.push_back(tablePlan);
      }
    }

    // Step 2: Create the product of all table plans
    auto plan = plans[0];
    plans.erase(plans.begin());
    for (const auto& nextPlan : plans) {
      plan = std::static_pointer_cast<Plan>(
          std::make_shared<ProductPlan>(plan, nextPlan));
    }

    // Step 3: Add a selection plan for the predicate
    plan = std::static_pointer_cast<Plan>(
        std::make_shared<SelectPlan>(plan, data.getPredicate()));

    // Step 4: Project on the field names
    return std::static_pointer_cast<Plan>(
        std::make_shared<ProjectPlan>(plan, data.getFields()));
  }
}
