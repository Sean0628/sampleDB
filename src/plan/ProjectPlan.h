#pragma once

#include <string>
#include <memory>
#include <vector>

#include "Plan.h"
#include "scan/ProjectScan.h"
#include "record/Schema.h"

namespace plan {
  class ProjectPlan : public Plan {
    public:
      ProjectPlan(const std::shared_ptr<Plan>& p, const std::vector<std::string>& fields);
      std::shared_ptr<scan::Scan> open() override;
      int blocksAccessed() const override;
      int recordsOutput() const override;
      int distinctValues(const std::string& fieldName) const override;
      record::Schema schema() const override;
    private:
      std::shared_ptr<Plan> _plan;
      record::Schema _schema;
  };
}
