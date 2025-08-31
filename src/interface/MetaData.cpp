#include "MetaData.h"

namespace interface {
  MetaData::MetaData(const record::Schema& schema) : _schema(schema) {}

  int MetaData::getColumnCount() const {
    try {
      return _schema.fieldNames().size();
    } catch (const std::exception& e) {
      throw std::runtime_error("Failed to get column count: " + std::string(e.what()));
    }
  }

  std::string MetaData::getColumnName(int column) const {
    try {
      return _schema.fieldNames().at(column - 1);
    } catch (const std::exception& e) {
      throw std::runtime_error("Failed to get column name: " + std::string(e.what()));
    }
  }

  int MetaData::getColumnType(int column) const {
    try {
      std::string fieldName = getColumnName(column);
      return _schema.fieldType(fieldName);
    } catch (const std::exception& e) {
      throw std::runtime_error("Failed to get column type: " + std::string(e.what()));
    }
  }

  int MetaData::getColumnDisplaySize(int column) const {
    try {
      std::string fieldName = getColumnName(column);
      int fieldType = _schema.fieldType(fieldName);
      int fieldLength = (fieldType == record::Schema::INTEGER) ? 6 : _schema.fieldLength(fieldName);
      return fieldName.length() > fieldLength ? fieldName.length() + 1: fieldLength + 1;
    } catch (const std::exception& e) {
      throw std::runtime_error("Failed to get column display size: " + std::string(e.what()));
    }
  }
}
