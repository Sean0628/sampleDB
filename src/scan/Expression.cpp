#include "Expression.h"

namespace scan {
  Expression::Expression(const Expression& other)
      : _value(other._value), _fieldName(other._fieldName) {
  }

  Expression::Expression(const Constant& value)
      : _value(value) {
  }

  Expression::Expression(const std::string& fieldName)
      : _fieldName(fieldName) {
  }

  Expression &Expression::operator=(const Expression& other) {
    if (this != &other) {
      _value = other._value;
      _fieldName = other._fieldName;
    }
    return *this;
  }

  bool Expression::isFieldName() const {
    return !_fieldName.empty();
  }

  Constant Expression::asConstant() const {
    return _value;
  }

  std::string Expression::asFieldName() const {
    return _fieldName;
  }

  Constant Expression::evaluate(Scan* scan) const {
    return _value.isNull() ? scan->getValue(_fieldName) : _value;
  }

  bool Expression::appliesTo(const record::Schema& schema) const {
    return _value.isNull() ? schema.hasField(_fieldName) : true;
  }

  std::string Expression::toString() const {
    return _value.isNull() ? _fieldName : _value.toString();
  }
}
