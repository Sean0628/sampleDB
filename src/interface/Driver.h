#pragma once

#include <memory>
#include <string>

#include "app/SampleDB.h"
#include "interface/Connection.h"

namespace interface {
  class Driver {
    public:
      std::unique_ptr<Connection> connect(const std::string& dbName);
  };
}
