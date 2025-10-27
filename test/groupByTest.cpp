#include "catch.hpp"
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>

#include "app/SampleDB.h"
#include "interface/Connection.h"
#include "interface/Driver.h"
#include "interface/Statement.h"
#include "interface/ResultSet.h"
#include "parse/Parser.h"
#include "plan/Plan.h"
#include "plan/TablePlan.h"
#include "plan/Planner.h"
#include "scan/Scan.h"
#include "tx/Transaction.h"

namespace {
  void createDB(const std::string& dbname) {
    try {
      auto driver = std::unique_ptr<interface::Driver>();
      auto conn = driver->connect(dbname);
      auto stmt = conn->createStatement();

      std::string s = "create table student ( sid int, sname varchar ( 10 ), majorid int, gradyear int )";
      stmt->executeUpdate(s);
      std::cout << "student table created" << std::endl;

      s = "create index majorid on student ( majorid )";
      stmt->executeUpdate(s);
      std::cout << s << " created" << std::endl;

      s = "insert into student ( sid, sname, majorid, gradyear ) values ";
      std::vector<std::string> studvals = {
          "( 1, 'joe', 10, 2021 )",
          "( 2, 'amy', 20, 2020 )",
          "( 3, 'max', 10, 2022 )",
          "( 4, 'sue', 20, 2022 )",
          "( 5, 'bob', 30, 2020 )",
          "( 6, 'kim', 20, 2020 )",
          "( 7, 'art', 30, 2021 )",
          "( 8, 'pat', 20, 2019 )",
          "( 9, 'lee', 10, 2021 )",
      };
      for (const auto& v : studvals) stmt->executeUpdate(s + v);
      std::cout << "student data inserted" << std::endl;

      s = "create table dept ( did int, dname varchar ( 8 ) )";
      stmt->executeUpdate(s);
      std::cout << "dept table created" << std::endl;

      s = "insert into dept ( did, dname ) values ";
      std::vector<std::string> deptvals = {
          "( 10, 'compsci' )",
          "( 20, 'math' )",
          "( 30, 'drama' )",
      };
      for (const auto& v : deptvals) stmt->executeUpdate(s + v);
      std::cout << "dept data inserted" << std::endl;

      s = "create table section ( sectid int, courseid int, prof varchar ( 8 ), yearoffered int )";
      stmt->executeUpdate(s);
      std::cout << "section table created" << std::endl;

      s = "insert into section ( sectid, courseid, prof, yearoffered ) values ";
      std::vector<std::string> sectvals = {
          "( 13, 12, 'turing', 2018 )",
          "( 23, 12, 'turing', 2019 )",
          "( 33, 32, 'newton', 2019 )",
          "( 43, 32, 'einstein', 2017 )",
          "( 53, 62, 'brando', 2018 )",
      };
      for (const auto& v : sectvals) stmt->executeUpdate(s + v);

      s = "create table enroll ( eId int, studentid int, sectionid int, grade varchar ( 2 ) )";
      stmt->executeUpdate(s);

      s = "insert into enroll ( eid, studentid, sectionid, grade ) values ";
      std::vector<std::string> enrollvals = {
          "( 14, 1, 13, 'A' )",
          "( 24, 1, 43, 'C' )",
          "( 34, 2, 43, 'B+' )",
          "( 44, 4, 33, 'B' )",
          "( 54, 4, 53, 'A' )",
          "( 64, 6, 53, 'A' )",
      };
      for (const auto& v : enrollvals) stmt->executeUpdate(s + v);
      std::cout << "enroll data inserted" << std::endl;
    } catch (std::exception& e) {
      std::cout << e.what() << std::endl;
      throw;  // surface failures to the test
    }
  }

  struct Row {
    int count;
    int maxmajorid;
    int minmajorid;
    int summajorid;
    int gradyear;
  };

  TEST_CASE("GroupByTest/Main", "[GroupByTest]") {
    std::string fileName = "groupbyTest";
    std::filesystem::remove_all(fileName);  // Clean start

    createDB(fileName);

    auto driver = std::unique_ptr<interface::Driver>();
    auto conn = driver->connect(fileName);
    auto stmt = conn->createStatement();

    SECTION("GROUP BY gradyear (unordered)") {
      std::string qry =
        "select count, maxmajorid, minmajorid, summajorid, gradyear from student group by gradyear";
      auto rs = stmt->executeQuery(qry);

      std::vector<Row> rows;

      std::string t1 = "count";
      std::string t2 = "maxmajorid";
      std::string t3 = "minmajorid";
      std::string t4 = "summajorid";
      std::string t5 = "gradyear";

      while (rs.next()) {
        rows.push_back(Row{
          rs.getInt(t1),
          rs.getInt(t2),
          rs.getInt(t3),
          rs.getInt(t4),
          rs.getInt(t5)
        });
      }
      rs.close();

      // Expect 4 grad years present.
      REQUIRE(rows.size() == 4);

      // Build expectations keyed by gradyear
      std::map<int, Row> expected{
        {2019, {1, 20, 20, 20, 2019}},
        {2020, {3, 30, 20, 70, 2020}},
        {2021, {3, 30, 10, 50, 2021}},
        {2022, {2, 20, 10, 30, 2022}}
      };

      // Verify each returned row matches the expected tuple for its gradyear
      for (const auto& r : rows) {
        REQUIRE(expected.count(r.gradyear) == 1);
        const auto& exp = expected[r.gradyear];
        REQUIRE(r.count == exp.count);
        REQUIRE(r.maxmajorid == exp.maxmajorid);
        REQUIRE(r.minmajorid == exp.minmajorid);
        REQUIRE(r.summajorid == exp.summajorid);
      }
    }

    SECTION("GROUP BY + ORDER BY count (nondecreasing)") {
      std::string qry =
        "select count, maxmajorid, minmajorid, summajorid, gradyear "
        "from student group by gradyear order by count";
      auto rs = stmt->executeQuery(qry);

      std::vector<Row> rows;

      std::string t1 = "count";
      std::string t2 = "maxmajorid";
      std::string t3 = "minmajorid";
      std::string t4 = "summajorid";
      std::string t5 = "gradyear";

      while (rs.next()) {
        rows.push_back(Row{
          rs.getInt(t1),
          rs.getInt(t2),
          rs.getInt(t3),
          rs.getInt(t4),
          rs.getInt(t5)
        });
      }
      rs.close();

      // Expect 4 grad years present.
      REQUIRE(rows.size() == 4);

      // Check nondecreasing order by count
      for (size_t i = 1; i < rows.size(); ++i) {
        REQUIRE(rows[i].count >= rows[i - 1].count);
      }

      // Same content as the first section (order can differ for ties)
      std::map<int, Row> expected{
        {2019, {1, 20, 20, 20, 2019}},
        {2020, {3, 30, 20, 70, 2020}},
        {2021, {3, 30, 10, 50, 2021}},
        {2022, {2, 20, 10, 30, 2022}}
      };
      for (const auto& r : rows) {
        REQUIRE(expected.count(r.gradyear) == 1);
        const auto& exp = expected[r.gradyear];
        REQUIRE(r.count == exp.count);
        REQUIRE(r.maxmajorid == exp.maxmajorid);
        REQUIRE(r.minmajorid == exp.minmajorid);
        REQUIRE(r.summajorid == exp.summajorid);
      }
    }
  }
}
