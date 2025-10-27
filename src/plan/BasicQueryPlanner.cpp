#include "BasicQueryPlanner.h"
#include <iostream>

namespace plan {
  BasicQueryPlanner::BasicQueryPlanner(meta::MetaDataMgr* metaDataMgr)
      : _metaDataMgr(metaDataMgr) {
  }

  std::shared_ptr<Plan> BasicQueryPlanner::createPlan(const parse::QueryData& data, tx::Transaction* tx) {
    _aggFns.clear();
    // Step 1: Create a plan for each mentioned table or view
    std::vector<std::shared_ptr<Plan>> plans;
    for (const auto& tableName : data.getTables()) {
      std::string viewDefinition = _metaDataMgr->getViewDef(tableName, *tx);
      record::Schema schema = _metaDataMgr->getLayout(tableName, *tx).schema();

      if (!viewDefinition.empty()) {
        parse::Parser parser(viewDefinition);
        parse::QueryData viewData = parser.query();
        plans.emplace_back(createPlan(viewData, tx));
      } else {
        auto tablePlan = std::make_shared<TablePlan>(tx, tableName, _metaDataMgr);
        plans.emplace_back(std::static_pointer_cast<Plan>(tablePlan));

        if (data.getGroupFields().size() == 0) continue;

        for (const auto& fieldName : schema.fieldNames()) {
          _aggFns.emplace_back(std::static_pointer_cast<materialize::AggregationFn>(
              std::make_shared<materialize::MaxFn>(fieldName)));
          _aggFns.emplace_back(std::static_pointer_cast<materialize::AggregationFn>(
              std::make_shared<materialize::MinFn>(fieldName)));

          if (schema.fieldType(fieldName) == record::Schema::INTEGER) {
            _aggFns.emplace_back(std::static_pointer_cast<materialize::AggregationFn>(
                std::make_shared<materialize::SumFn>(fieldName)));
          }
        }
      }
    }


    for (const auto& subdate : data.queryTables()) {
      auto subPlan = createPlan(subdate, tx);
      auto mp = std::make_shared<materialize::MaterializePlan>(tx, subPlan);
      plans.emplace_back(std::static_pointer_cast<Plan>(mp));
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
    if (data.getGroupFields().size() > 0) {
      _aggFns.emplace_back(std::static_pointer_cast<materialize::AggregationFn>(
          std::make_shared<materialize::CountFn>()));
      plan = std::static_pointer_cast<Plan>(
          std::make_shared<materialize::GroupByPlan>(tx, plan, data.getGroupFields(), _aggFns));
    }

    plan = std::static_pointer_cast<Plan>(
        std::make_shared<ProjectPlan>(plan, data.getFields()));

    if (data.getSortFields().size() > 0) {
      plan = std::static_pointer_cast<Plan>(
          std::make_shared<materialize::SortPlan>(plan, data.getSortFields(), tx));
    }

    return plan;
  }
}
