# pragma once

#include <string>
#include "scan/Constant.h"

namespace scan {
  class Scan {
    public:
      virtual ~Scan() = default;
      virtual void beforeFirst() = 0;
      virtual bool next() = 0;
      virtual int getInt(const std::string& fieldName) = 0;
      virtual std::string getString(const std::string& fieldName) = 0;
      virtual Constant getValue(const std::string& fieldName) = 0;
      virtual bool hasField(const std::string& fieldName) = 0;
      virtual void close() = 0;
  };
}
