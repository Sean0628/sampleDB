#pragma once

#include <memory>
#include <string>

#include "SortScan.h"
#include "scan/Constant.h"
#include "scan/Scan.h"

namespace materialize {
  class MergeJoinScan : public scan::Scan {
    public:
      MergeJoinScan(const std::shared_ptr<scan::Scan>& s1,
          const std::shared_ptr<SortScan>& s2,
          const std::string& fld1,
          const std::string& fld2);
      void close() override;
      void beforeFirst() override;
      bool next() override;
      int getInt(const std::string& fieldName) override;
      bool getBool(const std::string& fieldName) override;
      std::string getString(const std::string& fieldName) override;
      scan::Constant getValue(const std::string& fieldName) override;
      bool hasField(const std::string& fieldName) override;
    private:
      std::shared_ptr<scan::Scan> _s1;
      std::shared_ptr<SortScan> _s2;
      std::string _fld1, _fld2;
      scan::Constant _joinValue;
  };
}
