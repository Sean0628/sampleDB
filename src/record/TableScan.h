#pragma once

#include <string>
#include <memory>
#include "scan/UpdateScan.h"
#include "tx/Transaction.h"
#include "record/Layout.h"
#include "record/RecordPage.h"

namespace record {
  class TableScan : public scan::UpdateScan {
    public:
      ~TableScan() {};
      TableScan(tx::Transaction& tx, const std::string& tblname, const Layout& layout);
      void close() override;
      void beforeFirst() override;
      bool next() override;
      int getInt(const std::string& fieldName) override;
      bool getBool(const std::string& fieldName) override;
      std::string getString(const std::string& fieldName) override;
      scan::Constant getValue(const std::string& fieldName) override;
      bool hasField(const std::string& fieldName) override;
      void setInt(const std::string& fieldName, int val) override;
      void setBool(const std::string& fieldName, bool val);
      void setString(const std::string& fieldName, const std::string& val) override;
      void setValue(const std::string& fieldName, const scan::Constant& val) override;
      void insert() override;
      void remove() override;
      void moveToRid(const record::RID& rid) override;
      record::RID getRid() override;
    private:
      void moveToBlock(int blknum);
      void moveToNewBlock();
      bool atEndOfBlock();

      tx::Transaction& _tx;
      Layout _layout;
      std::unique_ptr<RecordPage> _recordPage;
      std::string _fileName;
      int _currentSlot;
  };
}
