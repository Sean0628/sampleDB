#include "Statement.h"

namespace interface {
  Statement::Statement(Connection* connection, plan::Planner& planner) : _connection(connection), _planner(planner) {}

  ResultSet Statement::executeQuery(const std::string& query) {
    try {
      tx::Transaction& tx = _connection->getTransaction();
      auto plan = _planner.createQueryPlan(query, &tx);
      ResultSet rs(plan.get(), _connection);
      return rs;
    } catch (const std::exception& e) {
      _connection->rollback();
      throw std::runtime_error("Failed to execute query: " + std::string(e.what()));
    }
  }

  int Statement::executeUpdate(const std::string& cmd) {
    try {
      tx::Transaction& tx = _connection->getTransaction();
      int result = _planner.executeUpdate(cmd, &tx);
      _connection->commit();
      return result;
    } catch (const std::exception& e) {
      _connection->rollback();
      std::runtime_error("Failed to execute update: " + std::string(e.what()));
      return 0;
    }
  }

  void Statement::close() {
    return;
  }
}
