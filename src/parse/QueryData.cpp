#include "QueryData.h"

namespace parse {
  QueryData::QueryData(const std::vector<std::string>& fields, 
                       const std::set<std::string>& tables, 
                       const scan::Predicate& pred, 
                       const std::vector<std::string>& groupFields, 
                       const std::vector<std::string>& sortFields)
      : _fields(fields), _tables(tables), _predicate(pred), 
        _groupFields(groupFields), _sortFields(sortFields) {
  }

  std::vector<std::string> QueryData::getFields() const {
    return _fields;
  }

  std::set<std::string> QueryData::getTables() const {
    return _tables;
  }

  std::vector<QueryData> QueryData::queryTables() const {
    return _queryTables;
  }

  void QueryData::addQueryData(const QueryData& queryData) {
    _queryTables.push_back(queryData);
  }

  scan::Predicate QueryData::getPredicate() const {
    return _predicate;
  }

  std::vector<std::string> QueryData::getGroupFields() const {
    return _groupFields;
  }

  std::vector<std::string> QueryData::getSortFields() const {
    return _sortFields;
  }

  std::string QueryData::toString() const {
    std::string result = Word::SELECT + Word::SPACE;
    for (const auto& field : _fields) {
      result += field + Word::COMMA + Word::SPACE;
    }
    result = result.substr(0, result.size() - 2); // Remove last comma and space

    result += Word::SPACE + Word::FROM + Word::SPACE;
    for (const auto& table : _tables) {
      result += table + Word::COMMA + Word::SPACE;
    }
    for (const auto& queryData : _queryTables) {
      result += Word::LEFT_PAREN + Word::SPACE;
      result += queryData.toString();
      result += Word::SPACE + Word::RIGHT_PAREN + Word::COMMA + Word::SPACE;
    }
    result = result.substr(0, result.size() - 2); // Remove last comma and space

    std::string predicateStr = _predicate.toString();
    if (!predicateStr.empty()) {
      result += Word::SPACE + Word::WHERE + Word::SPACE + predicateStr;
    }

    if (_groupFields.size() > 0) {
      result += Word::SPACE + Word::GROUP + Word::SPACE + Word::BY + Word::SPACE;
      for (const auto& groupField : _groupFields) {
        result += groupField + Word::COMMA + Word::SPACE;
      }
      result = result.substr(0, result.size() - 2); // Remove last comma and space
    }

    if (_sortFields.size() > 0) {
      result += Word::SPACE + Word::ORDER + Word::SPACE + Word::BY + Word::SPACE;
      for (const auto& sortField : _sortFields) {
        result += sortField + Word::COMMA + Word::SPACE;
      }
      result = result.substr(0, result.size() - 2); // Remove last comma and space
    }

    return result;
  }
}
