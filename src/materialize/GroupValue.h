#pragma once

#include <map>
#include <string>
#include <vector>

#include "scan/Constant.h"
#include "scan/Scan.h"

namespace materialize {
  class GroupValue {
    friend bool operator==(const GroupValue& gv1, const GroupValue& gv2);
    friend bool operator!=(const GroupValue& gv1, const GroupValue& gv2);

    public:
      GroupValue(scan::Scan* s, const std::vector<std::string>& groupFields);
      scan::Constant getValue(const std::string& fieldName) const;
      int hashCode() const;
    private:
      std::map<std::string, scan::Constant> _values;
  };
}
