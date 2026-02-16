#pragma once

#include <memory>
#include <string>
#include <vector>

#include "TempTable.h"
#include "RecordComparator.h"
#include "plan/Plan.h"
#include "record/Schema.h"
#include "record/TableScan.h"
#include "record/RID.h"
#include "tx/Transaction.h"
#include "scan/Constant.h"
#include "scan/Scan.h"
#include "scan/UpdateScan.h"

namespace materialize {
  class SortScan : public scan::Scan {
    public:
      SortScan(const std::vector<std::shared_ptr<TempTable>>& runs, const RecordComparator& comparator);
      void beforeFirst() override;
      bool next() override;
      void close() override;
      scan::Constant getValue(const std::string& fieldName) override;
      int getInt(const std::string& fieldName) override;
      bool getBool(const std::string& fieldName) override;
      std::string getString(const std::string& fieldName) override;
      bool hasField(const std::string& fieldName) override;
      void savePosition();
      void restorePosition();
    private:
      std::shared_ptr<scan::UpdateScan> _s1, _s2, _currentScan;
      RecordComparator _comparator;
      bool _hasMore1 = false, _hasMore2 = false;
      std::vector<record::RID> _savedPosition;
  };
}
