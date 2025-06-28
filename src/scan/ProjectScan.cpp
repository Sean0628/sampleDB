#include <algorithm>
#include "ProjectScan.h"

namespace scan {
  ProjectScan::ProjectScan(std::shared_ptr<Scan>& scan, const std::vector<std::string>& fields)
      : _scan(scan), _fields(fields) {
  }

  void ProjectScan::beforeFirst() {
    _scan->beforeFirst();
  }

  bool ProjectScan::next() {
    return _scan->next();
  }

  int ProjectScan::getInt(const std::string& fieldName) {
    if (hasField(fieldName)) return _scan->getInt(fieldName);

    throw std::runtime_error("Field not found: " + fieldName);
  }

  std::string ProjectScan::getString(const std::string& fieldName) {
    if (hasField(fieldName)) return _scan->getString(fieldName);

    throw std::runtime_error("Field not found: " + fieldName);
  }

  Constant ProjectScan::getValue(const std::string& fieldName) {
    if (hasField(fieldName)) return _scan->getValue(fieldName);

    throw std::runtime_error("Field not found: " + fieldName);
  }

  bool ProjectScan::hasField(const std::string& fieldName) {
    auto it = std::find(_fields.begin(), _fields.end(), fieldName);
    return it != _fields.end();
  }

  void ProjectScan::close() {
    _scan->close();
  }
}
