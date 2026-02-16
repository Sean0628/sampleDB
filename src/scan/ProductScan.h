#pragma once

#include <string>
#include <memory>
#include <vector>

#include "Constant.h"
#include "Scan.h"
#include "Predicate.h"

namespace scan {
  class ProductScan : public Scan {
    public:
      ProductScan(std::shared_ptr<Scan> scan1, std::shared_ptr<Scan> scan2);
      void beforeFirst() override;
      bool next() override;
      int getInt(const std::string& fieldName) override;
      bool getBool(const std::string& fieldName) override;
      std::string getString(const std::string& fieldName) override;
      Constant getValue(const std::string& fieldName) override;
      bool hasField(const std::string& fieldName) override;
      void close() override;
    private:
      std::shared_ptr<Scan> _scan1;
      std::shared_ptr<Scan> _scan2;
  };
}
