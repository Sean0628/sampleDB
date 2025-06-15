#include "catch.hpp"

#include <iostream>
#include <string>
#include <filesystem>
#include <memory>

#include "file/BlockId.h"
#include "file/FileMgr.h"
#include "logging/LogMgr.h"
#include "buffer/BufferMgr.h"
#include "tx/Transaction.h"
#include "record/Schema.h"
#include "record/Layout.h"
#include "meta/TableMgr.h"

namespace {
  const std::string kDirName = "tableMgrTest";
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

      // Setup schema and create "MyTable"
      tx::Transaction tx(*g_fm, *g_lm, *g_bm);
      meta::TableMgr tm(true, tx);

      record::Schema sch;
      sch.addIntField("A");
      sch.addStringField("B", 9);
      tm.createTable("MyTable", sch, tx);

      tx.commit();
    }
  }

  // Utility to get layout of "MyTable"
  record::Layout getMyTableLayout() {
    tx::Transaction tx(*g_fm, *g_lm, *g_bm);
    meta::TableMgr tm(false, tx); // Don't re-create catalog
    return tm.getLayout("MyTable", tx);
  }

  TEST_CASE("TableMgr creates and retrieves correct slot size", "[TableMgr]") {
    initSharedEnvironment();

    record::Layout layout = getMyTableLayout();
    int slotSize = layout.slotSize();
    std::cout << "MyTable has slot size " << slotSize << std::endl;
    REQUIRE(slotSize > 0);
  }

  TEST_CASE("TableMgr retrieves correct schema and field types", "[TableMgr]") {
    initSharedEnvironment();

    record::Layout layout = getMyTableLayout();
    record::Schema sch2 = layout.schema();
    auto fields = sch2.fieldNames();

    REQUIRE(fields.size() == 2);
    REQUIRE(std::find(fields.begin(), fields.end(), "A") != fields.end());
    REQUIRE(std::find(fields.begin(), fields.end(), "B") != fields.end());

    REQUIRE(sch2.fieldType("A") == record::Schema::INTEGER);
    REQUIRE(sch2.fieldType("B") == record::Schema::VARCHAR);
    REQUIRE(sch2.fieldLength("B") == 9);
  }
}
