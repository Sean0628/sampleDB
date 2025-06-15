#include "catch.hpp"

#include <iostream>
#include <string>
#include <map>

#include "app/SampleDB.h"
#include "tx/Transaction.h"
#include "record/Schema.h"
#include "record/Layout.h"
#include "record/TableScan.h"
#include "meta/MetaDataMgr.h"
#include "meta/StatInfo.h"
#include "meta/IndexInfo.h"

namespace {
  const std::string kTestDir = "metadatamgrTest";

  // Utility: Create SampleDB and schema
  std::unique_ptr<app::SampleDB> createDB() {
    return std::make_unique<app::SampleDB>(kTestDir, 400, 8);
  }

  record::Schema sampleSchema() {
    record::Schema sch;
    sch.addIntField("A");
    sch.addStringField("B", 9);
    return sch;
  }
}

TEST_CASE("MetaDataMgr supports table, stats, view, and index metadata", "[MetaDataMgr]") {
  auto db = createDB();
  auto tx = db->newTransaction();
  meta::MetaDataMgr mdm(true, *tx);

  const std::string tableName = "mytable";
  const std::string viewName = "viewA";
  const std::string indexA = "indexA";
  const std::string indexB = "indexB";
  const std::string viewDef = "select B from mytable where A = 1";

  SECTION("Table metadata can be created and retrieved") {
    mdm.createTable(tableName, sampleSchema(), *tx);
    record::Layout layout = mdm.getLayout(tableName, *tx);
    record::Schema sch2 = layout.schema();

    std::cout << tableName << " has slot size " << layout.slotSize() << std::endl;
    for (const auto& fld : sch2.fieldNames()) {
      std::string type = sch2.fieldType(fld) == record::Schema::INTEGER
                         ? "int"
                         : "varchar(" + std::to_string(sch2.fieldLength(fld)) + ")";
      std::cout << fld << ": " << type << std::endl;
    }

    REQUIRE(sch2.hasField("A"));
    REQUIRE(sch2.hasField("B"));
    REQUIRE(sch2.fieldType("A") == record::Schema::INTEGER);
    REQUIRE(sch2.fieldLength("B") == 9);
  }

  SECTION("Statistics metadata is generated from data") {
    mdm.createTable(tableName, sampleSchema(), *tx);
    record::Layout layout = mdm.getLayout(tableName, *tx);
    record::TableScan ts(*tx, tableName, layout);

    for (int i = 0; i < 50; ++i) {
      ts.insert();
      ts.setInt("A", i);
      ts.setString("B", "rec" + std::to_string(i));
    }

    meta::StatInfo si = mdm.getStatInfo(tableName, layout, *tx);

    std::cout << "B(" << tableName << ") = " << si.blocksAccessed() << std::endl;
    std::cout << "R(" << tableName << ") = " << si.recordOutput() << std::endl;
    std::cout << "V(" << tableName << ", A) = " << si.distinctValues("A") << std::endl;
    std::cout << "V(" << tableName << ", B) = " << si.distinctValues("B") << std::endl;

    REQUIRE(si.recordOutput() == 50);
    REQUIRE(si.distinctValues("A") == 1 + (50 / 3)); // 17 distinct values for A
    REQUIRE(si.distinctValues("B") == 1 + (50 / 3)); // 17 distinct values for B
  }

  SECTION("View metadata can be stored and retrieved") {
    mdm.createView(viewName, viewDef, *tx);
    std::string result = mdm.getViewDef(viewName, *tx);

    std::cout << "View def = " << result << std::endl;
    REQUIRE(result == viewDef);
  }

  SECTION("Index metadata supports multiple fields") {
    mdm.createTable(tableName, sampleSchema(), *tx);
    record::Layout layout = mdm.getLayout(tableName, *tx);
    record::TableScan ts(*tx, tableName, layout);

    for (int i = 0; i < 50; ++i) {
      ts.insert();
      ts.setInt("A", i);
      ts.setString("B", "rec" + std::to_string(i));
    }

    mdm.createIndex(indexA, tableName, "A", *tx);
    mdm.createIndex(indexB, tableName, "B", *tx);

    auto idxmap = mdm.getIndexInfo(tableName, *tx);

    REQUIRE(idxmap.find("A") != idxmap.end());
    REQUIRE(idxmap.find("B") != idxmap.end());

    const meta::IndexInfo& iiA = idxmap.at("A");
    std::cout << "IndexA: B=" << iiA.blocksAccessed() << std::endl;
    std::cout << "IndexA: R=" << iiA.recordOutput() << std::endl;
    std::cout << "IndexA: V(A)=" << iiA.distinctValues("A") << std::endl;
    std::cout << "IndexA: V(B)=" << iiA.distinctValues("B") << std::endl;

    const meta::IndexInfo& iiB = idxmap.at("B");
    std::cout << "IndexB: B=" << iiB.blocksAccessed() << std::endl;
    std::cout << "IndexB: R=" << iiB.recordOutput() << std::endl;
    std::cout << "IndexB: V(A)=" << iiB.distinctValues("A") << std::endl;
    std::cout << "IndexB: V(B)=" << iiB.distinctValues("B") << std::endl;
  }

  tx->commit();
}
