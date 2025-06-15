# pragma once

#include <string>

namespace meta {
  class StatInfo {
    public:
      StatInfo();
      StatInfo(int numBlocks, int numRecords);
      int blocksAccessed() const;
      int recordOutput() const;
      int distinctValues(const std::string& fieldName) const;
    private:
      int _numBlocks;
      int _numRecords;
  };
}
