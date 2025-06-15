#include "StatMgr.h"
#include "record/TableScan.h"

namespace meta {
  StatMgr::StatMgr(TableMgr& tm, tx::Transaction& tx) : _tableMgr(tm) {
      refreshStatistics(tx);
  }

  StatInfo StatMgr::getStatInfo(const std::string& tableName, const record::Layout& layout, tx::Transaction& tx) {
    std::unique_lock<std::recursive_mutex> lock(_mutex);
    _numCalls++;

    if (_numCalls > 100) refreshStatistics(tx);

    auto it = _tableStats.find(tableName);
    if (it != _tableStats.end()) {
      StatInfo info = calcTableStats(tableName, layout, tx);
      _tableStats[tableName] = info;
      return info;
    }

    StatInfo info = _tableStats[tableName];
    return info;
  }

  void StatMgr::refreshStatistics(tx::Transaction& tx) {
    std::unique_lock<std::recursive_mutex> lock(_mutex);
    _tableStats.clear();
    _numCalls = 0;
    record::Layout layout = _tableMgr.getLayout("tblcat", tx);
    record::TableScan ts(tx, "tblcat", layout);

    while (ts.next()) {
      std::string tableName = ts.getString("tblname");
      record::Layout layout = _tableMgr.getLayout(tableName, tx);
      StatInfo info = calcTableStats(tableName, layout, tx);
      _tableStats[tableName] = info;
    }

    ts.close();
  }

  StatInfo StatMgr::calcTableStats(const std::string& tableName, const record::Layout& layout, tx::Transaction& tx) {
    std::unique_lock<std::recursive_mutex> lock(_mutex);
    int numBlocks = 0;
    int numRecords = 0;

    record::TableScan ts(tx, tableName, layout);
    while (ts.next()) {
      numRecords++;
      numBlocks = ts.getRid().blockNumber() +1;
    }
    ts.close();

    return StatInfo(numBlocks, numRecords);
  }
}
