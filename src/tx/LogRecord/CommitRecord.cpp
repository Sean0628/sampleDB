#include <vector>
#include "tx/LogRecord/CommitRecord.h"

namespace tx {
  CommitRecord::CommitRecord(file::Page* p) {
    int tpos = sizeof(int);
    _txNum = p->getInt(tpos);
  }

  int CommitRecord::writeToLog(file::LogMgr& lm, int txNum) {
    int tpos = sizeof(int);
    int recSize = tpos + sizeof(int);
    auto bytes = std::make_shared<std::vector<char>>(recSize, 0);
    auto page = file::Page(bytes);
    page.setInt(0, COMMIT);
    page.setInt(tpos, txNum);
    return lm.append(*bytes);
  }
}
