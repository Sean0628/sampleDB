# pragma once

#include <string>
#include "record/RID.h"
#include "scan/Scan.h"

namespace scan {
  class UpdateScan : public Scan {
    public:
      virtual ~UpdateScan() = default;

      virtual void setInt(const std::string& fieldName, int val) = 0;
      virtual void setString(const std::string& fieldName, const std::string& val) = 0;
      virtual void setValue(const std::string& fieldName, const Constant& val) = 0;
      virtual void insert() = 0;
      virtual void remove() = 0;
      virtual void moveToRid(const record::RID& rid) = 0;
      virtual record::RID getRid() = 0;
  };
}
