#pragma once

#include <string>

#include "Object.h"
#include "ObjectID.h"
#include "record/Schema.h"

namespace parse {
  class CreateTableData : public Object {
    public:
      CreateTableData(const std::string& tableName, const record::Schema& schema);
      std::string tableName() const;
      record::Schema newSchema() const;

      int getObjectId() override;
    private:
      std::string _tableName;
      record::Schema _schema;

      const int _objectId = ObjectID::CREATE_TABLE;
  };
}
