#include "file/BlockId.h"

namespace file {
  BlockId::BlockId(std::string filename, int blknum) {
    _filename = filename;
    _blknum = blknum;
  }

  bool operator==(const file::BlockId& lhs, const file::BlockId& rhs) {
    return lhs.fileName() == rhs.fileName() && lhs.number() == rhs.number();
  }

  bool operator<(const file::BlockId& lhs, const file::BlockId& rhs) {
    return lhs.fileName() == rhs.fileName() ? lhs.number() < rhs.number() : lhs.fileName() < rhs.fileName();
  }

  std::string BlockId::fileName() const{
    return _filename;
  }

  int BlockId::number() const {
    return _blknum;
  }

  bool BlockId::equals(const BlockId& bi) {
    return _filename.compare(bi._filename) == 0 && (_blknum == bi._blknum);
  }

  bool BlockId::isNull() const {
    return _filename.empty();
  }

  std::string BlockId::toString() const {
    return "[file " + _filename + ", block " + std::to_string(_blknum) + "]";
  }

  int BlockId::hashCode() {
    return std::hash<std::string>{}(toString());
  }
}
