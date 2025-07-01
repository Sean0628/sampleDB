#pragma once

#include <string>

#include "Object.h"
#include "ObjectID.h"
#include "scan/Expression.h"
#include "scan/Predicate.h"

namespace parse {
  class ModifyData : public Object {
    public:
      ModifyData(const std::string& tableName, const std::string& fieldName, const scan::Expression& newValue, const scan::Predicate& predicate);
      std::string tableName() const;
      std::string targetField() const;
      scan::Expression newValue() const;
      scan::Predicate predicate() const;

      int getObjectId() override;
    private:
      std::string _tableName;
      std::string _fieldName;
      scan::Predicate _predicate;
      scan::Expression _newValue;

      const int _objectId = ObjectID::MODIFY;
  };
}
