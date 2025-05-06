#include "catch.hpp"

#include <iostream>
#include <filesystem>

#include "file/BlockId.h"
#include "file/Page.h"
#include "file/FileMgr.h"
#include "file/LogMgr.h"
#include "file/Buffer.h"
#include "file/BufferMgr.h"

TEST_CASE("Buffer Manager Integration Test", "[BufferTest]") {
  std::string file_name = "bufferTest";
  std::string log_file_name = "simpledb.log";

  int block_size = 400;
  auto path = std::filesystem::current_path() / file_name;
  int buffer_size = 3;

  file::FileMgr fm(file_name, block_size);
  file::LogMgr lm(fm, log_file_name);
  file::BufferMgr bm(fm, lm, buffer_size);

  SECTION("Basic Buffer Pinning and Flushing") {
    file::BlockId block_id1("testfile", 1);
    file::Buffer* buff1 = bm.pin(block_id1);

    REQUIRE(buff1 != nullptr);

    auto page = buff1->contents();
    int n = page->getInt(80);
    page->setInt(80, n + 1); // this modification will get written to disk
    buff1->setModified(1, 0);

    bm.unpin(*buff1);

    file::BlockId block_id2("testfile", 2);
    file::BlockId block_id3("testfile", 3);
    file::BlockId block_id4("testfile", 4);

    // One of these pins will force buff1 to flush to disk
    file::Buffer* buff2 = bm.pin(block_id2);
    file::Buffer* buff3 = bm.pin(block_id3);
    file::Buffer* buff4 = bm.pin(block_id4);

    REQUIRE(buff2 != nullptr);
    REQUIRE(buff3 != nullptr);
    REQUIRE(buff4 != nullptr);

    bm.unpin(*buff2);

    // Re-pin block_id1 to see the flushed value
    block_id1 = file::BlockId("testfile", 1);
    buff2 = bm.pin(block_id1);
    REQUIRE(buff2 != nullptr);

    auto page2 = buff2->contents();
    page2->setInt(80, 9999); // modify again
    buff2->setModified(1, 0);

    bm.unpin(*buff2);
  }
}
