#pragma once

#include <string>
#include <string>

#include "record/Schema.h"

namespace interface {
  class MetaData {
    public:
      MetaData(const record::Schema& schema);
      int getColumnCount() const;
      std::string getColumnName(int column) const;
      int getColumnType(int column) const;
      int getColumnDisplaySize(int column) const;
    private:
      record::Schema _schema;
  };
}
