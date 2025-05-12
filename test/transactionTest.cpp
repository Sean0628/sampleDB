#include "catch.hpp"

#include "file/BlockId.h"
#include "file/Page.h"
#include "file/FileMgr.h"
#include "logging/LogMgr.h"
#include "buffer/BufferMgr.h"
#include "tx/Transaction.h"

TEST_CASE("Transaction operations work correctly", "[Transaction]") {
  std::string fileName = "txTest";
  std::string logFileName = "simpledb.log";

  int blockSize = 400;
  auto path = std::filesystem::current_path() / fileName;

  file::FileMgr fm(path, blockSize);
  logging::LogMgr lm(fm, logFileName);
  buffer::BufferMgr bm(fm, lm, 8);

  file::BlockId blk("testfile", 1);

  SECTION("Transaction end-to-end behavior") {
    tx::Transaction tx1(fm, lm, bm);
    tx1.pin(blk);
    tx1.setInt(blk, 80, 1, false);
    tx1.setString(blk, 40, "one", false);
    tx1.commit();

    tx::Transaction tx2(fm, lm, bm);
    tx2.pin(blk);
    int ival = tx2.getInt(blk, 80);
    std::string sval = tx2.getString(blk, 40);

    REQUIRE(ival == 1);
    REQUIRE(sval == "one");

    int newival = ival + 1;
    std::string newsval = sval + "!";
    tx2.setInt(blk, 80, newival, true);
    tx2.setString(blk, 40, newsval, true);
    tx2.commit();

    tx::Transaction tx3(fm, lm, bm);
    tx3.pin(blk);
    ival = tx3.getInt(blk, 80);
    sval = tx3.getString(blk, 40);
    REQUIRE(ival == 2);
    REQUIRE(sval == "one!");

    tx3.setInt(blk, 80, 9999, true);
    REQUIRE(tx3.getInt(blk, 80) == 9999);
    tx3.rollback();

    tx::Transaction tx4(fm, lm, bm);
    tx4.pin(blk);
    ival = tx4.getInt(blk, 80);
    REQUIRE(ival == 2);
    tx4.commit();
  }
}
