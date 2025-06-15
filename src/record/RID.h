#pragma once

#include <string>

namespace record {
  class RID {
    public:
      friend bool operator==(const RID& lhs, const RID& rhs);

      RID(int blknum, int slotnum) : _blknum(blknum), _slotnum(slotnum) {}
      int blockNumber() const;
      int slotNumber() const;
      bool equals(const RID& rid);
      std::string toString() const;
    private:
      int _blknum;
      int _slotnum;
  };
}
