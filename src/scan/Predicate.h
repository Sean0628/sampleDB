#pragma once

#include <vector>
#include <memory>
#include <string>
#include "plan/Plan.h"
#include "Scan.h"
#include "Term.h"
#include "record/Schema.h"

namespace scan {
  class Predicate {
    public:
      Predicate() = default;
      Predicate(const Term& term);
      Predicate(const Predicate& other);
      Predicate &operator=(const Predicate& other);
      void conjoinWith(const Predicate& other);
      bool isSatisfied(Scan* scan) const;
      int reductionFactor(plan::Plan* plan) const;
      Predicate selectSubPredicate(const record::Schema& schema) const;
      Predicate joinSubPredicate(const record::Schema& schema1, const record::Schema& schema2) const;
      Constant equatesWithConstant(const std::string& fieldName) const;
      std::string equatesWithField(const std::string& fieldName) const;
      std::string toString() const;
      bool isNull() const;
    private:
      std::vector<Term> _terms;
  };
}
