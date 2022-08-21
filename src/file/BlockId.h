#pragma once

#include <string>

using namespace std;

namespace file {
  class BlockId {
    public:
      BlockId(string filename, int blknum);
      string fileName();
      int number();
      bool equals(const BlockId& bi);
      string toString();
      int hashCode();
    private:
      string _filename;
      int _blknum;
  };
}
