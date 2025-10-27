#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "MaterializePlan.h"
#include "RecordComparator.h"
#include "SortScan.h"
#include "TempTable.h"
#include "plan/Plan.h"
#include "scan/UpdateScan.h"
#include "record/Schema.h"
#include "record/TableScan.h"
#include "tx/Transaction.h"
#include "scan/Scan.h"
#include "scan/UpdateScan.h"

namespace materialize {
  class SortPlan : public plan::Plan {
    public:
      SortPlan(const std::shared_ptr<plan::Plan>& p, const std::vector<std::string>& sortFields, tx::Transaction* tx);
      std::shared_ptr<scan::Scan> open() override;
      int blocksAccessed() const override;
      int recordsOutput() const override;
      int distinctValues(const std::string& fieldName) const override;
      record::Schema schema() const override;
    private:
      std::shared_ptr<plan::Plan> _p;
      record::Schema _schema;
      tx::Transaction* _tx;
      RecordComparator _comparator;

      std::vector<std::shared_ptr<TempTable>> splitIntoRuns(scan::Scan* src);
      std::vector<std::shared_ptr<TempTable>> doAMergeIteration(const std::vector<std::shared_ptr<TempTable>>& runs);
      std::shared_ptr<TempTable> mergeTwoRuns(TempTable* run1, TempTable* run2);
      bool copy(scan::Scan* src, scan::UpdateScan* dest);
  };
}
