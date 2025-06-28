#pragma once

#include <string>

#include "Constant.h"
#include "record/Schema.h"
#include "Scan.h"

namespace scan {
  class Expression {
    public:
      Expression() = default;
      Expression(const Expression& other);
      Expression(const Constant& value);
      Expression(const std::string& fieldName);
      Expression &operator=(const Expression& other);
      bool isFieldName() const;
      Constant asConstant() const;
      std::string asFieldName() const;
      Constant evaluate(Scan* scan) const;
      bool appliesTo(const record::Schema& schema) const;
      std::string toString() const;
    private:
      Constant _value;
      std::string _fieldName;
  };
}
