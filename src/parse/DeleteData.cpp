#include "DeleteData.h"

namespace parse {
  DeleteData::DeleteData(const std::string& tableName, const scan::Predicate& predicate)
      : _tableName(tableName), _predicate(predicate) {
  }

  std::string DeleteData::tableName() const {
    return _tableName;
  }

  scan::Predicate DeleteData::predicate() const {
    return _predicate;
  }

  int DeleteData::getObjectId() {
    return _objectId;
  }
}
