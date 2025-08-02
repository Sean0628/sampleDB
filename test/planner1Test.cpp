#include "catch.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <filesystem>
#include <ctime>

#include "app/SampleDB.h"
#include "plan/Plan.h"
#include "plan/Planner.h"
#include "scan/Scan.h"
#include "tx/Transaction.h"

TEST_CASE("Planner executes table creation, insertion, and selection correctly", "[Planner1Test]") {
  std::string fileName = "planner1Test";
  std::filesystem::remove_all(fileName); // Clean up just in case

  app::SampleDB db(fileName);
  auto transaction = db.newTransaction();
  plan::Planner& planner = db.getPlanner();

  // Create table
  std::string cmd = "create table student ( id int, name varchar ( 12 ) )";
  REQUIRE_NOTHROW(planner.executeUpdate(cmd, transaction.get()));

  // Insert 200 rows
  for (int i = 0; i < 200; i++) {
    std::string b = "rec" + std::to_string(i % 10);
    cmd = "insert into student ( id, name ) values ( " +
      std::to_string(i) + ", '" + b + "' )";
    REQUIRE(planner.executeUpdate(cmd, transaction.get()) == 1);
  }

  // Run SELECT query for students with name = 'rec8'
  std::string qry = "select id from student where name = 'rec8'";
  auto p = planner.createQueryPlan(qry, transaction.get());
  auto s = p->open();

  int count = 0;
  while (s->next()) {
    int id = s->getInt("id");
    INFO("id: " << id);
    REQUIRE((id % 10) == 8); // All rec8s should have id % 10 == 8
    count++;
  }
  s->close();

  REQUIRE(count == 20); // Expecting 20 matches for rec8 out of 200

  transaction->commit();

  // Clean up DB files afterward
  std::filesystem::remove_all(fileName);
}

