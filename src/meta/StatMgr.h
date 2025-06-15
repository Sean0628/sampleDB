# pragma once

#include <string>
#include <map>
#include <mutex>

#include "meta/StatInfo.h"
#include "meta/TableMgr.h"
#include "tx/Transaction.h"

namespace meta {
  class StatMgr {
    public:
      StatMgr(TableMgr& tm, tx::Transaction& tx);
      StatInfo getStatInfo(const std::string& tableName, const record::Layout& layout, tx::Transaction& tx);
      void refreshStatistics(tx::Transaction& tx);
      StatInfo calcTableStats(const std::string& tableName, const record::Layout& layout, tx::Transaction& tx);
    private:
      TableMgr& _tableMgr;
      std::map<std::string, StatInfo> _tableStats;
      int _numCalls;
      std::recursive_mutex _mutex;
  };
}
