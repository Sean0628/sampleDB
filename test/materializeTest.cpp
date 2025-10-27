#include "catch.hpp"

#include <iostream>
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

      s = "create index majorid on student ( majorid )";
      stmt->executeUpdate(s);

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
        "( 9, 'lee', 10, 2021 )"
      };
      for (const auto& v : studvals) stmt->executeUpdate(s + v);

      s = "create table dept ( did int, dname varchar ( 8 ) )";
      stmt->executeUpdate(s);

      s = "insert into dept ( did, dname ) values ";
      std::vector<std::string> deptvals = {
        "( 10, 'compsci' )",
        "( 20, 'math' )",
        "( 30, 'drama' )"
      };
      for (const auto& v : deptvals) stmt->executeUpdate(s + v);

      s = "create table section ( sectid int, courseid int, prof varchar ( 8 ), yearoffered int )";
      stmt->executeUpdate(s);

      s = "insert into section ( sectid, courseid, prof, yearoffered ) values ";
      std::vector<std::string> sectvals = {
        "( 13, 12, 'turing', 2018 )",
        "( 23, 12, 'turing', 2019 )",
        "( 33, 32, 'newton', 2019 )",
        "( 43, 32, 'einstein', 2017 )",
        "( 53, 62, 'brando', 2018 )"
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
        "( 64, 6, 53, 'A' )"
      };
      for (const auto& v : enrollvals) stmt->executeUpdate(s + v);
    } catch (std::exception& e) {
      std::cout << e.what() << std::endl;
      throw;
    }
  }
}

TEST_CASE("MaterializeTest/Main", "[MaterializeTest]") {
  std::string fileName = "materializeTest";
  std::filesystem::remove_all(fileName);  // clean start
  createDB(fileName);

  auto driver = std::unique_ptr<interface::Driver>();
  auto conn = driver->connect(fileName);
  auto stmt = conn->createStatement();

  SECTION("SELECT sname FROM (SELECT sid, sname FROM student WHERE gradyear=2020)") {
    std::string qry =
      "select sname from ( select sid, sname from student where gradyear = 2020 )";
    auto rs = stmt->executeQuery(qry);

    std::vector<std::string> names;
    std::string t1 = "sname";
    while (rs.next()) {
      names.push_back(rs.getString(t1));
    }
    rs.close();

    std::sort(names.begin(), names.end());
    std::vector<std::string> expected = {"amy", "bob", "kim"};
    REQUIRE(names == expected);
  }

  SECTION("SELECT studentid, grade FROM (SELECT studentid, grade FROM enroll WHERE grade='A')") {
    std::string qry =
      "select studentid, grade from ( select studentid, grade from enroll where grade = 'A' )";
    auto rs = stmt->executeQuery(qry);

    std::vector<std::pair<int,std::string>> rows;
    std::string t1 = "studentid";
    std::string t2 = "grade";

    while (rs.next()) {
      rows.emplace_back(rs.getInt(t1), rs.getString(t2));
    }
    rs.close();

    std::sort(rows.begin(), rows.end());
    std::vector<std::pair<int,std::string>> expected = {
      {1, "a"}, {4, "a"}, {6, "a"} // lowercase 'a'
    };
    for (size_t i = 0; i < expected.size(); ++i) {
      REQUIRE(rows[i].first == expected[i].first);
      REQUIRE(rows[i].second == expected[i].second);
    }
  }

  SECTION("Join two materialized subqueries on sid=studentid; expect only 'kim'") {
    std::string qry =
      "select sname "
      "from ( select sid, sname from student where gradyear = 2020 ) "
      ",    ( select studentid from enroll where grade = 'A' ) "
      "where sid = studentid";
    auto rs = stmt->executeQuery(qry);

    std::vector<std::string> names;
    std::string t1 = "sname";
    while (rs.next()) {
      names.push_back(rs.getString(t1));
    }
    rs.close();

    REQUIRE(names.size() == 1);
    REQUIRE(names[0] == "kim"); // only student with gradyear=2020 and an 'A'
  }
}
