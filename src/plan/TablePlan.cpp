#include "TablePlan.h"

namespace plan {
  TablePlan::TablePlan(tx::Transaction* tx, const std::string& tableName, meta::MetaDataMgr* metaDataMgr)
      : _tx(tx), _tableName(tableName) {
    _layout = metaDataMgr->getLayout(tableName, *_tx);
    _statInfo = metaDataMgr->getStatInfo(_tableName, _layout, *_tx);
  }

  std::shared_ptr<scan::Scan> TablePlan::open() {
    auto tableScan = std::make_shared<record::TableScan>(*_tx, _tableName, _layout);
    return std::static_pointer_cast<scan::Scan>(tableScan);
  }

  int TablePlan::blocksAccessed() const {
    return _statInfo.blocksAccessed();
  }

  int TablePlan::recordsOutput() const {
    return _statInfo.recordsOutput();
  }

  int TablePlan::distinctValues(const std::string& fieldName) const {
    return _statInfo.distinctValues(fieldName);
  }

  record::Schema TablePlan::schema() const {
    return _layout.schema();
  }
}
