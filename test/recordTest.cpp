#include "catch.hpp"

#include <iostream>
#include <string>
#include <filesystem>
#include "file/BlockId.h"
#include "file/FileMgr.h"
#include "logging/LogMgr.h"
#include "buffer/BufferMgr.h"
#include "tx/Transaction.h"
#include "record/RecordPage.h"
#include "record/Schema.h"
#include "record/Layout.h"

TEST_CASE("Record operations: insert, delete, iterate", "[record]") {
  std::string fileName = "recordTest";
  std::string logFileName = "simpledb.log";

  int blockSize = 400;
  auto path = std::filesystem::current_path() / fileName;

  file::FileMgr fm(path, blockSize);
  logging::LogMgr lm(fm, logFileName);
  buffer::BufferMgr bm(fm, lm, 16);

  tx::Transaction tx1(fm, lm, bm);
  record::Schema sch;
  sch.addIntField("A");
  sch.addStringField("B", 9);
  record::Layout layout(sch);

  SECTION("Field offsets should be set correctly") {
    for (const auto& fieldName : layout.schema().fieldNames()) {
      int offset = layout.offset(fieldName);
      std::cout << fieldName << " has offset " << offset << std::endl;
      REQUIRE(offset >= 0);
    }
  }

  file::BlockId blk = tx1.append("recordfile");
  tx1.pin(blk);
  record::RecordPage rp(tx1, blk, layout);
  rp.format();

  SECTION("Insert records and delete those with A < 25") {
    std::cout << "Filling the page with random records." << std::endl;
    int slot = rp.insertAfter(-1);
    int i = 0;

    while (slot >= 0) {
      int n = 23 + i;
      rp.setInt(slot, "A", n);
      rp.setString(slot, "B", "rec" + std::to_string(n));
      std::cout << "inserting into slot " << slot << ": {" << n << ", rec" << n << "}" << std::endl;
      slot = rp.insertAfter(slot);
      i++;
    }

    std::cout << "Deleted these records with A-values < 25." << std::endl;
    int count = 0;
    slot = rp.nextAfter(-1);
    while (slot >= 0) {
      int a = rp.getInt(slot, "A");
      std::string b = rp.getString(slot, "B");
      if (a < 25) {
        count++;
        std::cout << "slot " << slot << ": {" << a << ", " << b << "}" << std::endl;
        rp.remove(slot);
      }
      slot = rp.nextAfter(slot);
    }

    std::cout << count << " values under 25 were deleted" << std::endl;
    REQUIRE(count > 0);

    std::cout << "Here are the remaining records." << std::endl;
    slot = rp.nextAfter(-1);
    while (slot >= 0) {
      int a = rp.getInt(slot, "A");
      std::string b = rp.getString(slot, "B");
      std::cout << "slot " << slot << ": {" << a << ", " << b << "}" << std::endl;
      REQUIRE(a >= 25);
      slot = rp.nextAfter(slot);
    }
  }

  tx1.unpin(blk);
  tx1.commit();
}
