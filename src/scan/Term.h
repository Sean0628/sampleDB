#pragma once

#include <climits>
#include <string>
#include "plan/Plan.h"
#include "Expression.h"

namespace scan {
  class Term {
    public:
      Term() = default;
      Term(const Expression& lhs, const Expression& rhs);
      bool isSatisfied(Scan* scan) const;
      bool appliesTo(const record::Schema& schema) const;
      int reductionFactor(plan::Plan* plan) const;
      Constant equatesWithConstant(const std::string& fieldName) const;
      std::string equatesWithField(const std::string& fieldName) const;
      std::string toString() const;
    private:
      Expression _lhs;
      Expression _rhs;
  };
}
