#include "record/RID.h"

namespace record {
  bool operator==(const RID& lhs, const RID& rhs) {
    return lhs.blockNumber() == rhs.blockNumber() && lhs.slotNumber() == rhs.slotNumber();
  }

  int RID::blockNumber() const {
    return _blknum;
  }

  int RID::slotNumber() const {
    return _slotnum;
  }

  bool RID::equals(const RID& rid) {
    return _blknum == rid._blknum && _slotnum == rid._slotnum;
  }

  std::string RID::toString() const {
    return "[" + std::to_string(_blknum) + ", " + std::to_string(_slotnum) + "]";
  }
}
