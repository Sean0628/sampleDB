#include "interface/ResultSet.h"

namespace interface {
  ResultSet::ResultSet(plan::Plan* plan, Connection* connection) : _scan(plan->open()), _schema(plan->schema()), _connection(connection) {}

  bool ResultSet::next() {
    try {
      return _scan->next();
    } catch (const std::exception& e) {
      _connection->rollback();
      std::runtime_error("Failed to move to next record: " + std::string(e.what()));
      return false;
    }
  }

  int ResultSet::getInt(std::string& fieldName) const {
    try {
      boost::algorithm::to_lower(fieldName);
      return _scan->getInt(fieldName);
    } catch (const std::exception& e) {
      _connection->rollback();
      std::runtime_error("Failed to get integer value for field '" + fieldName + "': " + std::string(e.what()));
      return -1;
    }
  }

  std::string ResultSet::getString(std::string& fieldName) const {
    try {
      boost::algorithm::to_lower(fieldName);
      return _scan->getString(fieldName);
    } catch (const std::exception& e) {
      _connection->rollback();
      std::runtime_error("Failed to get string value for field '" + fieldName + "': " + std::string(e.what()));
      return "";
    }
  }

  MetaData ResultSet::getMetaData() const {
    MetaData metaData(_schema);
    return metaData;
  }

  void ResultSet::close() {
    try {
      _scan->close();
      _connection->commit();
    } catch (const std::exception& e) {
      _scan->close();
      _connection->commit();
    }
  }
}
