#include "catch.hpp"

#include <iostream>
#include <memory>
#include <string>

#include "tx/Transaction.h"
#include "record/Schema.h"
#include "record/Layout.h"
#include "record/TableScan.h"
#include "app/SampleDB.h"
#include "scan/ProjectScan.h"
#include "scan/SelectScan.h"

TEST_CASE("Scan1Test Main", "[Scan1Test]") {
  std::string fileName = "scan1Test";
  app::SampleDB db;
  db.tinySetup(fileName, 400, 8);
  auto transaction = db.newTransaction();

  record::Schema sch;
  sch.addIntField("A");
  sch.addStringField("B", 9);
  record::Layout layout(sch);
  auto s1 = std::static_pointer_cast<scan::UpdateScan>(
      std::make_shared<record::TableScan>(*transaction, "T", layout));
  s1->beforeFirst();

  for (int i = 0; i < 100; i++) {
    s1->insert();
    int n = i;
    s1->setInt("A", n);
    s1->setString("B", "rec" + std::to_string(n));
  }
  s1->close();

  auto s2 = std::static_pointer_cast<scan::Scan>(
      std::make_shared<record::TableScan>(*transaction, "T", layout));

  int target = 10;
  scan::Constant c(target);
  scan::Term t(scan::Expression("A"), scan::Expression(c));
  scan::Predicate pred(t);

  std::cout << "The predicate is " << pred.toString() << std::endl;

  auto s3 = std::static_pointer_cast<scan::Scan>(
      std::make_shared<scan::SelectScan>(s2, pred));
  std::vector<std::string> fields = {"B"};
  auto s4 = std::static_pointer_cast<scan::Scan>(
      std::make_shared<scan::ProjectScan>(s3, fields));

  int count = 0;
  std::string result;
  while (s4->next()) {
    result = s4->getString("B");
    std::cout << result << std::endl;
    count++;
  }

  s4->close();
  transaction->commit();

  REQUIRE(count == 1);
  REQUIRE(result == "rec10");
}
