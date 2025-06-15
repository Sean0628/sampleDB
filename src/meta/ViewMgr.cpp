#include "ViewMgr.h"
#include "record/TableScan.h"

namespace meta {
  ViewMgr::ViewMgr(bool isNew, TableMgr& tm, tx::Transaction& tx)
      : _tableMgr(tm) {
      if (isNew) {
          record::Schema viewSchema;
          viewSchema.addStringField("viewname", TableMgr::MAX_NAME);
          viewSchema.addStringField("viewdef", MAX_VIEW_DEF);
          tm.createTable("viewcat", viewSchema, tx);
      }
  }

  void ViewMgr::createView(const std::string& viewName, const std::string& vdef, tx::Transaction& tx) {
    record::Layout layout = _tableMgr.getLayout("viewcat", tx);
    record::TableScan ts(tx, "viewcat", layout);

    ts.insert();
    ts.setString("viewname", viewName);
    ts.setString("viewdef", vdef);
    ts.close();
  }

  std::string ViewMgr::getViewDef(const std::string& viewName, tx::Transaction& tx) const {
    std::string result;
    record::Layout layout = _tableMgr.getLayout("viewcat", tx);
    record::TableScan ts(tx, "viewcat", layout);

    while (ts.next()) {
      if (ts.getString("viewname") == viewName) {
        result = ts.getString("viewdef");
        break;
      }
    }

    ts.close();
    return result;
  }
}
