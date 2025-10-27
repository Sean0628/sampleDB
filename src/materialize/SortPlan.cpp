#include "SortPlan.h"

#include <iostream>

namespace materialize {
  SortPlan::SortPlan(const std::shared_ptr<plan::Plan>& p, const std::vector<std::string>& sortFields, tx::Transaction* tx)
      : _p(p),  _tx(tx), _comparator(sortFields) {
        _schema = p->schema();
      }

  std::shared_ptr<scan::Scan> SortPlan::open() {
    auto src = _p->open();
    auto runs = splitIntoRuns(src.get());
    src->close();
    
    while (runs.size() > 2) {
      runs = doAMergeIteration(runs);
    }

    return std::static_pointer_cast<scan::Scan>(std::make_shared<SortScan>(runs, _comparator) );
  }

  int SortPlan::blocksAccessed() const {
    auto mp = std::static_pointer_cast<plan::Plan>(std::make_shared<MaterializePlan>(_tx, _p));
    return mp->blocksAccessed();
  }

  int SortPlan::recordsOutput() const {
    return _p->recordsOutput();
  }

  int SortPlan::distinctValues(const std::string& fieldName) const {
    return _p->distinctValues(fieldName);
  }

  record::Schema SortPlan::schema() const {
    return _schema;
  }

  std::vector<std::shared_ptr<TempTable>> SortPlan::splitIntoRuns(scan::Scan* src) {
    std::vector<std::shared_ptr<TempTable>> temps;
    src->beforeFirst();
    if (!src->next()) return temps;

    auto currentTemp = std::make_shared<TempTable>(_tx, _schema);
    temps.emplace_back(currentTemp);
    auto currentScan = currentTemp->open();

    while (copy(src, currentScan.get())) {
      if (_comparator.compare(src, currentScan.get()) < 0) {
        currentScan->close();
        currentTemp = std::make_shared<TempTable>(_tx, _schema);
        temps.emplace_back(currentTemp);
        currentScan = std::static_pointer_cast<scan::UpdateScan>(currentTemp->open());
      }
    }

    currentScan->close();
    return temps;
  }

  std::vector<std::shared_ptr<TempTable>> SortPlan::doAMergeIteration(const std::vector<std::shared_ptr<TempTable>>& runs) {
    std::vector<std::shared_ptr<TempTable>> result;
    for (int i = 0; i + 2 <= runs.size(); i += 2) {
      auto p1 = runs[i];
      auto p2 = runs[i + 1];

      result.emplace_back(mergeTwoRuns(p1.get(), p2.get()));
    }
    if (runs.size() % 2 == 1) {
      result.emplace_back(runs.back());
    }

    return result;
  }

  std::shared_ptr<TempTable> SortPlan::mergeTwoRuns(TempTable* run1, TempTable* run2) {
    auto src1 = run1->open();
    auto src2 = run2->open();
    auto result = std::make_shared<TempTable>(_tx, _schema);
    auto dest = result->open();

    bool hasmore1 = src1->next();
    bool hasmore2 = src2->next();
    while (hasmore1 && hasmore2) {
      if (_comparator.compare(src1.get(), src2.get()) < 0) {
        hasmore1 = copy(src1.get(), dest.get());
      } else {
        hasmore2 = copy(src2.get(), dest.get());
      }
    }

    if (hasmore1) {
      while (hasmore1) {
          hasmore1 = copy(src1.get(), dest.get());
      }
    } else {
      while (hasmore2) {
          hasmore2 = copy(src2.get(), dest.get());
      }
    }

    src1->close();
    src2->close();
    dest->close();
    return result;
  }

  bool SortPlan::copy(scan::Scan* src, scan::UpdateScan* dest) {
    dest->insert();
    for (const auto& fieldName : _schema.fieldNames()) {
      dest->setValue(fieldName, src->getValue(fieldName));
    }

    return src->next();
  }
}
