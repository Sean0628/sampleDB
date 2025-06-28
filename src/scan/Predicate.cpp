#include "Predicate.h"

namespace scan {
  Predicate::Predicate(const Term& term) {
    _terms.push_back(term);
  }

  Predicate::Predicate(const Predicate& other) : _terms(other._terms) {
  }

  Predicate& Predicate::operator=(const Predicate& other) {
    if (this != &other) {
      _terms = other._terms;
    }
    return *this;
  }

  void Predicate::conjoinWith(const Predicate& other) {
    _terms.insert(_terms.end(), other._terms.begin(), other._terms.end());
  }

  bool Predicate::isSatisfied(Scan* scan) const {
    for (const auto& term : _terms) {
      if (!term.isSatisfied(scan)) {
        return false;
      }
    }
    return true;
  }

  int Predicate::reductionFactor(plan::Plan* plan) const {
    int factor = 1;

    for (const auto& term : _terms) {
      factor *= term.reductionFactor(plan);
    }

    return factor;
  }

  Predicate Predicate::selectSubPredicate(const record::Schema& schema) const {
    Predicate result;
    for (const auto& term : _terms) {
      if (term.appliesTo(schema)) {
        result._terms.push_back(term);
      }
    }

    return result;
  }

  Predicate Predicate::joinSubPredicate(const record::Schema& schema1, const record::Schema& schema2) const {
    Predicate result;
    record::Schema newSchema;
    newSchema.addAll(schema1);
    newSchema.addAll(schema2);

    for (const auto& term : _terms) {
      if (!term.appliesTo(schema1) && !term.appliesTo(schema2) && term.appliesTo(newSchema)) {
        result._terms.push_back(term);
      }
    }

    return result;
  }

  Constant Predicate::equatesWithConstant(const std::string& fieldName) const {
    for (const auto& term : _terms) {
      Constant constant = term.equatesWithConstant(fieldName);
      if (!constant.isNull()) return constant;
    }

    Constant nullConstant;
    return nullConstant; // Return a null constant if no match found
  }

  std::string Predicate::equatesWithField(const std::string& fieldName) const {
    for (auto& term : _terms) {
      std::string field = term.equatesWithField(fieldName);
      if (!field.empty()) return field;
    }

    return ""; // Return an empty string if no match found
  }

  std::string Predicate::toString() const {
    std::string result = "";
    int cnt = 0;
    for (auto& term : _terms) {
      if (cnt == 0) {
        result += term.toString();
      } else {
        result += " and " + term.toString();
      }

      cnt++;
    }

    return result;
  }

  bool Predicate::isNull() const {
    return _terms.size() == 0;
  }
}
