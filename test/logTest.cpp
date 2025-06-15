#include "catch.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <filesystem>
#include "app/SampleDB.h"
#include "file/BlockId.h"
#include "file/Page.h"
#include "file/FileMgr.h"
#include "logging/LogMgr.h"

void createRecords(logging::LogMgr& lm, int start, int end) {
  for (int i = start; i <= end; i++) {
    std::string s = "record" + std::to_string(i);

    int npos = file::Page::maxLength(s.size());

    auto rec = std::make_shared<std::vector<char>>(npos + sizeof(int), 0);
    file::Page page(rec);
    page.setString(0, s);
    page.setInt(npos, i + 100);

    int lsn = lm.append(*rec);
  }
}

TEST_CASE("LogMgr Integration Test", "[LogMgr]") {
  std::string fileName = "logTest";
  std::filesystem::remove_all(fileName);  // Clean start

  app::SampleDB db;
  db.tinySetup(fileName, 400, 8);

  logging::LogMgr& lm = db.logMgr();

  SECTION("Flush only happens when requested") {
    createRecords(lm, 1, 35);

    REQUIRE(lm.getLastSavedLSN() < 35);

    // Manually flush everything
    lm.flush(35);
    REQUIRE(lm.getLastSavedLSN() == 35);

    createRecords(lm, 36, 70);

    // LSN should now be 70, but nothing flushed yet
    REQUIRE(lm.getLastSavedLSN() < 70);

    // Flush up to LSN 65
    lm.flush(65);
    REQUIRE(lm.getLastSavedLSN() == 70);
  }
}
