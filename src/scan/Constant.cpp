#include "scan/Constant.h"

namespace scan {
  bool operator==(const Constant& lhs, const Constant& rhs) {
    return lhs._ival ? *lhs._ival == *rhs._ival : *lhs._sval == *rhs._sval;
  }

  bool operator!=(const Constant& lhs, const Constant& rhs) {
    return !(lhs == rhs);
  }

  bool operator<(const Constant& lhs, const Constant& rhs) {
    return lhs._ival ? *lhs._ival < *rhs._ival : *lhs._sval < *rhs._sval;
  }

  bool operator>(const Constant& lhs, const Constant& rhs) {
    return lhs._ival ? *lhs._ival > *rhs._ival : *lhs._sval > *rhs._sval;
  }

  bool operator<=(const Constant& lhs, const Constant& rhs) {
    return !(lhs > rhs);
  }

  bool operator>=(const Constant& lhs, const Constant& rhs) {
    return !(lhs < rhs);
  }

  void operator+=(const Constant& lhs, const Constant& rhs) {
    if (lhs._ival) *lhs._ival += *rhs._ival;
  }

  Constant::Constant(const Constant& other) {
    if (other._ival) {
      _ival = std::make_unique<int>(*other._ival);
    } else if (other._sval) {
      _sval = std::make_unique<std::string>(*other._sval);
    }
  }

  Constant::Constant(int ival) {
    _ival = std::make_unique<int>(ival);
  }

  Constant::Constant(const std::string& sval) {
    _sval = std::make_unique<std::string>(sval);
  }

  Constant& Constant::operator=(const Constant& other) {
    if (this != &other) {
      if (other._ival) {
        _ival = std::make_unique<int>(*other._ival);
      } else if (other._sval) {
        _sval = std::make_unique<std::string>(*other._sval);
      }
    }
    return *this;
  }

  int Constant::asInt() const {
    return *_ival;
  }

  std::string Constant::asString() const {
    return *_sval;
  }

  int Constant::hashCode() const {
    return _ival ? std::hash<int>()(*_ival) : std::hash<std::string>()(*_sval);
  }

  std::string Constant::toString() const {
    return _ival ? std::to_string(*_ival) : *_sval;
  }

  bool Constant::isNull() const {
    return _ival == nullptr && _sval == nullptr;
  }
}
