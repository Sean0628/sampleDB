#pragma once

#include <string>
#include <memory>
#include <vector>
#include <set>

#include "CreateIndexData.h"
#include "CreateTableData.h"
#include "CreateViewData.h"
#include "InsertData.h"
#include "DeleteData.h"
#include "ModifyData.h"
#include "QueryData.h"
#include "Lexer.h"
#include "Object.h"
#include "Word.h"
#include "TableData.h"
#include "scan/Constant.h"
#include "scan/Expression.h"
#include "scan/Predicate.h"
#include "scan/Term.h"
#include "record/Schema.h"

namespace parse {
  class Parser {
    public:
      Parser(const std::string& query);
      std::string fieldName() const;
      scan::Constant constant() const;
      scan::Expression expression() const;
      scan::Term term() const;
      scan::Predicate predicate() const;
      QueryData query() const;
      std::shared_ptr<Object> updateCmd();
      std::shared_ptr<Object> create();
      // Methods to for parsing delete commands
      std::shared_ptr<DeleteData> remove();
      // Methods for parsing insert commands
      std::shared_ptr<InsertData> insert();
      // Methods for parsing modify commands
      std::shared_ptr<ModifyData> modify();
      // Methods for parsing create table commands
      std::shared_ptr<CreateTableData> createTable();
      // Methods for parsing create view commands
      std::shared_ptr<CreateViewData> createView();
      // Methods for parsing create index commands
      std::shared_ptr<CreateIndexData> createIndex();
    private:
      std::unique_ptr<Lexer> _lexer;
      std::vector<std::string> selectList() const;
      TableData tableList() const;
      std::vector<std::string> fieldList() const;
      std::vector<scan::Constant> constantList() const;
      std::vector<std::string> sortFieldList() const;
      std::vector<std::string> groupFieldList() const;

      record::Schema fieldDefinition() const;
      record::Schema fieldDefinitionList() const;
      record::Schema fieldType(const std::string& fieldName) const;
  };
}
