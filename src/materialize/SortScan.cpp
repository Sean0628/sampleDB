#include "SortScan.h"

namespace materialize {
  SortScan::SortScan(const std::vector<std::shared_ptr<TempTable>>& runs, const RecordComparator& comparator)
      : _comparator(comparator) {
        _s1 = std::static_pointer_cast<scan::UpdateScan>(runs[0]->open());
        _hasMore1 = _s1->next();

        if (runs.size() > 1) {
          _s2 = std::static_pointer_cast<scan::UpdateScan>(runs[1]->open());
          _hasMore2 = _s2->next();
        }
      }

  void SortScan::beforeFirst() {
    _s1->beforeFirst();
    _hasMore1 = _s1->next();
    if (_s2) {
      _s2->beforeFirst();
      _hasMore2 = _s2->next();
    }

    _currentScan.reset();
  }

  bool SortScan::next() {
    if (_currentScan) {
      if (_currentScan == _s1) {
        _hasMore1 = _s1->next();
      } else if (_currentScan == _s2) {
        _hasMore2 = _s2->next();
      }
    }

    if (!_hasMore1 && !_hasMore2) {
      return false;
    } else if (_hasMore1 && _hasMore2) {
      if (_comparator.compare(_s1.get(), _s2.get()) < 0) {
        _currentScan = _s1;
      } else {
        _currentScan = _s2;
      }
    } else if (_hasMore1) {
      _currentScan = _s1;
    } else if (_hasMore2) {
      _currentScan = _s2;
    }

    return true;
  }

  void SortScan::close() {
    _s1->close();

    if (_s2) _s2->close();
  }

  scan::Constant SortScan::getValue(const std::string& fieldName) {
    return _currentScan->getValue(fieldName);
  }

  int SortScan::getInt(const std::string& fieldName) {
    return _currentScan->getInt(fieldName);
  }

  bool SortScan::getBool(const std::string& fieldName) {
    return _currentScan->getBool(fieldName);
  }

  std::string SortScan::getString(const std::string& fieldName) {
    return _currentScan->getString(fieldName);
  }

  bool SortScan::hasField(const std::string& fieldName) {
    return _currentScan->hasField(fieldName);
  }

  void SortScan::savePosition() {
    record::RID rid1 = _s1->getRid();
    record::RID rid2 = _s2->getRid();
    _savedPosition = std::vector<record::RID>{rid1, rid2};
  }

  void SortScan::restorePosition() {
    record::RID rid1 = _savedPosition[0];
    record::RID rid2 = _savedPosition[1];
    _s1->moveToRid(rid1);
    _s2->moveToRid(rid2);
  }
}
