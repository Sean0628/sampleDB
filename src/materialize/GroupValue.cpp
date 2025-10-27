#include "GroupValue.h"

namespace materialize {
  bool operator==(const GroupValue& gv1, const GroupValue& gv2) {
    for (const auto& [fieldName, value] : gv1._values) {
      if (gv2._values.find(fieldName) == gv2._values.end() || gv2._values.at(fieldName) != value) {
        return false;
      }
    }

    return true;
  }

  bool operator!=(const GroupValue& gv1, const GroupValue& gv2) {
    return !(gv1 == gv2);
  }

  GroupValue::GroupValue(scan::Scan* s, const std::vector<std::string>& groupFields) {
    for (const auto& fieldName : groupFields) {
      _values[fieldName] = s->getValue(fieldName);
    }
  }

  scan::Constant GroupValue::getValue(const std::string& fieldName) const {
    return _values.at(fieldName);
  }

  int GroupValue::hashCode() const {
    int hash = 0;
    for (const auto& [fieldName, value] : _values) {
      hash += value.hashCode();
    }

    return hash;
  }
}
