# pragma once

#include <string>

#include "meta/TableMgr.h"
#include "tx/Transaction.h"

namespace meta {
  class ViewMgr {
    public:
      ViewMgr(bool isNew, TableMgr& tm, tx::Transaction& tx);
      void createView(const std::string& viewName, const std::string& vdef, tx::Transaction& tx);
      std::string getViewDef(const std::string& viewName, tx::Transaction& tx) const;
    private:
      const static int MAX_VIEW_DEF = 100; // max length of view definition
      TableMgr& _tableMgr;
  };
}
