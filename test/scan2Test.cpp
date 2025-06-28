#include "catch.hpp"

#include <iostream>
#include <memory>
#include <string>

#include "tx/Transaction.h"
#include "record/Schema.h"
#include "record/Layout.h"
#include "record/TableScan.h"
#include "app/SampleDB.h"
#include "scan/ProductScan.h"
#include "scan/ProjectScan.h"
#include "scan/SelectScan.h"


TEST_CASE("Scan2Test Main", "[Scan2Test]") {
  std::string fileName = "scanr2Test";
  app::SampleDB db;
  db.tinySetup(fileName, 400, 8);
  auto transaction = db.newTransaction();

  // Create and populate table T1
  record::Schema sch1;
  sch1.addIntField("A");
  sch1.addStringField("B", 9);
  record::Layout layout1(sch1);
  auto s1 = std::static_pointer_cast<scan::UpdateScan>(
      std::make_shared<record::TableScan>(*transaction, "T1", layout1));
  s1->beforeFirst();

  for (int i = 0; i < 100; i++) {
    s1->insert();
    s1->setInt("A", i);
    s1->setString("B", "rec" + std::to_string(i));
  }
  s1->close();

  // Create and populate table T2
  record::Schema sch2;
  sch2.addIntField("C");
  sch2.addStringField("D", 9);
  record::Layout layout2(sch2);
  auto s2 = std::static_pointer_cast<scan::UpdateScan>(
      std::make_shared<record::TableScan>(*transaction, "T2", layout2));
  s2->beforeFirst();

  for (int i = 0; i < 100; i++) {
    int n = 100 - i - 1;
    s2->insert();
    s2->setInt("C", n);
    s2->setString("D", "rec" + std::to_string(n));
  }
  s2->close();

  // Join and filter on A = C
  auto s3 = std::static_pointer_cast<scan::Scan>(
      std::make_shared<record::TableScan>(*transaction, "T1", layout1));
  auto s4 = std::static_pointer_cast<scan::Scan>(
      std::make_shared<record::TableScan>(*transaction, "T2", layout2));
  auto s5 = std::static_pointer_cast<scan::Scan>(
      std::make_shared<scan::ProductScan>(s3, s4));

  scan::Term t(scan::Expression("A"), scan::Expression("C"));
  scan::Predicate pred(t);
  std::cout << "The predicate is " << pred.toString() << std::endl;

  auto s6 = std::static_pointer_cast<scan::Scan>(
      std::make_shared<scan::SelectScan>(s5, pred));

  std::vector<std::string> c = {"B", "D"};
  auto s7 = std::static_pointer_cast<scan::Scan>(
      std::make_shared<scan::ProjectScan>(s6, c));

  int match_count = 0;
  while (s7->next()) {
    std::string b = s7->getString("B");
    std::string d = s7->getString("D");
    std::cout << b << " " << d << std::endl;

    REQUIRE(b == d);  // since A == C and B/D are constructed as "rec{value}"
    match_count++;
  }

  s7->close();
  transaction->commit();

  REQUIRE(match_count == 100); // Expect 100 matches for A == C
}
