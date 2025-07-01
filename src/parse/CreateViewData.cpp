#include "CreateViewData.h"

namespace parse {
  CreateViewData::CreateViewData(const std::string& viewName, const QueryData& queryData)
      : _viewName(viewName), _queryData(queryData) {
  }

  std::string CreateViewData::viewName() const {
    return _viewName;
  }

  std::string CreateViewData::viewDefinition() const {
    return _queryData.toString();
  }

  int CreateViewData::getObjectId() {
    return _objectId;
  }
}
