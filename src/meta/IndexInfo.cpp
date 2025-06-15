#include "IndexInfo.h"
#include "indexing/BTreeIndex.h"

namespace meta {
  IndexInfo::IndexInfo(
    const std::string& indexName,
    const std::string& fieldName,
    const record::Schema& schema,
    tx::Transaction* tx,
    const StatInfo& statInfo
  ) : _indexName(indexName),
      _fieldName(fieldName),
      _tx(tx),
      _schema(schema),
      _statInfo(statInfo) {
    _layout = createIndexLayout();
  }

  std::shared_ptr<indexing::Index> IndexInfo::open() {
    return std::static_pointer_cast<indexing::Index>(
     std::make_shared<indexing::BTreeIndex>(*_tx, _indexName, _layout)
    );
  }

  int IndexInfo::blocksAccessed() const {
    int rpb = _tx->blockSize() / _layout.slotSize();
    int numBlocks = _statInfo.recordOutput() / rpb;
    return indexing::BTreeIndex::searchCost(numBlocks, rpb);
  }

  int IndexInfo::recordOutput() const {
    return _statInfo.recordOutput() / _statInfo.distinctValues(_fieldName);
  }

  int IndexInfo::distinctValues(const std::string& fieldName) const {
    return _fieldName == fieldName ? 1 : _statInfo.distinctValues(fieldName);
  }

  record::Layout IndexInfo::createIndexLayout() {
    record::Schema indexSchema;
    indexSchema.addIntField("block");
    indexSchema.addIntField("id");
    if (_schema.fieldType(_fieldName) == record::Schema::INTEGER) {
      indexSchema.addIntField("dataval");
    } else {
      int fieldLength = _schema.fieldLength(_fieldName);
      indexSchema.addStringField("dataval", fieldLength);
    }
    record::Layout layout(indexSchema);

    return layout;
  }

  bool IndexInfo::isNull() const {
    return _indexName.empty() || _fieldName.empty();
  }
}
