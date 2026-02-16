#include "MergeJoinScan.h"

namespace materialize {
  MergeJoinScan::MergeJoinScan(const std::shared_ptr<scan::Scan>& s1,
      const std::shared_ptr<SortScan>& s2,
      const std::string& fld1,
      const std::string& fld2)
      : _s1(s1), _s2(s2), _fld1(fld1), _fld2(fld2) {
        beforeFirst();
      }

  void MergeJoinScan::close() {
    _s1->close();
    _s2->close();
  }

  void MergeJoinScan::beforeFirst() {
    _s1->beforeFirst();
    _s2->beforeFirst();
  }

  bool MergeJoinScan::next() {
    bool hasMore2 = _s2->next();
    if (!hasMore2 && _s2->getValue(_fld2) == _joinValue) {
      return true;
    }

    bool hasMore1 = _s1->next();
    if (hasMore1 && _s1->getValue(_fld1) == _joinValue) {
      _s2->restorePosition();
      return true;
    }

    while (hasMore1 && hasMore2) {
      scan::Constant v1 = _s1->getValue(_fld1);
      scan::Constant v2 = _s2->getValue(_fld2);

      if (v1 < v2) {
        hasMore1 = _s1->next();
      } else if (v1 > v2) {
        hasMore2 = _s2->next();
      } else {
        _s2->savePosition();
        _joinValue = _s2->getValue(_fld2);
        return true;
      }
    }

    return false;
  }

  int MergeJoinScan::getInt(const std::string& fieldName) {
    return _s1->hasField(fieldName) ? _s1->getInt(fieldName) : _s2->getInt(fieldName);
  }

  bool MergeJoinScan::getBool(const std::string& fieldName) {
    return _s1->hasField(fieldName) ? _s1->getBool(fieldName) : _s2->getBool(fieldName);
  }

  std::string MergeJoinScan::getString(const std::string& fieldName) {
    return _s1->hasField(fieldName) ? _s1->getString(fieldName) : _s2->getString(fieldName);
  }

  scan::Constant MergeJoinScan::getValue(const std::string& fieldName) {
    return _s1->hasField(fieldName) ? _s1->getValue(fieldName) : _s2->getValue(fieldName);
  }

  bool MergeJoinScan::hasField(const std::string& fieldName) {
    return _s1->hasField(fieldName) || _s2->hasField(fieldName);
  }
}
