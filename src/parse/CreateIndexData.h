#pragma once

#include <string>

#include "Object.h"
#include "ObjectID.h"

namespace parse {
  class CreateIndexData : public Object {
    public:
      CreateIndexData(const std::string& indexname, const std::string& tableName, const std::string& fieldName);
      std::string indexName() const;
      std::string tableName() const;
      std::string fieldName() const;

      int getObjectId() override;

    private:
      std::string _indexName;
      std::string _tableName;
      std::string _fieldName;

      const int _objectId = ObjectID::CREATE_INDEX;
  };
}
