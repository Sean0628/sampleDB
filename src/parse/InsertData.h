#pragma once

#include <string>
#include <vector>

#include "Object.h"
#include "ObjectId.h"
#include "scan/Constant.h"

namespace parse {
  class InsertData : public Object {
    public:
      InsertData(const std::string& tableName, const std::vector<std::string>& fieldNames, const std::vector<scan::Constant>& values);
      std::string tableName() const;
      std::vector<std::string> fieldNames() const;
      std::vector<scan::Constant> values() const;

      int getObjectId() override;
    private:
      std::string _tableName;
      std::vector<std::string> _fieldNames;
      std::vector<scan::Constant> _values;

      const int _objectId = ObjectID::INSERT;
  };
}
