#include "MaxFn.h"

namespace materialize {
  MaxFn::MaxFn(const std::string& fieldName) : _fieldName(fieldName) {}

  void MaxFn::processFirst(scan::Scan* s) {
    _value = s->getValue(_fieldName);
  }

  void MaxFn::processNext(scan::Scan* s) {
    scan::Constant newValue = s->getValue(_fieldName);
    if (newValue > _value) {
      _value = newValue;
    }
  }

  std::string MaxFn::fieldName() const {
    return parse::Word::MAX + _fieldName;
  }

  scan::Constant MaxFn::value() const {
    return _value;
  }
}
