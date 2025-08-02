#pragma once

#include <string>
#include <memory>
#include <vector>

#include "Plan.h"
#include "scan/ProductScan.h"
#include "record/Schema.h"

namespace plan {
  class ProductPlan : public Plan {
    public:
      ProductPlan(const std::shared_ptr<Plan>& p1, const std::shared_ptr<Plan>& p2);
      std::shared_ptr<scan::Scan> open() override;
      int blocksAccessed() const override;
      int recordsOutput() const override;
      int distinctValues(const std::string& fieldName) const override;
      record::Schema schema() const override;
    private:
      std::shared_ptr<Plan> _plan1;
      std::shared_ptr<Plan> _plan2;
      record::Schema _schema;
  };
}
