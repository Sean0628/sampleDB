#include "ModifyData.h"

namespace parse {
  ModifyData::ModifyData(const std::string& tableName, const std::string& fieldName, const scan::Expression& newValue, const scan::Predicate& predicate)
      : _tableName(tableName), _fieldName(fieldName), _newValue(newValue), _predicate(predicate) {
  }

  std::string ModifyData::tableName() const {
    return _tableName;
  }

  std::string ModifyData::targetField() const {
    return _fieldName;
  }

  scan::Expression ModifyData::newValue() const {
    return _newValue;
  }

  scan::Predicate ModifyData::predicate() const {
    return _predicate;
  }

  int ModifyData::getObjectId() {
    return _objectId;
  }
}
