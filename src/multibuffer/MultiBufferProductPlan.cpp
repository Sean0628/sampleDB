#include "MultiBufferProductPlan.h"

namespace multibuffer {
  MultiBufferProductPlan::MultiBufferProductPlan(const std::shared_ptr<plan::Plan>& p1,
                                                 const std::shared_ptr<plan::Plan>& p2,
                                                 tx::Transaction* tx)
    : _tx(tx), _p1(p1), _p2(p2) {
      _schema.addAll(p1->schema());
      _schema.addAll(p2->schema());
  }

  std::shared_ptr<scan::Scan>> MultiBufferProductPlan::open() {
    auto scan1 = _p1->open();
    auto t = copyRecordFrom(_p2);
    return std::static_pointer_cast<scan::Scan>(std::make_shared<MultiBufferProductScan>(scan1, t->tableName(), t->layout(), _tx));
  }

  int MultiBufferProductPlan::blocksAccessed() {
    int avail = _tx->availableBuffers();
    auto materializePlan = std::make_shared<materialize::MaterializePlan>(_tx, _p2);
    int size = materializePlan->blocksAccessed();
    int numChunks = size / avail;

    return (numChunks * _p1->blocksAccessed()) + _p2->blocksAccessed();
  }

  int MultiBufferProductPlan::recordsOutput() {
    return _p1->recordsOutput() * _p2->recordsOutput();
  }

  int MultiBufferProductPlan::distinctValues(const std::string& fldname) {
    if (_p1->schema().hasField(fldname)) {
      return _p1->distinctValues(fldname);
    } else {
      return _p2->distinctValues(fldname);
    }
  }

  record::Schema MultiBufferProductPlan::schema() {
    return _schema;
  }

  std::shared_ptr<materialize::TempTable> MultiBufferProductPlan::copyRecordFrom(const std::shared_ptr<plan::Plan>& p) {
    auto src = p->open();
    record::Schema sch = p->schema();
    auto tt = std::make_shared<materialize::TempTable>(_tx, sch);
    auto dest = tt->open();
    while (src->next()) {
      dest->insert();
      for (const auto& fldname : sch.fields()) {
        dest->setVal(fldname, src->getVal(fldname));
      }
    }

    src->close();
    dest->close();
    return tt;
  }
}
