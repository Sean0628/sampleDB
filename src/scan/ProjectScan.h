#pragma once

#include <string>
#include <memory>
#include <vector>

#include "Constant.h"
#include "UpdateScan.h"
#include "Scan.h"
#include "Predicate.h"

namespace scan {
  class ProjectScan : public Scan {
    public:
      ProjectScan(std::shared_ptr<Scan>& scan, const std::vector<std::string>& fields);
      void beforeFirst() override;
      bool next() override;
      int getInt(const std::string& fieldName) override;
      std::string getString(const std::string& fieldName) override;
      Constant getValue(const std::string& fieldName) override;
      bool hasField(const std::string& fieldName) override;
      void close() override;
    private:
      std::shared_ptr<Scan> _scan;
      std::vector<std::string> _fields;
  };
}
