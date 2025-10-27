#include "CountFn.h"

namespace materialize {
  CountFn::CountFn() : _value(0), _count(0) {}

  void CountFn::processFirst(scan::Scan* s) {
    _count = 1;
  }

  void CountFn::processNext(scan::Scan* s) {
    _count++;
  }

  std::string CountFn::fieldName() const {
    return parse::Word::COUNT;
  }

  scan::Constant CountFn::value() const {
    scan::Constant _value(_count);
    return _value;
  }
}
