#pragma once

#include <string>
#include <memory>

#include "scan/Scan.h"
#include "record/Schema.h"

namespace plan {
  class Plan {
    public:
      virtual ~Plan() = default;
      virtual std::shared_ptr<scan::Scan> open() = 0;
      virtual int blockAccess(const std::string& fieldName) const = 0;
      virtual int recordsOutput() const = 0;
      virtual int distinctValues(const std::string& fieldName) const = 0;
      virtual record::Schema schema() const = 0;
  };
}
