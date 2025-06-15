# pragma once

#include "record/RID.h"
#include "scan/Constant.h"

namespace indexing {
  class Index {
    public:
      virtual ~Index() = default;
      virtual void beforeFirst(const scan::Constant& key) = 0;
      virtual bool next() = 0;
      virtual record::RID getDataRid() const = 0;
      virtual void insert(const scan::Constant& dataval, const record::RID& datarid) = 0;
      virtual void remove(const scan::Constant& dataval, const record::RID& datarid) = 0;
      virtual void close() = 0;
  };
}
