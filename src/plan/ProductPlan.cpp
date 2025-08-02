#include "ProductPlan.h"

namespace plan {
  ProductPlan::ProductPlan(const std::shared_ptr<Plan>& p1, const std::shared_ptr<Plan>& p2)
      : _plan1(p1), _plan2(p2) {
    _schema.addAll(p1->schema());
    _schema.addAll(p2->schema());
  }

  std::shared_ptr<scan::Scan> ProductPlan::open() {
    auto scan1 = _plan1->open();
    auto scan2 = _plan2->open();
    return std::static_pointer_cast<scan::Scan>(
        std::make_shared<scan::ProductScan>(scan1, scan2));
  }

  int ProductPlan::blocksAccessed() const {
    return _plan1->blocksAccessed() + (_plan1->recordsOutput() * _plan2->blocksAccessed());
  }

  int ProductPlan::recordsOutput() const {
    return _plan1->recordsOutput() * _plan2->recordsOutput();
  }

  int ProductPlan::distinctValues(const std::string& fieldName) const {
    if (_plan1->schema().hasField(fieldName)) {
      return _plan1->distinctValues(fieldName);
    } else {
      return _plan2->distinctValues(fieldName);
    }
  }

  record::Schema ProductPlan::schema() const {
    return _schema;
  }
}
