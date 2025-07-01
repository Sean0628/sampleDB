#pragma once

#include <set>
#include <string>
#include <vector>
#include "QueryData.h"

namespace parse {
  class TableData {
    public:
      TableData() = default;
      std::set<std::string> getTableNames() const;
      std::vector<QueryData> getQueryDataList() const;
      void addTableName(const std::string& tableName);
      void addQueryData(const QueryData& queryData);

    private:
      std::set<std::string> _tables;
      std::vector<QueryData> _queryData;
  };
}
