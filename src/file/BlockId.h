#pragma once

#include <string>

using namespace std;

namespace file {
  class BlockId {
    public:
      friend bool operator==(const BlockId& lhs, const BlockId& rhs);
      friend bool operator<(const BlockId& lhs, const BlockId& rhs);

      BlockId() {};
      BlockId(string filename, int blknum);
      string fileName() const;
      int number() const;
      bool equals(const BlockId& bi);
      string toString() const;
      int hashCode();
    private:
      string _filename;
      int _blknum;
  };
}
