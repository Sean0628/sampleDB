#include "file/BlockId.h"

using namespace std;

namespace file {
  BlockId::BlockId(string filename, int blknum) {
    _filename = filename;
    _blknum = blknum;
  }

  bool operator==(const file::BlockId& lhs, const file::BlockId& rhs) {
    return lhs.fileName() == rhs.fileName() && lhs.number() == rhs.number();
  }

  bool operator<(const file::BlockId& lhs, const file::BlockId& rhs) {
    return lhs.fileName() == rhs.fileName() ? lhs.number() < rhs.number() : lhs.fileName() < rhs.fileName();
  }

  string BlockId::fileName() const{
    return _filename;
  }

  int BlockId::number() const {
    return _blknum;
  }

  bool BlockId::equals(const BlockId& bi) {
    return _filename.compare(bi._filename) == 0 && (_blknum == bi._blknum);
  }

  string BlockId::toString() const {
    return "[file " + _filename + ", block " + to_string(_blknum) + "]";
  }

  int BlockId::hashCode() {
    return hash<string>{}(toString());
  }
}
