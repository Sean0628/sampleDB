#pragma once

#include <memory>
#include <string>
#include <vector>
#include "materialize/MaterializePlan.h"
#include "materialize/RecordComparator.h"
#include "materialize/TempTable.h"
#include "materialize/SortScan.h"
#include "BufferNeeds.h"
#include "plan/Plan.h"
#include "record/Schema.h"
#include "record/TableScan.h"
#include "scan/Scan.h"
#include "scan/UpdateScan.h"
#include "tx/Transaction.h"

namespace multibuffer {
  class MultiBufferSortPlan : public plan::Plan {
    public:
      MultiBuferSortPlan(const std::shared_ptr<plan::Plan>& p, const std::vector<std::string>& sortFields, tx::Transaction* tx);
      std::shared_ptr<scan::Scan> open() override;
      int blocksAccessed() override;
      int recordsOutput() override;
      int distinctValues(const std::string& fldname) override;
      record::Schema schema() override;
    private:
      tx::Transaction* _tx;
      std::shared_ptr<plan::Plan> _p;
      record::Schema _schema;
      materialize::RecordComparator _comparator;

      void sortTempTable(const std::shared_ptr<materialize::TempTable>& tt);
      std::vector<std::shared_ptr<materialize::TempTable>> splitIntoBlockWise(int runSize);
      std::vector<std::shared_ptr<materialize::TempTable>> doMergeIteration(const std::vector<std::shared_ptr<materialize::TempTable>>& tables, int runsPerMerge);
      bool copy(scan::Scan& src, scan::UpdateScan& dest);
  };
}
