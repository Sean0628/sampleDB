#include "catch.hpp"

#include <iostream>
#include <filesystem>
#include <vector>

#include "file/BlockId.h"
#include "file/Page.h"
#include "file/FileMgr.h"
#include "logging/LogMgr.h"
#include "buffer/Buffer.h"
#include "buffer/BufferMgr.h"

TEST_CASE("Buffer Manager Basic Behaviour Test", "[BufferManagerTest]") {
  std::string file_name = "bufferMgrTest";
  std::string log_file_name = "simpledb.log";

  int block_size = 400;
  auto path = std::filesystem::current_path() / file_name;
  int buffer_size = 3;

  file::FileMgr fm(file_name, block_size);
  logging::LogMgr lm(fm, log_file_name);
  buffer::BufferMgr bm(fm, lm, buffer_size);

  std::vector<buffer::Buffer*> buff(6);
  file::BlockId block_id0("testfile", 0);
  file::BlockId block_id1("testfile", 1);
  file::BlockId block_id2("testfile", 2);

  buff[0] = bm.pin(block_id0);
  buff[1] = bm.pin(block_id1);
  buff[2] = bm.pin(block_id2);

  bm.unpin(*buff[1]);
  buff[1] = nullptr;

  block_id0 = file::BlockId("testfile", 0);
  block_id1 = file::BlockId("testfile", 1);
  buff[3] = bm.pin(block_id0);
  buff[4] = bm.pin(block_id1);

  REQUIRE(bm.available() == 0);

  file::BlockId block_id3("testfile", 3);

  SECTION("Attempting to pin block 3 when no buffers available should throw") {
    try {
      std::cout << "Attempting to pin block 3..." << std::endl;
      buff[5] = bm.pin(block_id3);
      FAIL("Expected exception due to no available buffers, but pin succeeded!");
    } catch (const std::exception& e) {
      std::cout << "Exception: No Available buffers" << std::endl;
      SUCCEED("Caught expected exception");
    }
  }

  bm.unpin(*buff[2]);
  buff[2] = nullptr;

  buff[5] = bm.pin(block_id3);

  REQUIRE(buff[0]->block().number() == 0);
  REQUIRE(buff[3]->block().number() == 0);
  REQUIRE(buff[4]->block().number() == 1);
  REQUIRE(buff[5]->block().number() == 3);
}
