#include "meta/StatInfo.h"

namespace meta {
  StatInfo::StatInfo() {};

  StatInfo::StatInfo(int numBlocks, int numRecords)
    : _numBlocks(numBlocks), _numRecords(numRecords) {}

  int StatInfo::blocksAccessed() const {
    return _numBlocks;
  }

  int StatInfo::recordOutput() const {
    return _numRecords;
  }

  int StatInfo::distinctValues(const std::string& fieldName) const {
    return 1 + (_numRecords / 3); // This is wilidly inaccurate.
  }
}
