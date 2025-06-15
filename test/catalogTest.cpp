#include "catch.hpp"

#include <iostream>
#include <string>

#include "file/BlockId.h"
#include "file/FileMgr.h"
#include "logging/LogMgr.h"
#include "buffer/BufferMgr.h"
#include "tx/Transaction.h"
#include "record/RecordPage.h"
#include "record/Schema.h"
#include "record/Layout.h"
#include "record/TableScan.h"
#include "meta/TableMgr.h"

namespace {
  const std::string kDirName = "catalogTest";
  const std::string kLogFileName = "simpledb.log";

  // Global pointers for shared environment
  std::unique_ptr<file::FileMgr> g_fm;
  std::unique_ptr<logging::LogMgr> g_lm;
  std::unique_ptr<buffer::BufferMgr> g_bm;

  // Run once per process to prepare environment
  void initSharedEnvironment() {
    if (!g_fm) {
      const std::filesystem::path path = std::filesystem::current_path() / kDirName;

      // Clean up leftover test files
      std::filesystem::remove_all(path);
      std::filesystem::create_directory(path);

      g_fm = std::make_unique<file::FileMgr>(path, 400);
      g_lm = std::make_unique<logging::LogMgr>(*g_fm, kLogFileName);
      g_bm = std::make_unique<buffer::BufferMgr>(*g_fm, *g_lm, 8);

      // Setup schema and create "mytable"
      tx::Transaction tx(*g_fm, *g_lm, *g_bm);
      meta::TableMgr tm(true, tx);

      record::Schema sch;
      sch.addIntField("A");
      sch.addStringField("B", 9);
      tm.createTable("mytable", sch, tx);

      tx.commit();
    }
  }

  // Utility to get Transaction + Layout
  std::tuple<std::unique_ptr<tx::Transaction>, record::Layout> getLayoutWithTransaction(const std::string& tableName) {
    auto txPtr = std::make_unique<tx::Transaction>(*g_fm, *g_lm, *g_bm);
    meta::TableMgr tm(false, *txPtr);
    record::Layout layout = tm.getLayout(tableName, *txPtr);
    return {std::move(txPtr), layout};
  }

  TEST_CASE("TableManager populates tblcat and fldcat correctly", "[Catalog]") {
    initSharedEnvironment();

    SECTION("tblcat should contain mytable entry") {
      auto [txPtr, layout] = getLayoutWithTransaction("tblcat");
      record::TableScan ts(*txPtr, "tblcat", layout);

      bool found = false;
      while (ts.next()) {
        std::string tname = ts.getString("tblname");
        int size = ts.getInt("slotsize");
        std::cout << tname << " " << size << std::endl;
        if (tname == "mytable") {
          found = true;
          REQUIRE(size > 0); // sanity check
        }
      }
      txPtr->commit();
      REQUIRE(found);
    }

    SECTION("fldcat should contain fields for mytable") {
      auto [txPtr, layout] = getLayoutWithTransaction("fldcat");
      record::TableScan ts(*txPtr, "fldcat", layout);

      int fieldCount = 0;
      while (ts.next()) {
        std::string tname = ts.getString("tblname");
        std::string fname = ts.getString("fldname");
        int offset = ts.getInt("offset");

        if (tname == "mytable") {
          std::cout << tname << " " << fname << " " << offset << std::endl;
          REQUIRE(offset >= 0);
          ++fieldCount;
        }
      }
      txPtr->commit();

      REQUIRE(fieldCount == 2); // fields A and B
    }
  }
}
