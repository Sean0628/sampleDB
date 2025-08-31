#include "interface/Connection.h"

namespace interface {
  Connection::Connection(std::unique_ptr<app::SampleDB> db)
      : _db(std::move(db)), _transaction(_db->newTransaction()), _planner(_db->getPlanner()) {}

  std::unique_ptr<Statement> Connection::createStatement() {
    try {
      auto ptr = std::make_unique<Statement>(this, _planner);
      return ptr;
    } catch (const std::exception& e) {
      throw std::runtime_error("Failed to create statement: " + std::string(e.what()));
    }
  }

  void Connection::close() {
    try {
      commit();
    } catch (const std::exception& e) {
      throw std::runtime_error("Failed to close connection: " + std::string(e.what()));
    }
  }

  void Connection::commit() {
    try {
      _transaction->commit();
      _transaction = _db->newTransaction();
    } catch (const std::exception& e) {
      throw std::runtime_error("Failed to commit transaction: " + std::string(e.what()));
    }
  }

  void Connection::rollback() {
    try {
      _transaction->rollback();
      _transaction = _db->newTransaction();
    } catch (const std::exception& e) {
      throw std::runtime_error("Failed to rollback transaction: " + std::string(e.what()));
    }
  }

  tx::Transaction& Connection::getTransaction() {
    return *_transaction;
  }
}
