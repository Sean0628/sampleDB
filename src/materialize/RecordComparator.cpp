#include "RecordComparator.h"

namespace materialize {
  RecordComparator::RecordComparator(const std::vector<std::string>& sortFields) : _sortFields(sortFields) {}

  int RecordComparator::compare(scan::Scan* s1, scan::Scan* s2) const {
    for (const auto& fieldName : _sortFields) {
      scan::Constant c1 = s1->getValue(fieldName);
      scan::Constant c2 = s2->getValue(fieldName);
      if (c1 < c2) return -1;
      if (c1 > c2) return 1;
    }

    return 0;
  }

  int RecordComparator::minIndex(const std::vector<std::shared_ptr<scan::Scan>>& scans) const {
    int minIdx = -1;
    for (int i = 0; i < scans.size(); i++) {
      if (compare(scans[i].get(), scans[i].get()) > 0) {
        minIdx = i;
      }
    }
    return minIdx;
  }
}
