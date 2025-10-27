#pragma once

#include <string>

#include "scan/Constant.h"
#include "scan/Scan.h"

namespace materialize {
  class AggregationFn {
    public:
      virtual ~AggregationFn() = default;
      virtual void processFirst(scan::Scan* s) = 0;
      virtual void processNext(scan::Scan* s) = 0;
      virtual std::string fieldName() const = 0;
      virtual scan::Constant value() const = 0;
  };
}
