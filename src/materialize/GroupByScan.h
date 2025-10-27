#pragma once

#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>

#include "AggregationFn.h"
#include "GroupValue.h"
#include "scan/Constant.h"
#include "scan/Scan.h"

namespace materialize {
  class GroupByScan : public scan::Scan {
    public:
      GroupByScan(const std::shared_ptr<scan::Scan>& src,
          const std::vector<std::string>& groupFields,
          const std::vector<std::shared_ptr<AggregationFn>>& aggFns);
      void beforeFirst() override;
      bool next() override;
      void close() override;
      scan::Constant getValue(const std::string& fieldName) override;
      int getInt(const std::string& fieldName) override;
      std::string getString(const std::string& fieldName) override;
      bool hasField(const std::string& fieldName) override;
    private:
      std::shared_ptr<scan::Scan> _src;
      std::vector<std::string> _groupFields;
      std::vector<std::shared_ptr<AggregationFn>> _aggFns;
      std::unique_ptr<GroupValue> _currentGroup;
      bool _moreGroups;
  };
}
