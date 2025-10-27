#include "MergeJoinPlan.h"

namespace materialize {
  MergeJoinPlan::MergeJoinPlan(tx::Transaction* tx,
      const std::shared_ptr<plan::Plan>& p1,
      const std::shared_ptr<plan::Plan>& p2,
      const std::string& fld1,
      const std::string& fld2)
      : _p1(p1), _p2(p2), _fld1(fld1), _fld2(fld2) {
        std::vector<std::string> sortFields1 = {fld1};
        _p1 = std::static_pointer_cast<plan::Plan>(std::make_shared<SortPlan>(p1, sortFields1, tx));
        std::vector<std::string> sortFields2 = {fld2};
        _p2 = std::static_pointer_cast<plan::Plan>(std::make_shared<SortPlan>(p2, sortFields2, tx));
        _schema.addAll(p1->schema());
        _schema.addAll(p2->schema());
      }

  std::shared_ptr<scan::Scan> MergeJoinPlan::open() {
    auto s1 = _p1->open();
    auto s2 = std::static_pointer_cast<SortScan>(_p2->open());

    return std::static_pointer_cast<scan::Scan>(std::make_shared<MergeJoinScan>(s1, s2, _fld1, _fld2));
  }

  int MergeJoinPlan::blocksAccessed() const {
    return _p1->blocksAccessed() + _p2->blocksAccessed();
  }

  int MergeJoinPlan::recordsOutput() const {
    int maxValues = std::max(_p1->distinctValues(_fld1), _p2->distinctValues(_fld2));
    return (_p1->recordsOutput() * _p2->recordsOutput()) / maxValues;
  }

  int MergeJoinPlan::distinctValues(const std::string& fieldName) const {
    if (_p1->schema().hasField(fieldName)) return _p1->distinctValues(fieldName);

    return _p2->distinctValues(fieldName);
  }

  record::Schema MergeJoinPlan::schema() const {
    return _schema;
  }
}
