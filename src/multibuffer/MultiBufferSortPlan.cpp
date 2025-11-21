#include "MultiBufferSortPlan.h"

namespace multibuffer {
  MultiBufferSortPlan::MultiBufferSortPlan(const std::shared_ptr<plan::Plan>& p, const std::vector<std::string>& sortFields, tx::Transaction* tx)
    : _tx(tx), _p(p), _comparator(sortFields) {
      _schema = p->schema();
  }

  std::shared_ptr<scan::Scan> MultiBufferSortPlan::open() {
    int size = blocksAccessed();
    int avail = _tx->availableBuffers();
    int numBuffersForMerge = BufferNeeds::bestRoot(avail, size); 

    auto runs = splitIntoBlockWise(numBuffersForMerge);
    for (const auto& run : runs) {
      sortTempTable(run);
    }

    while (runs.size() > 1) {
      runs = doMergeIteration(runs, numBuffersForMerge );
    }

    auto dest = runs[0]->open();
    return std::static_pointer_cast<scan::Scan>(dest);
  }

  int MultiBufferSortPlan::blocksAccessed() {
    auto materializePlan = std::static_pointer_cast<plan::Plan>(std::make_shared<materialize::MaterializePlan>(_tx, _p));
    return materializePlan->blocksAccessed();
  }

  int MultiBufferSortPlan::recordsOutput() {
    return _p->recordsOutput();
  }

  int MultiBufferSortPlan::distinctValues(const std::string& fldname) {
    return _p->distinctValues(fldname);
  }

  record::Schema MultiBufferSortPlan::schema() {
    return _schema;
  }

  std::vector<std::shared_ptr<materialize::TempTable>> MultiBufferSortPlan::splitIntoBlockWise(int runSize) {
    std::vector<std::shared_ptr<materialize::TempTable>> temps;

    auto originTempTable = std::make_shared<materialize::TempTable>(_tx, _schema);
    auto src = _p->open();
    auto originScan = originTempTable->open();
    while (src->next()) {
      originScan->insert();
      for (const auto& fldname : _schema.fields()) {
        originScan->setVal(fldname, src->getVal(fldname));
      }
    }

    src->close();
    originScan->beforeFirst();

    auto currentTempTable = std::make_shared<materialize::TempTable>(_tx, _schema);
    temps.push_back(currentTempTable);
    auto currentScan = currentTempTable->open();
    int prevBlkNum = -1;

    while (originScan->next()) {
      currentScan->insert();
      for (const auto& fldname : _schema.fields()) {
        currentScan->setVal(fldname, originScan->getVal(fldname));
      }

      int currentBlkNum = originScan->getCurrentBlock().number();
      if (prevBlkNum != -1 && currentBlkNum == prevBlkNum + runSize) {
        currentScan->close();
        currentTempTable = std::make_shared<materialize::TempTable>(_tx, _schema);
        temps.push_back(currentTempTable);
        currentScan = currentTempTable->open();
      }

      prevBlkNum = currentBlkNum;
    }
    currentScan->close();
    return temps;
  }

  void MultiBufferSortPlan::sortTempTable(const std::shared_ptr<materialize::TempTable>& tt) {
    auto exscan = tt->open();
    auto inscan1 = tt->open();
    auto inscan2 = tt->open();

    while (exscan->next()) {
      record::RID minValRID = exscan->getRid();
      inscan1->moveToRid(minValRID);

      while (inscan1->next()) {
        inscan2->moveToRid(minValRID);

        if (_comparator.compare(inscan1.get(), inscan2.get()) < 0) {
          minValRID = inscan1->getRid();
        }
      }

      inscan2->moveToRid(minValRID);
      for (const auto& fldname : _schema.fields()) {
        scan::Constant temp = exscan->getVal(fldname);
        exscan->setVal(fldname, inscan2->getVal(fldname));
        inscan2->setVal(fldname, temp);
      }

      inscan1->beforeFirst();
    }

    exscan->close();
    inscan1->close();
    inscan2->close();
  }

  std::vector<std::shared_ptr<materialize::TempTable>> MultiBufferSortPlan::doMergeIteration(const std::vector<std::shared_ptr<materialize::TempTable>>& tables, int runsPerMerge) {
    std::vector<std::shared_ptr<materialize::TempTable>> resultTables;
    int i = 0;

    for (i = 0; i + runsPerMerge <= tables.size(); i += runsPerMerge) {
      std::vector<std::shared_ptr<materialize::TempTable>> mergeGroup;
      for (int j = 0; j < runsPerMerge; j++) {
        mergeGroup.push_back(tables[i + j]);
      }

      resultTables.push_back(mergeSeveralRuns(mergeGroup));
    }

    while (i < tables.size()) {
      resultTables.push_back(tables[i]);
      i++;
    }

    return resultTables;
  }

  std::shared_ptr<materialize::TempTable> MultiBufferSortPlan::mergeSeveralRuns(const std::vector<materialize::TempTable>>& temps) {
    auto resultTempTable = std::make_shared<materialize::TempTable>(_tx, _schema);
    auto dest = resultTempTable->open();

    std::vector<std::shared_ptr<scan::UpdateScan>> scans;
    std::vector<bool> hasMore;

    int i = 0;
    for (const auto& temp : temps) {
      scans.push_back(temp->open());
      hasMore.push_back(scans[i]->next());
      i++;
    }

    while (true) {
      std::vector<std::shared_ptr<scan::Scan>> currentScans;
      std::vector<int> currentIndices;

      for (int i = 0; i < hasMore.size(); i++) {
        if (hasMore[i]) {
          currentScans.push_back(std::static_pointer_cast<scan::Scan>(scans[i]));
          currentIndices.push_back(i);
        }
      }

      if (currentScans.empty()) break;

      int minIndex = _comparator.minIndex(currentScans);
      hasMore[currentIndices[minIndex]] = copy(*scans[currentIndices[minIndex]], *dest);
    }

    for (const auto& scan : scans) {
      scan->close();
    }
    
    return resultTempTable;
  }

  bool MultiBufferSortPlan::copy(scan::Scan& src, scan::UpdateScan& dest) {
    dest->insert();

    for (const auto& fldname : _schema.fields()) {
      dest->setVal(fldname, src->getVal(fldname));
    }

    return src->next();
  }
}
