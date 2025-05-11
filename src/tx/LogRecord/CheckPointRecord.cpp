#include <vector>
#include "tx/LogRecord/CheckpointRecord.h"

namespace tx {
  CheckpointRecord::CheckpointRecord() : LogRecord() { }

  int CheckpointRecord::writeToLog(file::LogMgr& lm) {
    auto bytes = std::make_shared<std::vector<char>>(sizeof(int), 0);
    auto page = file::Page(bytes);
    page.setInt(0, CHECKPOINT);
    return lm.append(*bytes);
  }
}
