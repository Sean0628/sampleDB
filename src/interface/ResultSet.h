#pragma once

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <memory>
#include <string>

#include "Connection.h"
#include "MetaData.h"
#include "plan/Plan.h"
#include "scan/Scan.h"
#include "record/Schema.h"

namespace interface {
  class Connection;

  class ResultSet {
    public:
      ResultSet(plan::Plan* plan, Connection* connection);
      bool next();
      int getInt(std::string& fieldName) const;
      std::string getString(std::string& fieldName) const;
      MetaData getMetaData() const;
      void close();
    private:
      std::shared_ptr<scan::Scan> _scan;
      record::Schema _schema;
      Connection* _connection;
  };
}
