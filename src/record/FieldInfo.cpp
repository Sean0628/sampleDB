#include "record/FieldInfo.h"

namespace record {
  int FieldInfo::type() const {
    return _type;
  }

  int FieldInfo::length() const {
    return _length;
  }
}
