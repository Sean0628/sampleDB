#include <cmath>
#include <climits>

#include "BTreeIndex.h"
#include "BTreeDir.h"

namespace indexing {
  BTreeIndex::BTreeIndex(tx::Transaction& tx, const std::string& indexName, const record::Layout& leafLayout)
    : _tx(tx), _leafLayout(leafLayout) {
    // deal with the leaves
    _leafTable = indexName + "leaf";

    if (_tx.size(_leafTable) == 0) {
      file::BlockId blk = _tx.append(_leafTable);
      BTreePage node(&_tx, blk, _leafLayout);
      node.format(blk, -1);
    }

    // deal with the directory
    record::Schema dirSchema;
    dirSchema.add("block", _leafLayout.schema());
    dirSchema.add("dataval", _leafLayout.schema());
    std::string dirTable = indexName + "dir";
    _dirLayout = record::Layout(dirSchema);
    _rootBlk = file::BlockId(dirTable, 0);
    if (_tx.size(dirTable) == 0) {
      // create new root block
      _tx.append(dirTable);
      BTreePage node(&_tx, _rootBlk, _dirLayout);
      node.format(_rootBlk, 0);

      // insert initial directory entry
      int fieldType = dirSchema.fieldType("dataval");
      scan::Constant minVal = (fieldType == record::Schema::INTEGER) ? scan::Constant(INT_MIN) : scan::Constant("");
      node.insertDir(0, minVal, 0);
      node.close();
    }
  }

  void BTreeIndex::beforeFirst(const scan::Constant& key) {
    close();
    BTreeDir root(_tx, _rootBlk, _dirLayout);
    int blkNum = root.search(key);
    root.close();
    file::BlockId leafBlk(_leafTable, blkNum);
    _leaf = BTreeLeaf(&_tx, leafBlk, _leafLayout, key);
  }

  bool BTreeIndex::next() {
    return _leaf.next();
  }

  record::RID BTreeIndex::getDataRid() const {
    return _leaf.getDataRid();
  }

  void BTreeIndex::insert(const scan::Constant& dataval, const record::RID& datarid) {
    beforeFirst(dataval);
    DirEntry dirEntry = _leaf.insert(datarid);
    _leaf.close();
    if (dirEntry.isNull()) return;

    BTreeDir root(_tx, _rootBlk, _dirLayout);
    DirEntry newEntry = root.insert(dirEntry);
    if (!newEntry.isNull()) root.makeNewRoot(newEntry);

    root.close();
  }

  void BTreeIndex::remove(const scan::Constant& dataval, const record::RID& datarid) {
    beforeFirst(dataval);
    _leaf.remove(datarid);
    _leaf.close();
  }

  void BTreeIndex::close() {
    if (!_leaf.isNull()) _leaf.close();
  }

  int BTreeIndex::searchCost(int numBlocks, int rpb) {
    return 1 + std::round(log(numBlocks) / log(rpb));
  }
}
