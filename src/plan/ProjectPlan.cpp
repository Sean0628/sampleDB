#include "ProjectPlan.h"

namespace plan {
  ProjectPlan::ProjectPlan(const std::shared_ptr<Plan>& p, const std::vector<std::string>& fields) 
      : _plan(p) {
    for (const auto& fieldName : fields) {
      _schema.add(fieldName, _plan->schema());
    }
  }

  std::shared_ptr<scan::Scan> ProjectPlan::open() {
    auto scan = _plan->open();
    return std::static_pointer_cast<scan::Scan>(std::make_shared<scan::ProjectScan>(scan, _schema.fieldNames()));
  }

  int ProjectPlan::blocksAccessed() const {
    return _plan->blocksAccessed();
  }

  int ProjectPlan::recordsOutput() const {
    return _plan->recordsOutput();
  }

  int ProjectPlan::distinctValues(const std::string& fieldName) const {
    return _plan->distinctValues(fieldName);
  }

  record::Schema ProjectPlan::schema() const {
    return _schema;
  }
}
