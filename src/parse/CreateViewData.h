#pragma once

#include <string>

#include "Object.h"
#include "ObjectID.h"
#include "QueryData.h"

namespace parse {
  class CreateViewData : public Object {
    public:
      CreateViewData(const std::string& viewName, const QueryData& queryData);
      std::string viewName() const;
      std::string viewDefinition() const;

      int getObjectId() override;
    private:
      std::string _viewName;
      QueryData _queryData;

      const int _objectId = ObjectID::CREATE_VIEW;
  };
}
