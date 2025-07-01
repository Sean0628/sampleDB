#include "CreateIndexData.h"

namespace parse {
  CreateIndexData::CreateIndexData(const std::string& indexName, const std::string& tableName, const std::string& fieldName)
      : _indexName(indexName), _tableName(tableName), _fieldName(fieldName) {
  }

  std::string CreateIndexData::indexName() const {
    return _indexName;
  }

  std::string CreateIndexData::tableName() const {
    return _tableName;
  }

  std::string CreateIndexData::fieldName() const {
    return _fieldName;
  }

  int CreateIndexData::getObjectId() {
    return _objectId;
  }
}
