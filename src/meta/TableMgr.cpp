#include "meta/TableMgr.h"
#include "record/TableScan.h"

namespace meta {
  TableMgr::TableMgr(bool isNew, tx::Transaction& tx) {
    record::Schema tcatSchema;
    tcatSchema.addStringField("tblname", MAX_NAME);
    tcatSchema.addIntField("slotsize");
    _tcatLayout = record::Layout(tcatSchema);

    record::Schema fcatSchema;
    fcatSchema.addStringField("tblname", MAX_NAME);
    fcatSchema.addStringField("fldname", MAX_NAME);
    fcatSchema.addIntField("type");
    fcatSchema.addIntField("length");
    fcatSchema.addIntField("offset");
    _fcatLayout = record::Layout(fcatSchema);

    if (isNew) {
      createTable("tblcat", tcatSchema, tx);
      createTable("fldcat", fcatSchema, tx);
    }
  }

  void TableMgr::createTable(const std::string& tableName, const record::Schema& schema, tx::Transaction& tx) {
    record::Layout layout(schema);
    // insert one record into tblcat
    record::TableScan tcat(tx, "tblcat", _tcatLayout);
    tcat.insert();
    tcat.setString("tblname", tableName);
    tcat.setInt("slotsize", layout.slotSize());
    tcat.close();

    // insert one record into fldcat for each field in schema
    record::TableScan fcat(tx, "fldcat", _fcatLayout);
    for (const auto& fieldName : schema.fieldNames()) {
      fcat.insert();
      fcat.setString("tblname", tableName);
      fcat.setString("fldname", fieldName);
      fcat.setInt("type", schema.fieldType(fieldName));
      fcat.setInt("length", schema.fieldLength(fieldName));
      fcat.setInt("offset", layout.offset(fieldName));
    }
    fcat.close();
  }

  record::Layout TableMgr::getLayout(const std::string& tableName, tx::Transaction& tx) {
    int size = -1;
    record::TableScan tcat(tx, "tblcat", _tcatLayout);
    while (tcat.next()) {
      if (tcat.getString("tblname") == tableName) {
        size = tcat.getInt("slotsize");
        break;
      }
    }
    tcat.close();

    record::Schema schema;
    std::map<std::string, int> offsets;
    record::TableScan fcat(tx, "fldcat", _fcatLayout);
    while (fcat.next()) {
      if (fcat.getString("tblname") == tableName) {
        std::string fieldName = fcat.getString("fldname");
        int type = fcat.getInt("type");
        int length = fcat.getInt("length");
        int offset = fcat.getInt("offset");
        offsets[fieldName] = offset;

        if (type == record::Schema::INTEGER) {
          schema.addIntField(fieldName);
        } else if (type == record::Schema::VARCHAR) {
          schema.addStringField(fieldName, length);
        }
      }
    }
    fcat.close();
    record::Layout layout(schema, offsets, size);
    return layout;
  }
}
