#include "InsertData.h"

namespace parse {
  InsertData::InsertData(const std::string& tableName, const std::vector<std::string>& fieldNames, const std::vector<scan::Constant>& values)
      : _tableName(tableName), _fieldNames(fieldNames), _values(values) {
  }

  std::string InsertData::tableName() const {
    return _tableName;
  }

  std::vector<std::string> InsertData::fieldNames() const {
    return _fieldNames;
  }

  std::vector<scan::Constant> InsertData::values() const {
    return _values;
  }

  int InsertData::getObjectId() {
    return _objectId;
  }
}
