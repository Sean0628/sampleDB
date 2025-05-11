#include "tx/LogRecord/LogRecord.h"
#include "tx/LogRecord/RollbackRecord.h"
#include "tx/LogRecord/SetIntRecord.h"
#include "tx/LogRecord/SetStringRecord.h"
#include "tx/LogRecord/CommitRecord.h"
#include "tx/LogRecord/StartRecord.h"
#include "tx/LogRecord/CheckPointRecord.h"

std::shared_ptr<tx::LogRecord> tx::LogRecord::createLogRecord(const std::vector<char>& bytes) {
  auto shared_bytes = std::make_shared<std::vector<char>>(bytes);
  file::Page page(shared_bytes);

  switch (page.getInt(0)) {
    case tx::LogRecord::CHECKPOINT:
      return std::make_shared<tx::CheckpointRecord>();
    case tx::LogRecord::START:
      return std::make_shared<tx::StartRecord>(&page);
    case tx::LogRecord::COMMIT:
      return std::make_shared<tx::CommitRecord>(&page);
    case tx::LogRecord::ROLLBACK:
      return std::make_shared<tx::RollbackRecord>(&page);
    case tx::LogRecord::SETINT:
      return std::make_shared<tx::SetIntRecord>(&page);
    case tx::LogRecord::SETSTRING:
      return std::make_shared<tx::SetStringRecord>(&page);
    default:
      return std::unique_ptr<tx::LogRecord>(nullptr);
  }
}
