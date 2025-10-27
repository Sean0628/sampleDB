#pragma once

#include <string>
#include <vector>

#include "scan/Constant.h"
#include "scan/Scan.h"

namespace materialize {
  class RecordComparator {
    public:
      RecordComparator(const std::vector<std::string>& sortFields);
      int compare(scan::Scan* s1, scan::Scan* s2) const;
      int minIndex(const std::vector<std::shared_ptr<scan::Scan>>& scans) const;
    private:
      std::vector<std::string> _sortFields;
  };
}
