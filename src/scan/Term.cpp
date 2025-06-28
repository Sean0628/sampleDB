#include "Term.h"

namespace scan {
  Term::Term(const Expression& lhs, const Expression& rhs)
      : _lhs(lhs), _rhs(rhs) {
  }

  bool Term::isSatisfied(Scan* scan) const {
    Constant lhsValue = _lhs.evaluate(scan);
    Constant rhsValue = _rhs.evaluate(scan);
    return lhsValue == rhsValue;
  }

  bool Term::appliesTo(const record::Schema& schema) const {
    return _lhs.appliesTo(schema) && _rhs.appliesTo(schema);
  }

  int Term::reductionFactor(plan::Plan* plan) const {
    std::string lhsName, rhsName;
    if (_lhs.isFieldName() && _rhs.isFieldName()) {
      lhsName = _lhs.asFieldName();
      rhsName = _rhs.asFieldName();
      return std::max(plan->distinctValues(lhsName), plan->distinctValues(rhsName));
    }

    if (_lhs.isFieldName()) {
      lhsName = _lhs.asFieldName();
      return plan->distinctValues(lhsName);
    }

    if (_rhs.isFieldName()) {
      rhsName = _rhs.asFieldName();
      return plan->distinctValues(rhsName);
    }

    if (_lhs.asConstant() == _rhs.asConstant()) {
      return 1;
    }

    return INT_MAX;
  }

  Constant Term::equatesWithConstant(const std::string& fieldName) const {
    if (_lhs.isFieldName() && _lhs.asFieldName() == fieldName && !_rhs.isFieldName()) {
      return _rhs.asConstant();
    } else if (_rhs.isFieldName() && _rhs.asFieldName() == fieldName && !_lhs.isFieldName()) {
      return _lhs.asConstant();
    } else {
      Constant constant;
      return constant; // Return a null constant if no match found
    }
  }

  std::string Term::equatesWithField(const std::string& fieldName) const {
    if (_lhs.isFieldName() && _lhs.asFieldName() == fieldName && _rhs.isFieldName()) {
        return _rhs.asFieldName();
    } else if (_rhs.isFieldName() && _rhs.asFieldName() == fieldName && _lhs.isFieldName()) {
      return _lhs.asFieldName();
    } else {
      return ""; // Return an empty string if no match found
    }
  }

  std::string Term::toString() const {
    return _lhs.toString() + "=" + _rhs.toString();
  }
}
