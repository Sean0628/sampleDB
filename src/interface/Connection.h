#pragma once

#include <memory>
#include <string>

#include "app/SampleDB.h"
#include "plan/Planner.h"
#include "interface/Statement.h"
#include "tx/Transaction.h"

namespace interface {
  class Statement;

  class Connection {
    public:
      Connection(std::unique_ptr<app::SampleDB> db);
      std::unique_ptr<Statement> createStatement();
      void close();
      void commit();
      void rollback();
      tx::Transaction& getTransaction();
    private:
      std::unique_ptr<app::SampleDB> _db;
      std::unique_ptr<tx::Transaction> _transaction;
      plan::Planner& _planner;
  };
}
