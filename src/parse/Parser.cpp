#include "parse/Parser.h"

namespace parse {
  Parser::Parser(const std::string& query) {
    _lexer = std::make_unique<Lexer>(query);
  }

  std::string Parser::fieldName() const {
    return _lexer->eatIdentifier();
  }

  scan::Constant Parser::constant() const {
    if (_lexer->matchString()) {
      scan::Constant c(_lexer->eatString());
      return c;
    } else {
      scan::Constant c(_lexer->eatInteger());
      return c;
    }
  }

  scan::Expression Parser::expression() const {
    if (_lexer->matchIdentifier()) {
      scan::Expression expr(fieldName());
      return expr;
    } else {
      scan::Expression expr(constant());
      return expr;
    }
  }

  scan::Term Parser::term() const {
    scan::Expression lhs = expression();
    _lexer->eatDelimiter(Word::EQUAL_SIGN);
    scan::Expression rhs = expression();
    scan::Term term(lhs, rhs);
    return term;
  }

  scan::Predicate Parser::predicate() const {
    scan::Predicate pred(term());
    if (_lexer->matchKeyword(Word::AMPERSAND)) {
      _lexer->eatKeyword(Word::AMPERSAND);
      pred.conjoinWith(predicate());
    }

    return pred;
  }

  QueryData Parser::query() const {
    _lexer->eatKeyword(Word::SELECT);
    std::vector<std::string> fields = selectList();

    _lexer->eatKeyword(Word::FROM);
    TableData tableData = tableList();

    scan::Predicate pred;
    if (_lexer->matchKeyword(Word::WHERE)) {
      _lexer->eatKeyword(Word::WHERE);
      pred = predicate();
    }

    std::vector<std::string> groupFields = groupFieldList();
    std::vector<std::string> sortFields = sortFieldList();

    QueryData queryData(fields, tableData.getTableNames(), pred, groupFields, sortFields);
    for (const auto& qd : tableData.getQueryDataList()) {
      queryData.addQueryData(qd);
    }

    return queryData;
  }

  std::vector<std::string> Parser::selectList() const {
    std::vector<std::string> list;
    list.push_back(fieldName());

    if (_lexer->matchDelimiter(Word::COMMA)) {
      _lexer->eatDelimiter(Word::COMMA);

      for (const auto& field : selectList()) {
        list.push_back(field);
      }
    }

    return list;
  }

  TableData Parser::tableList() const {
    TableData tableData;
    if (_lexer->matchDelimiter(Word::LEFT_PAREN)) {
      _lexer->eatDelimiter(Word::LEFT_PAREN);
      QueryData queryData = query();
      tableData.addQueryData(queryData);
      _lexer->eatDelimiter(Word::RIGHT_PAREN);
    } else {
      tableData.addTableName(_lexer->eatIdentifier());
    }

    while (_lexer->matchDelimiter(Word::COMMA)) {
      _lexer->eatDelimiter(Word::COMMA);
      if (_lexer->matchDelimiter(Word::LEFT_PAREN)) {
        _lexer->eatDelimiter(Word::LEFT_PAREN);
        QueryData queryData = query();
        tableData.addQueryData(queryData);
        _lexer->eatDelimiter(Word::RIGHT_PAREN);
      } else {
        tableData.addTableName(_lexer->eatIdentifier());
      }
    }

    return tableData;
  }

  std::shared_ptr<Object> Parser::updateCmd() {
    if (_lexer->matchKeyword(Word::INSERT)) {
      return std::static_pointer_cast<Object>(insert());
    } else if (_lexer->matchKeyword(Word::DELETE)) {
      return std::static_pointer_cast<Object>(remove());
    } else if (_lexer->matchKeyword(Word::UPDATE)) {
      return std::static_pointer_cast<Object>(modify());
    } else {
      return std::static_pointer_cast<Object>(create());
    }
  }

  std::shared_ptr<Object> Parser::create() {
    _lexer->eatKeyword(Word::CREATE);
    if (_lexer->matchKeyword(Word::TABLE)) {
      return std::static_pointer_cast<Object>(createTable());
    } else if (_lexer->matchKeyword(Word::VIEW)) {
      return std::static_pointer_cast<Object>(createView());
    } else {
      return std::static_pointer_cast<Object>(createIndex());
    }
  }

  std::shared_ptr<DeleteData> Parser::remove() {
    _lexer->eatKeyword(Word::DELETE);
    _lexer->eatKeyword(Word::FROM);
    std::string tableName = _lexer->eatIdentifier();
    scan::Predicate pred;
    if (_lexer->matchKeyword(Word::WHERE)) {
      _lexer->eatKeyword(Word::WHERE);
      pred = predicate();
    }

    return std::make_shared<DeleteData>(tableName, pred);
  }

  std::shared_ptr<InsertData> Parser::insert() {
    _lexer->eatKeyword(Word::INSERT);
    _lexer->eatKeyword(Word::INTO);
    std::string tableName = _lexer->eatIdentifier();
    _lexer->eatDelimiter(Word::LEFT_PAREN);
    std::vector<std::string> fields = fieldList();
    _lexer->eatDelimiter(Word::RIGHT_PAREN);
    _lexer->eatKeyword(Word::VALUES);
    _lexer->eatDelimiter(Word::LEFT_PAREN);
    std::vector<scan::Constant> values = constantList();
    _lexer->eatDelimiter(Word::RIGHT_PAREN);

    return std::make_shared<InsertData>(tableName, fields, values);
  }

  std::vector<std::string> Parser::fieldList() const {
    std::vector<std::string> list;
    list.push_back(fieldName());
    if (_lexer->matchDelimiter(Word::COMMA)) {
      _lexer->eatDelimiter(Word::COMMA);
      for (const auto& field : fieldList()) {
        list.push_back(field);
      }
    }

    return list;
  }

  std::vector<std::string> Parser::groupFieldList() const {
    std::vector<std::string> list;
    if (_lexer->matchKeyword(Word::GROUP)) {
      _lexer->eatKeyword(Word::GROUP);
      if (_lexer->matchKeyword(Word::BY)) {
        _lexer->eatKeyword(Word::BY);
        for (const auto& field : fieldList()) {
          list.push_back(field);
        }
      }
    }

    return list;
  }

  std::vector<std::string> Parser::sortFieldList() const {
    std::vector<std::string> list;
    if (_lexer->matchKeyword(Word::ORDER)) {
      _lexer->eatKeyword(Word::ORDER);
      if (_lexer->matchKeyword(Word::BY)) {
        _lexer->eatKeyword(Word::BY);

        for (const auto& field : fieldList()) {
          list.push_back(field);
        }
      }
    }

    return list;
  }

  std::vector<scan::Constant> Parser::constantList() const {
    std::vector<scan::Constant> list;
    list.push_back(constant());
    if (_lexer->matchDelimiter(Word::COMMA)) {
      _lexer->eatDelimiter(Word::COMMA);
      for (const auto& value : constantList()) {
        list.push_back(value);
      }
    }

    return list;
  }

  std::shared_ptr<ModifyData> Parser::modify() {
    _lexer->eatKeyword(Word::UPDATE);
    std::string tableName = _lexer->eatIdentifier();
    _lexer->eatKeyword(Word::SET);
    std::string fdName = fieldName();
    _lexer->eatDelimiter(Word::EQUAL_SIGN);
    scan::Expression newValue;
    scan::Predicate pred;
    if (_lexer->matchKeyword(Word::WHERE)) {
      _lexer->eatKeyword(Word::WHERE);
      pred = predicate();
    }

    return std::make_shared<ModifyData>(tableName, fdName, newValue, pred);
  }

  std::shared_ptr<CreateTableData> Parser::createTable() {
    _lexer->eatKeyword(Word::TABLE);
    std::string tableName = _lexer->eatIdentifier();
    _lexer->eatDelimiter(Word::LEFT_PAREN);
    record::Schema schema = fieldDefinitionList();
    _lexer->eatDelimiter(Word::RIGHT_PAREN);

    return std::make_shared<CreateTableData>(tableName, schema);
  }

  record::Schema Parser::fieldDefinitionList() const {
    record::Schema schema = fieldDefinition();
    if (_lexer->matchDelimiter(Word::COMMA)) {
      _lexer->eatDelimiter(Word::COMMA);
      record::Schema nextSchema = fieldDefinitionList();
      schema.addAll(nextSchema);
    }

    return schema;
  }

  record::Schema Parser::fieldDefinition() const {
    std::string fdName = fieldName();
    return fieldType(fdName);
  }

  record::Schema Parser::fieldType(const std::string& fieldName) const {
    record::Schema schema;
    if (_lexer->matchKeyword(Word::INT)) {
      _lexer->eatKeyword(Word::INT);
      schema.addIntField(fieldName);
    } else {
      _lexer->eatKeyword(Word::VARCHAR);
      _lexer->eatDelimiter(Word::LEFT_PAREN);
      int stringLength = _lexer->eatInteger();
      _lexer->eatDelimiter(Word::RIGHT_PAREN);
      schema.addStringField(fieldName, stringLength);
    }

    return schema;
  }

  std::shared_ptr<CreateViewData> Parser::createView() {
    _lexer->eatKeyword(Word::VIEW);
    std::string viewName = _lexer->eatIdentifier();
    _lexer->eatKeyword(Word::AS);
    QueryData queryData = query();

    return std::make_shared<CreateViewData>(viewName, queryData);
  }

  std::shared_ptr<CreateIndexData> Parser::createIndex() {
    _lexer->eatKeyword(Word::INDEX);
    std::string indexName = _lexer->eatIdentifier();
    _lexer->eatKeyword(Word::ON);
    std::string tableName = _lexer->eatIdentifier();
    _lexer->eatDelimiter(Word::LEFT_PAREN);
    std::string fdName = fieldName();
    _lexer->eatDelimiter(Word::RIGHT_PAREN);

    return std::make_shared<CreateIndexData>(indexName, tableName, fdName);
  }
}
