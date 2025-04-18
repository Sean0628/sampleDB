#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <string>
#include "app/SampleDB.h"
#include "file/BlockId.h"
#include "file/FileMgr.h"
#include "file/Page.h"

std::pair<std::string, int> writeThenReadBack(
  const std::string& dbName,
  const std::string& fileName,
  int blockNum,
  int pos,
  const std::string& valueStr,
  int valueInt
) {
  app::SampleDB db(dbName, 400, 8);
  file::FileMgr& fm = db.fileMgr();
  file::BlockId blk(fileName, blockNum);

  file::Page p1(fm.blockSize());
  p1.setString(pos, valueStr);

  int pos2 = pos + file::Page::maxLength(valueStr.size());
  p1.setInt(pos2, valueInt);

  fm.write(blk, p1);

  file::Page p2(fm.blockSize());
  fm.read(blk, p2);

  return {p2.getString(pos), p2.getInt(pos2)};
}

TEST_CASE("String and int values can be written and read back") {
  std::string str = "abcdefghijklm";
  int number = 345;
  int pos = 88;

  auto [readStr, readInt] = writeThenReadBack("filetest", "testfile", 2, pos, str, number);

  REQUIRE(readStr == str);
  REQUIRE(readInt == number);
}
