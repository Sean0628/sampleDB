#pragma once

#include <string>

#include "AggregationFn.h"
#include "parse/Word.h"
#include "scan/Constant.h"
#include "scan/Scan.h"

namespace materialize {
  class CountFn : public AggregationFn {
    public:
      CountFn();
      void processFirst(scan::Scan* s) override;
      void processNext(scan::Scan* s) override;
      std::string fieldName() const override;
      scan::Constant value() const override;
    private:
      scan::Constant _value;
      int _count;
  };
}
