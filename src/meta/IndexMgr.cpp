#include "meta/IndexMgr.h"
#include "record/TableScan.h"

namespace meta {
  IndexMgr::IndexMgr(bool isNew, TableMgr& tableMgr, StatMgr& statMgr, tx::Transaction& tx)
      : _tableMgr(tableMgr), _statMgr(statMgr) {
        if (isNew) {
          record::Schema schema;
          schema.addStringField("indexname", TableMgr::MAX_NAME);
          schema.addStringField("tablename", TableMgr::MAX_NAME);
          schema.addStringField("fieldname", TableMgr::MAX_NAME);
          _tableMgr.createTable("idxcat", schema, tx);
        }

        _layout = _tableMgr.getLayout("idxcat", tx);
  }

  void IndexMgr::createIndex(const std::string& indexName, const std::string& tableName, const std::string& fieldName, tx::Transaction& tx) {
    record::TableScan ts(tx, "idxcat", _layout);
    ts.insert();
    ts.setString("indexname", indexName);
    ts.setString("tablename", tableName);
    ts.setString("fieldname", fieldName);
    ts.close();
  }

  std::map<std::string, IndexInfo> IndexMgr::getIndexInfo(const std::string& tableName, tx::Transaction& tx) {
    std::map<std::string, IndexInfo> indexInfoMap;
    record::TableScan ts(tx, "idxcat", _layout);
    while (ts.next()) {
      if (ts.getString("tablename") == tableName) {
        std::string indexName = ts.getString("indexname");
        std::string fieldName = ts.getString("fieldname");
        record::Layout tableLayout = _tableMgr.getLayout(tableName, tx);
        StatInfo statInfo = _statMgr.getStatInfo(tableName, tableLayout, tx);
        IndexInfo indexInfo(indexName, fieldName, tableLayout.schema(), &tx, statInfo);
        indexInfoMap[fieldName] = indexInfo;
      }
    }
    ts.close();
    return indexInfoMap;
  }
}
