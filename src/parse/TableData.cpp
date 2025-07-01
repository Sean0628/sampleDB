#include "TableData.h"

namespace parse {
  std::set<std::string> TableData::getTableNames() const {
    return _tables;
  }

  std::vector<QueryData> TableData::getQueryDataList() const {
    return _queryData;
  }

  void TableData::addTableName(const std::string& tableName) {
    _tables.insert(tableName);
  }

  void TableData::addQueryData(const QueryData& queryData) {
    _queryData.push_back(queryData);
  }
}
