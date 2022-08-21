#include "file/BlockId.h"

using namespace file;
using namespace std;

BlockId::BlockId(string filename, int blknum) {
  _filename = filename;
  _blknum = blknum;
}

string BlockId::fileName() {
  return _filename;
}

int BlockId::number() {
  return _blknum;
}

bool BlockId::equals(const BlockId& bi) {
  return _filename.compare(bi._filename) == 0 && (_blknum == bi._blknum);
}

string BlockId::toString() {
  return "[file " + _filename + ", block " + to_string(_blknum) + "]";
}

int BlockId::hashCode() {
  return hash<string>{}(toString());
}
