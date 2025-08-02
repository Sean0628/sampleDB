#include "IndexUpdatePlanner.h"

namespace indexing {
    IndexUpdatePlanner::IndexUpdatePlanner(meta::MetaDataMgr* metaDataMgr)
        : _metaDataMgr(metaDataMgr) {
    }

    int IndexUpdatePlanner::executeDelete(const parse::DeleteData* data, tx::Transaction* tx) {
      std::string tableName = data->tableName();
      auto plan = std::static_pointer_cast<plan::Plan>(
          std::make_shared<plan::TablePlan>(tx, tableName, _metaDataMgr));
      plan = std::static_pointer_cast<plan::Plan>(
          std::make_shared<plan::SelectPlan>(plan, data->predicate()));
      std::map<std::string, meta::IndexInfo> indexes = _metaDataMgr->getIndexInfo(tableName, *tx);
      auto updateScan = std::static_pointer_cast<scan::UpdateScan>(plan->open());
      int count = 0;

      while (updateScan->next()) {
        record::RID rid = updateScan->getRid();
        for (const auto& [fieldName, index] : indexes) {
          scan::Constant value = updateScan->getValue(fieldName);
          auto idx = indexes[fieldName].open();
          idx->remove(value, rid);
          idx->close();
        }

        updateScan->remove();
        count++;
      }

      updateScan->close();
      return count;
    }

    int IndexUpdatePlanner::executeModify(const parse::ModifyData* data, tx::Transaction* tx) {
      std::string tableName = data->tableName();
      std::string fieldName = data->targetField();
      auto plan = std::static_pointer_cast<plan::Plan>(
          std::make_shared<plan::TablePlan>(tx, tableName, _metaDataMgr));
      plan = std::static_pointer_cast<plan::Plan>(
          std::make_shared<plan::SelectPlan>(plan, data->predicate()));
      meta::IndexInfo indexInfo = _metaDataMgr->getIndexInfo(tableName, *tx)[fieldName];
      auto idx = (indexInfo.isNull()) ? nullptr : indexInfo.open();
      auto updateScan = std::static_pointer_cast<scan::UpdateScan>(plan->open());
      int count = 0;

      while (updateScan->next()) {
        scan::Constant newValue = data->newValue().evaluate(updateScan.get());
        scan::Constant oldValue = updateScan->getValue(fieldName);
        updateScan->setValue(data->targetField(), newValue);

        if (idx) {
          record::RID rid = updateScan->getRid();
          idx->remove(oldValue, rid);
          idx->insert(newValue, rid);
        }

        count++;
      }

      if (idx) idx->close();
      updateScan->close();
      return count;
    }

    int IndexUpdatePlanner::executeInsert(const parse::InsertData* data, tx::Transaction* tx) {
      std::string tableName = data->tableName();
      auto plan = std::static_pointer_cast<plan::Plan>(
          std::make_shared<plan::TablePlan>(tx, tableName, _metaDataMgr));
      auto updateScan = std::static_pointer_cast<scan::UpdateScan>(plan->open());
      updateScan->insert();
      record::RID rid = updateScan->getRid();
      
      std::map<std::string, meta::IndexInfo> indexes = _metaDataMgr->getIndexInfo(tableName, *tx);
      auto values = data->values();
      int i = 0;
      for (const auto& field : data->fieldNames()) {
        scan::Constant value = values[i];
        updateScan->setValue(field, value);

        meta::IndexInfo indexInfo = indexes[field];
        if (!indexInfo.isNull()) {
          auto idx = indexInfo.open();
          idx->insert(value, rid);
          idx->close();
        }
        i++;
      }

      updateScan->close();
      return 1;
    }

    int IndexUpdatePlanner::executeCreateTable(const parse::CreateTableData* data, tx::Transaction* tx) {
      _metaDataMgr->createTable(data->tableName(), data->newSchema(), *tx);
      return 0; // Table creation does not return a count
    }

    int IndexUpdatePlanner::executeCreateView(const parse::CreateViewData* data, tx::Transaction* tx) {
      _metaDataMgr->createView(data->viewName(), data->viewDefinition(), *tx);
      return 0; // View creation does not return a count
    }

    int IndexUpdatePlanner::executeCreateIndex(const parse::CreateIndexData* data, tx::Transaction* tx) {
      _metaDataMgr->createIndex(data->indexName(), data->tableName(), data->fieldName(), *tx);
      return 0; // Index creation does not return a count
    }
}
