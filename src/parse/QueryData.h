#pragma once

#include <set>
#include <string>
#include <vector>

#include "Word.h"
#include "scan/Constant.h"
#include "scan/Predicate.h"

namespace parse {
  class QueryData {
    public:
      QueryData(const std::vector<std::string>& fields, const std::set<std::string>& tables, const scan::Predicate& pred, const std::vector<std::string>& groupFields, const std::vector<std::string>& sortFields);
      std::vector<std::string> getFields() const;
      std::set<std::string> getTables() const;
      std::vector<QueryData> queryTables() const;
      void addQueryData(const QueryData& queryData);
      scan::Predicate getPredicate() const;
      std::vector<std::string> getGroupFields() const;
      std::vector<std::string> getSortFields() const;
      std::string toString() const;
    private:
      std::vector<std::string> _fields;
      std::set<std::string> _tables;
      std::vector<QueryData> _queryTables;
      scan::Predicate _predicate;
      std::vector<std::string> _groupFields;
      std::vector<std::string> _sortFields;
  };
}
