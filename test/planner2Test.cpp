#include "catch.hpp"

#include <string>

#include "app/SampleDB.h"
#include "plan/Plan.h"
#include "plan/Planner.h"
#include "scan/Scan.h"
#include "tx/Transaction.h"

TEST_CASE("Planner executes join query correctly", "[Planner2Test]") {
  std::string fileName = "planner2Test";
  std::filesystem::remove_all(fileName); // Clean up just in case

  app::SampleDB db(fileName);
  auto transaction = db.newTransaction();
  plan::Planner& planner = db.getPlanner();

  // Create table t1
  std::string cmd = "create table t1 ( a int, b varchar ( 12 ) )";
  REQUIRE_NOTHROW(planner.executeUpdate(cmd, transaction.get()));

  // Insert rows into t1
  for (int i = 0; i < 200; i++) {
    std::string b = "t1-" + std::to_string(i);
    cmd = "insert into t1 ( a, b ) values ( " +
      std::to_string(i) + ", '" + b + "' )";
    REQUIRE(planner.executeUpdate(cmd, transaction.get()) == 1);
  }

  // Create table t2
  cmd = "create table t2 ( c int, d varchar ( 12 ) )";
  REQUIRE_NOTHROW(planner.executeUpdate(cmd, transaction.get()));

  // Insert rows into t2
  for (int i = 0; i < 200; i++) {
    std::string b = "t2-" + std::to_string(200 - i - 1);
    cmd = "insert into t2 ( c, d ) values ( " +
      std::to_string(200 - i - 1) + ", '" + b + "' )";
    REQUIRE(planner.executeUpdate(cmd, transaction.get()) == 1);
  }

  // Run join query
  std::string qry = "select b, d from t1, t2 where a = c";
  auto p = planner.createQueryPlan(qry, transaction.get());
  auto s = p->open();

  int count = 0;
  while (s->next()) {
    auto bVal = s->getString("b");
    auto dVal = s->getString("d");

    // Extract numeric part and verify they match
    int bNum = std::stoi(bVal.substr(3));
    int dNum = std::stoi(dVal.substr(3));
    REQUIRE(bNum == dNum);

    count++;
  }
  s->close();

  // Expect exactly 200 matches (since both tables have matching keys 0..199)
  REQUIRE(count == 200);

  transaction->commit();
}
