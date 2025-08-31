#pragma once

#include <string>

#include "Connection.h"
#include "interface/ResultSet.h"
#include "plan/Planner.h"

namespace interface {
  class Connection;

  class ResultSet;

  class Statement {
    public:
      Statement(Connection* connection, plan::Planner& planner);
      ResultSet executeQuery(const std::string& query);
      int executeUpdate(const std::string& cmd);
      void close();
    private:
      Connection* _connection;
      plan::Planner& _planner;
  };
}
