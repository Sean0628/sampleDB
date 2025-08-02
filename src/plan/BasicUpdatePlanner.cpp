#include <iostream>
#include "plan/BasicUpdatePlanner.h"

namespace plan {
  BasicUpdatePlanner::BasicUpdatePlanner(meta::MetaDataMgr* metaDataMgr)
      : _metaDataMgr(metaDataMgr) {
  }

  int BasicUpdatePlanner::executeDelete(const parse::DeleteData* data, tx::Transaction* tx) {
      auto plan = std::static_pointer_cast<Plan>(
          std::make_shared<TablePlan>(tx, data->tableName(), _metaDataMgr)
          );

      plan = std::static_pointer_cast<Plan>(
          std::make_shared<SelectPlan>(plan, data->predicate()));
      auto updateScan = std::static_pointer_cast<scan::UpdateScan>(plan->open());
      int count = 0;
      while (updateScan->next()) {
          updateScan->remove();
          count++;
      }
      updateScan->close();

      return count;
  }

  int BasicUpdatePlanner::executeModify(const parse::ModifyData* data, tx::Transaction* tx) {
      auto plan = std::static_pointer_cast<Plan>(
          std::make_shared<TablePlan>(tx, data->tableName(), _metaDataMgr));

      plan = std::static_pointer_cast<Plan>(
          std::make_shared<SelectPlan>(plan, data->predicate()));
      auto updateScan = std::static_pointer_cast<scan::UpdateScan>(plan->open());
      int count = 0;
      while (updateScan->next()) {
        scan::Constant value = data->newValue().evaluate(updateScan.get());
        updateScan->setValue(data->targetField(), value);
        count++;
      }
      updateScan->close();

      return count;
  }

  int BasicUpdatePlanner::executeInsert(const parse::InsertData* data, tx::Transaction* tx) {
      auto plan = std::static_pointer_cast<Plan>(
          std::make_shared<TablePlan>(tx, data->tableName(), _metaDataMgr));

      auto updateScan = std::static_pointer_cast<scan::UpdateScan>(plan->open());
      updateScan->insert();
      auto values = data->values();
      int i = 0;
      for (const auto& field : data->fieldNames()) {
        scan::Constant value = values[i];
        updateScan->setValue(field, value);
        i++;
      }
      updateScan->close();

      return 1; // Insert returns 1 for successful insertion
  }

  int BasicUpdatePlanner::executeCreateTable(const parse::CreateTableData* data, tx::Transaction* tx) {
      _metaDataMgr->createTable(data->tableName(), data->newSchema(), *tx);
      return 0;
  }

  int BasicUpdatePlanner::executeCreateView(const parse::CreateViewData* data, tx::Transaction* tx) {
      _metaDataMgr->createView(data->viewName(), data->viewDefinition(), *tx);
      return 0;
  }

  int BasicUpdatePlanner::executeCreateIndex(const parse::CreateIndexData* data, tx::Transaction* tx) {
      _metaDataMgr->createIndex(data->indexName(), data->tableName(), data->fieldName(), *tx);
      return 0;
  }
}
