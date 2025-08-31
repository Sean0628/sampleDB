#include "interface/Driver.h"

namespace interface {
  std::unique_ptr<Connection> Driver::connect(const std::string& dbName) {
    auto db = std::make_unique<app::SampleDB>(dbName);
    auto connection = std::make_unique<Connection>(std::move(db));

    return connection;
  }
}
