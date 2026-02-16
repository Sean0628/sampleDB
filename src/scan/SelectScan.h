#pragma once

#include <string>
#include <memory>

#include "scan/Constant.h"
#include "scan/UpdateScan.h"
#include "scan/Scan.h"
#include "scan/Predicate.h"
#include "record/RID.h"

namespace scan {
  class SelectScan : public UpdateScan {
    public:
      SelectScan(const std::shared_ptr<Scan>& scan, const Predicate& pred);
      void beforeFirst() override;
      bool next() override;
      int getInt(const std::string& fieldName) override;
      bool getBool(const std::string& fieldName) override;
      std::string getString(const std::string& fieldName) override;
      Constant getValue(const std::string& fieldName) override;
      bool hasField(const std::string& fieldName) override;
      void close() override;
      void setInt(const std::string& fieldName, int val) override;
      void setString(const std::string& fieldName, const std::string& val) override;
      void setValue(const std::string& fieldName, const Constant& val) override;
      void insert() override;
      void remove() override;
      void moveToRid(const record::RID& rid) override;
      record::RID getRid() override;
    private:
      std::shared_ptr<Scan> _scan;
      Predicate _predicate;
  };
}

