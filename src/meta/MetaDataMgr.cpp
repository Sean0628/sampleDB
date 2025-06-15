#include "meta/MetaDataMgr.h"

namespace meta {
  std::unique_ptr<TableMgr> MetaDataMgr::_tableMgr;
  std::unique_ptr<ViewMgr> MetaDataMgr::_viewMgr;
  std::unique_ptr<StatMgr> MetaDataMgr::_statMgr;
  std::unique_ptr<IndexMgr> MetaDataMgr::_indexMgr;

  MetaDataMgr::MetaDataMgr(bool isNew, tx::Transaction& tx) {
    _tableMgr = std::make_unique<TableMgr>(isNew, tx);
    _viewMgr = std::make_unique<ViewMgr>(isNew, *_tableMgr, tx);
    _statMgr = std::make_unique<StatMgr>(*_tableMgr, tx);
    _indexMgr = std::make_unique<IndexMgr>(isNew, *_tableMgr, *_statMgr, tx);
  }

  void MetaDataMgr::createTable(const std::string& tableName, const record::Schema& schema, tx::Transaction& tx) {
    _tableMgr->createTable(tableName, schema, tx);
  }

  record::Layout MetaDataMgr::getLayout(const std::string& tableName, tx::Transaction& tx) {
    return _tableMgr->getLayout(tableName, tx);
  }

  void MetaDataMgr::createView(const std::string& viewName, const std::string& viewDef, tx::Transaction& tx) {
    _viewMgr->createView(viewName, viewDef, tx);
  }

  std::string MetaDataMgr::getViewDef(const std::string& viewName, tx::Transaction& tx) {
    return _viewMgr->getViewDef(viewName, tx);
  }

  void MetaDataMgr::createIndex(const std::string& indexName, const std::string& tableName, const std::string& fieldName, tx::Transaction& tx) {
    _indexMgr->createIndex(indexName, tableName, fieldName, tx);
  }

  std::map<std::string, IndexInfo> MetaDataMgr::getIndexInfo(const std::string& tableName, tx::Transaction& tx) {
    return _indexMgr->getIndexInfo(tableName, tx);
  }

  StatInfo MetaDataMgr::getStatInfo(const std::string& tableName, const record::Layout& layout, tx::Transaction& tx) {
    return _statMgr->getStatInfo(tableName, layout, tx);
  }
}
