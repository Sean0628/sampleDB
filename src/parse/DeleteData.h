#pragma once

#include <string>
#include "parse/Object.h"
#include "parse/ObjectID.h"
#include "scan/Predicate.h"

namespace parse {
  class DeleteData : public Object {
    public:
      DeleteData(const std::string& tableName, const scan::Predicate& predicate);
      std::string tableName() const;
      scan::Predicate predicate() const;

      int getObjectId() override;
    private:
      std::string _tableName;
      scan::Predicate _predicate;

      const int _objectId = ObjectID::DELETE;
  };
}
