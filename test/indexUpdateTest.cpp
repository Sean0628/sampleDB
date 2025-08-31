#include "catch.hpp"

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "app/SampleDB.h"
#include "interface/Connection.h"
#include "interface/Driver.h"
#include "interface/Statement.h"
#include "interface/ResultSet.h"
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

  TEST_CASE("IndexUpdateTest/Main", "[IndexUpdateTest]") {
    std::string fileName = "indexreupdateTest";
    std::filesystem::remove_all(fileName);  // Clean start

    createDB(fileName);

    app::SampleDB db(fileName);
    auto transaction = db.newTransaction();
    auto& mdm = db.getMetaDataManager();

    // open a scan on the data table
    auto studentplan = std::static_pointer_cast<plan::Plan>(
        std::make_shared<plan::TablePlan>(transaction.get(), "student", &mdm));
    auto studentscan = std::static_pointer_cast<scan::UpdateScan>(studentplan->open());

    // create a map containing all indexes for student
    std::map<std::string, std::shared_ptr<indexing::Index>> indexes;
    std::map<std::string, meta::IndexInfo> idxinfo =
        mdm.getIndexInfo("student", *transaction);
    for (auto& [fldname, ii] : idxinfo) {
      indexes[fldname] = ii.open();
    }

    SECTION("Task 1: insert 'sam' and index entries") {
      studentscan->insert();
      studentscan->setInt("sid", 11);
      studentscan->setString("sname", "sam");
      studentscan->setInt("gradyear", 2023);
      studentscan->setInt("majorid", 30);

      record::RID samRid = studentscan->getRid();
      for (const auto& [fldname, _ii] : indexes) {
        scan::Constant dataval = studentscan->getValue(fldname);
        indexes[fldname]->insert(dataval, samRid);
      }

      SECTION("Task 2: delete 'joe' and remove index entries") {
        studentscan->beforeFirst();
        while (studentscan->next()) {
          if (studentscan->getString("sname") == "joe") {
            record::RID joeRid = studentscan->getRid();
            for (const auto& [fldname, _ii] : indexes) {
              scan::Constant dataval = studentscan->getValue(fldname);
              indexes[fldname]->remove(dataval, joeRid);
            }
            studentscan->remove();
            break;
          }
        }

        studentscan->beforeFirst();
        bool seenJoe = false;
        bool seenSam = false;
        while (studentscan->next()) {
          const auto name = studentscan->getString("sname");
          if (name == "joe") seenJoe = true;
          if (name == "sam") seenSam = true;
        }
        REQUIRE_FALSE(seenJoe);  // joe deleted
        REQUIRE(seenSam);        // sam inserted
      }
    }

    studentscan->close();
    for (const auto& [_, idx] : indexes) idx->close();
    transaction->commit();
  }
}
