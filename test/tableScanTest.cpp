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

TEST_CASE("TableScan inserts, deletes, and scans records correctly", "[TableScan]") {
  std::string fileName = "tablescanTest";
  std::string logFileName = "simpledb.log";

  int blockSize = 400;
  auto path = std::filesystem::current_path() / fileName;

  file::FileMgr fm(path, blockSize);
  logging::LogMgr lm(fm, logFileName);
  buffer::BufferMgr bm(fm, lm, 8);

  tx::Transaction tx1(fm, lm, bm);

  record::Schema sch;
  sch.addIntField("A");
  sch.addStringField("B", 9);
  record::Layout layout(sch);

  SECTION("Verify schema offsets are correctly calculated") {
    for (const auto& fldname : layout.schema().fieldNames()) {
      int offset = layout.offset(fldname);
      std::cout << fldname << " has offset " << offset << std::endl;
      REQUIRE(offset >= 0);
    }
  }

  record::TableScan ts(tx1, "T", layout);
  std::cout << "Filling the table with 50 random records" << std::endl;

  ts.beforeFirst();
  for (int i = 0; i < 50; i++) {
    ts.insert();
    ts.setInt("A", i);
    ts.setString("B", "rec" + std::to_string(i));
    std::cout << "inserting into slot " << ts.getRid().toString()
              << ": {" << i << ", rec" << i << "}" << std::endl;
  }

  std::cout << "Deleted these records with A-values < 25." << std::endl;
  int count = 0;
  ts.beforeFirst();

  while (ts.next()) {
    int a = ts.getInt("A");
    std::string b = ts.getString("B");
    if (a < 25) {
      count++;
      std::cout << "slot " << ts.getRid().toString()
                << ": {" << a << ", " << b << "}" << std::endl;
      ts.remove();
    }
  }

  REQUIRE(count == 25);
  std::cout << count << " values under 25 were deleted" << std::endl;

  std::cout << "Here are the remaining records." << std::endl;
  ts.beforeFirst();
  int remaining = 0;

  while (ts.next()) {
    int a = ts.getInt("A");
    std::string b = ts.getString("B");
    std::cout << "slot " << ts.getRid().toString() << ": {" << a << ", " << b << "}" << std::endl;
    REQUIRE(a >= 25); // Ensure only records with A >= 25 remain
    remaining++;
  }

  REQUIRE(remaining == 25); // 50 - 25 = 25 remaining records

  ts.close();
  tx1.commit();
}

TEST_CASE("TableScan handles BOOL fields correctly", "[TableScan]") {
  std::string fileName = "tableScanBoolTest";
  std::string logFileName = "simpledb.log";

  int blockSize = 400;
  auto path = std::filesystem::current_path() / fileName;

  file::FileMgr fm(path, blockSize);
  logging::LogMgr lm(fm, logFileName);
  buffer::BufferMgr bm(fm, lm, 8);

  tx::Transaction tx1(fm, lm, bm);

  record::Schema sch;
  sch.addIntField("id");
  sch.addBoolField("active");
  record::Layout layout(sch);

  record::TableScan ts(tx1, "BoolT", layout);

  ts.beforeFirst();
  for (int i = 0; i < 10; i++) {
    ts.insert();
    ts.setInt("id", i);
    ts.setBool("active", i % 2 == 0); // even ids are active
  }

  SECTION("getBool returns correct values") {
    ts.beforeFirst();
    while (ts.next()) {
      int id = ts.getInt("id");
      bool active = ts.getBool("active");
      REQUIRE(active == (id % 2 == 0));
    }
  }

  SECTION("getValue wraps BOOL as int Constant") {
    ts.beforeFirst();
    while (ts.next()) {
      int id = ts.getInt("id");
      scan::Constant val = ts.getValue("active");
      REQUIRE(val.asInt() == (id % 2 == 0 ? 1 : 0));
    }
  }

  ts.close();
  tx1.commit();
}
