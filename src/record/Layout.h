# pragma once

#include <string>
#include <map>
#include "record/Schema.h"

namespace record {
  class Layout {
    public:
      Layout() {};
      Layout(const Schema& schema);
      Layout(const Schema& schema, std::map<std::string, int>& offsets, int slotSize);
      Layout &operator=(const Layout &layout);
      Schema schema() const;
      int offset(const std::string& fieldName) const;
      int slotSize() const;
    private:
      Schema _schema;
      std::map<std::string, int> _offsets;
      int _slotSize;
      int lengthInBytes(const std::string& fieldName) const;
  };
}
