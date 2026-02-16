#include "GroupByScan.h"

namespace materialize {
  GroupByScan::GroupByScan(const std::shared_ptr<scan::Scan>& src,
      const std::vector<std::string>& groupFields,
      const std::vector<std::shared_ptr<AggregationFn>>& aggFns)
      : _src(src), _groupFields(groupFields), _aggFns(aggFns) {
        beforeFirst();
      }

  void GroupByScan::beforeFirst() {
    _src->beforeFirst();
    _moreGroups = _src->next();
  }

  bool GroupByScan::next() {
    if (!_moreGroups) return false;

    for (const auto& ptr : _aggFns) ptr->processFirst(_src.get());
    _currentGroup = std::make_unique<GroupValue>(_src.get(), _groupFields);

    while ((_moreGroups = _src->next())) {
      auto gv = std::make_unique<GroupValue>(_src.get(), _groupFields);
      if (*_currentGroup != *gv) break;

      for (const auto& ptr : _aggFns) ptr->processNext(_src.get());
    }

    return true;
  }

  void GroupByScan::close() {
    _src->close();
  }

  scan::Constant GroupByScan::getValue(const std::string& fieldName) {
    if (std::find(_groupFields.begin(), _groupFields.end(), fieldName) != _groupFields.end()) {
      return _currentGroup->getValue(fieldName);
    }

    for (const auto& fn : _aggFns) {
      if (fn->fieldName() == fieldName) {
        return fn->value();
      }
    }

    throw std::runtime_error("GroupByScan: unknown field '" + fieldName + "'");
  }

  int GroupByScan::getInt(const std::string& fieldName) {
    return getValue(fieldName).asInt();
  }

  bool GroupByScan::getBool(const std::string& fieldName) {
    return getValue(fieldName).asInt() != 0;
  }

  std::string GroupByScan::getString(const std::string& fieldName) {
    return getValue(fieldName).asString();
  }

  bool GroupByScan::hasField(const std::string& fieldName) {
    if (std::find(_groupFields.begin(), _groupFields.end(), fieldName) != _groupFields.end()) {
      return true;
    }

    for (const auto& fn : _aggFns) {
      if (fn->fieldName() == fieldName) {
        return true;
      }
    }
    return false;
  }
}
