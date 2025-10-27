#include "MaterializePlan.h"

namespace materialize {
  MaterializePlan::MaterializePlan(tx::Transaction* tx, const std::shared_ptr<plan::Plan>& plan)
      : _tx(tx), _plan(plan) {
      }

  std::shared_ptr<scan::Scan> MaterializePlan::open() {
    record::Schema sch = _plan->schema();
    auto temp = std::make_shared<TempTable>(_tx, sch);
    auto src = _plan->open();
    auto dest = temp->open();

    while (src->next()) {
      dest->insert();
      for (const auto& fieldName : sch.fieldNames()) {
        auto c = src->getValue(fieldName);
        dest->setValue(fieldName, c);
      }
    }

    src->close();
    dest->beforeFirst();
    return std::static_pointer_cast<scan::Scan>(dest);
  }

  int MaterializePlan::blocksAccessed() const {
    record::Layout layout(_plan->schema());
    double rpb = static_cast<double>(_tx->blockSize()) / layout.slotSize();
    return static_cast<int>(std::ceil(_plan->recordsOutput() / rpb));
  }

  int MaterializePlan::recordsOutput() const {
    return _plan->recordsOutput();
  }

  int MaterializePlan::distinctValues(const std::string& fieldName) const {
    return _plan->distinctValues(fieldName);
  }

  record::Schema MaterializePlan::schema() const {
    return _plan->schema();
  }
}
