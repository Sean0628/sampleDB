#include "MinFn.h"

namespace materialize {
  MinFn::MinFn(const std::string& fieldName) : _fieldName(fieldName) {}

  void MinFn::processFirst(scan::Scan* s) {
    _value = s->getValue(_fieldName);
  }

  void MinFn::processNext(scan::Scan* s) {
    scan::Constant newValue = s->getValue(_fieldName);
    if (newValue < _value) {
      _value = newValue;
    }
  }

  std::string MinFn::fieldName() const {
    return parse::Word::MIN + _fieldName;
  }

  scan::Constant MinFn::value() const {
    return _value;
  }
}
