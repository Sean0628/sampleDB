#include "SelectPlan.h"

namespace plan {
  SelectPlan::SelectPlan(const std::shared_ptr<Plan>& p, const scan::Predicate& pred)
      : _plan(p), _predicate(pred) {
  }

  std::shared_ptr<scan::Scan> SelectPlan::open() {
    auto scan = _plan->open();
    return std::static_pointer_cast<scan::Scan>(
        std::make_shared<scan::SelectScan>(scan, _predicate));
  }

  int SelectPlan::blocksAccessed() const {
    return _plan->blocksAccessed();
  }

  int SelectPlan::recordsOutput() const {
    return _plan->recordsOutput() / _predicate.reductionFactor(_plan.get());
  }

  int SelectPlan::distinctValues(const std::string& fieldName) const {
    if (!_predicate.equatesWithConstant(fieldName).isNull()) {
      return 1; // If the predicate equates with a constant, only one distinct value
    } else {
      std::string fieldNameEquated = _predicate.equatesWithField(fieldName);
      if (!fieldNameEquated.empty()) {
        return std::min(_plan->distinctValues(fieldNameEquated), _plan->distinctValues(fieldNameEquated));
      } else {
        return _plan->distinctValues(fieldName);
      }
    }
  }

  record::Schema SelectPlan::schema() const {
    return _plan->schema();
  }
}
