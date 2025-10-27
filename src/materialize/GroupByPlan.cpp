#include "GroupByPlan.h"

namespace materialize {
  GroupByPlan::GroupByPlan(tx::Transaction* tx,
      const std::shared_ptr<plan::Plan>& plan,
      const std::vector<std::string>& groupFields,
      const std::vector<std::shared_ptr<AggregationFn>>& aggFns)
      : _plan(plan), _groupFields(groupFields), _aggFns(aggFns) {
        _plan = std::static_pointer_cast<plan::Plan>(std::make_shared<SortPlan>(_plan, _groupFields, tx));
        for (const auto& fieldName : _groupFields) {
          _schema.add(fieldName, _plan->schema());
        }

        for (const auto& fn : aggFns) {
          _schema.addIntField(fn->fieldName());
        }
  }

  std::shared_ptr<scan::Scan> GroupByPlan::open() {
    auto src = _plan->open();
    return std::static_pointer_cast<scan::Scan>(std::make_shared<GroupByScan>(src, _groupFields, _aggFns));
  }

  int GroupByPlan::blocksAccessed() const {
    return _plan->blocksAccessed();
  }

  int GroupByPlan::recordsOutput() const {
    int numGroups = 1;
    for (const auto& fieldName : _groupFields) {
      numGroups *= _plan->distinctValues(fieldName);
    }

    return numGroups;
  }

  int GroupByPlan::distinctValues(const std::string& fieldName) const {
    if (_plan->schema().hasField(fieldName)) {
      return _plan->distinctValues(fieldName);
    } else {
      return recordsOutput();
    }
  }

  record::Schema GroupByPlan::schema() const {
    return _schema;
  }
}
