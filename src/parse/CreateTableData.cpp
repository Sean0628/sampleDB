#include "CreateTableData.h"

namespace parse {
  CreateTableData::CreateTableData(const std::string& tableName, const record::Schema& schema)
      : _tableName(tableName), _schema(schema) {
  }

  std::string CreateTableData::tableName() const {
    return _tableName;
  }

  record::Schema CreateTableData::newSchema() const {
    return _schema;
  }

  int CreateTableData::getObjectId() {
    return _objectId;
  }
}
