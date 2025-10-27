#include "SumFn.h"

namespace materialize {
  SumFn::SumFn(const std::string& fieldName) : _fieldName(fieldName) {}

  void SumFn::processFirst(scan::Scan* s) {
    _value = s->getValue(_fieldName);
  }

  void SumFn::processNext(scan::Scan* s) {
    scan::Constant newValue = s->getValue(_fieldName);
    _value += newValue;
  }

  std::string SumFn::fieldName() const {
    return parse::Word::SUM + _fieldName;
  }

  scan::Constant SumFn::value() const {
    return _value;
  }
}
