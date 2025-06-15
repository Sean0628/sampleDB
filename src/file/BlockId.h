#pragma once

#include <string>

namespace file {
  class BlockId {
    public:
      friend bool operator==(const BlockId& lhs, const BlockId& rhs);
      friend bool operator<(const BlockId& lhs, const BlockId& rhs);

      BlockId() {};
      BlockId(std::string filename, int blknum);
      std::string fileName() const;
      int number() const;
      bool equals(const BlockId& bi);
      bool isNull() const;
      std::string toString() const;
      int hashCode();
    private:
      std::string _filename;
      int _blknum;
  };
}
