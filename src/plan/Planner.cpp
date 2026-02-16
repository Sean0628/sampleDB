#include <iostream>

#include "Planner.h"

namespace plan {
  Planner::Planner(std::unique_ptr<QueryPlanner> queryPlanner, std::unique_ptr<UpdatePlanner> updatePlanner)
      : _queryPlanner(std::move(queryPlanner)), _updatePlanner(std::move(updatePlanner)) {
  }

  std::shared_ptr<Plan> Planner::createQueryPlan(const std::string& query, tx::Transaction* tx) {
    parse::Parser parser(query);
    parse::QueryData data = parser.query();
    std::cout << data.toString() << std::endl;
    return std::static_pointer_cast<Plan>(
        _queryPlanner->createPlan(data, tx));
  }

  int Planner::executeUpdate(const std::string& update, tx::Transaction* tx) {
    parse::Parser parser(update);
    auto objectPtr = parser.updateCmd();
    int id = objectPtr->getObjectId();

    if (id == parse::ObjectID::INSERT) {
      return _updatePlanner->executeInsert(static_cast<parse::InsertData*>(objectPtr.get()), tx);
    } else if (id == parse::ObjectID::DELETE) {
      return _updatePlanner->executeDelete(static_cast<parse::DeleteData*>(objectPtr.get()), tx);
    } else if (id == parse::ObjectID::MODIFY) {
      return _updatePlanner->executeModify(static_cast<parse::ModifyData*>(objectPtr.get()), tx);
    } else if (id == parse::ObjectID::CREATE_TABLE) {
      return _updatePlanner->executeCreateTable(static_cast<parse::CreateTableData*>(objectPtr.get()), tx);
    } else if (id == parse::ObjectID::CREATE_VIEW) {
      return _updatePlanner->executeCreateView(static_cast<parse::CreateViewData*>(objectPtr.get()), tx);
    } else if (id == parse::ObjectID::CREATE_INDEX) {
      return _updatePlanner->executeCreateIndex(static_cast<parse::CreateIndexData*>(objectPtr.get()), tx);
    } else {
      throw std::runtime_error("Unknown update command");
    }
  }
}
