#pragma once

#include <string>
#include "AggregationFn.h"
#include "parse/Word.h"
#include "scan/Constant.h"
#include "scan/Scan.h"

namespace materialize {
  class SumFn : public AggregationFn {
    public:
      SumFn(const std::string& fieldName);
      void processFirst(scan::Scan* s) override;
      void processNext(scan::Scan* s) override;
      std::string fieldName() const override;
      scan::Constant value() const override;
    private:
      std::string _fieldName;
      scan::Constant _value;
  };
}
