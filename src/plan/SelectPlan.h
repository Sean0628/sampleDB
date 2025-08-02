#pragma once

#include <string>
#include <memory>
#include <algorithm>

#include "Plan.h"
#include "scan/SelectScan.h"
#include "scan/Predicate.h"

namespace plan {
  class SelectPlan : public Plan {
    public:
      SelectPlan(const std::shared_ptr<Plan>& p, const scan::Predicate& pred);
      std::shared_ptr<scan::Scan> open() override;
      int blocksAccessed() const override;
      int recordsOutput() const override;
      int distinctValues(const std::string& fieldName) const override;
      record::Schema schema() const override;
    private:
      std::shared_ptr<Plan> _plan;
      scan::Predicate _predicate;
  };
}
