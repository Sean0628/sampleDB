#include "interface/ResultSet.h"

namespace interface {
  ResultSet::ResultSet(plan::Plan* plan, Connection* connection) : _connection(connection) {
    if (!plan) throw std::runtime_error("ResultSet: null Plan*");

    // Force a schema() call first — if any pure-virtual/signature issue exists,
    // it will blow here with a clear stack.
    _schema = plan->schema();
    // Then open; if open() returns nullptr or throws, you'll see it clearly.
    _scan = plan->open();
    if (!_scan) throw std::runtime_error("ResultSet: Plan::open() returned null Scan");
  }

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

  bool ResultSet::getBool(std::string& fieldName) const {
    try {
      boost::algorithm::to_lower(fieldName);
      return _scan->getBool(fieldName);
    } catch (const std::exception& e) {
      _connection->rollback();
      std::runtime_error("Failed to get bool value for field '" + fieldName + "': " + std::string(e.what()));
      return false;
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
