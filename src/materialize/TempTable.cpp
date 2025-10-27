#include "TempTable.h"

namespace materialize {
  int TempTable::_nextTableNum = 0;
  std::mutex TempTable::_mtx;

  TempTable::TempTable(tx::Transaction* tx, const record::Schema& schema) : _tx(tx) {
    _tableName = nextTableName();
    _layout = record::Layout(schema);
  }

  std::shared_ptr<scan::UpdateScan> TempTable::open() {
    return std::static_pointer_cast<scan::UpdateScan>(std::make_shared<record::TableScan>(*_tx, _tableName, _layout));
  }

  std::string TempTable::tableName() const {
    return _tableName;
  }

  record::Layout TempTable::getLayout() const {
    return _layout;
  }

  std::string TempTable::nextTableName() {
    std::unique_lock<std::mutex> lock(_mtx);
    _nextTableNum++;
    return "temp" + std::to_string(_nextTableNum);
  }
}
