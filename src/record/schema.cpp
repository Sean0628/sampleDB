#include "record/Schema.h"

namespace record {
  Schema& Schema::operator=(const Schema& schema) {
    if (this != &schema) {
      _fields = schema._fields;
      _fieldInfo = schema._fieldInfo;
    }

    return *this;
  }

  void Schema::addField(const std::string& fieldName, int fieldType, int length) {
    _fields.push_back(fieldName);
    _fieldInfo[fieldName] = FieldInfo(fieldType, length);
  }

  void Schema::addIntField(const std::string& fieldName) {
    addField(fieldName, INTEGER, 0);
  }

  void Schema::addStringField(const std::string& fieldName, int length) {
    addField(fieldName, VARCHAR, length);
  }

  void Schema::add(const std::string& fieldName, Schema& schema) {
    int type = schema.fieldType(fieldName);
    int length = schema.fieldLength(fieldName);
    addField(fieldName, type, length);
  }

  void Schema::addAll(Schema& schema) {
    for (const auto& fieldName : schema.fieldNames()) {
      add(fieldName, schema);
    }
  }

  std::vector<std::string> Schema::fieldNames() const {
    return _fields;
  }

  bool Schema::hasField(const std::string& fieldName) const {
    return _fieldInfo.find(fieldName) != _fieldInfo.end();
  }

  int Schema::fieldType(const std::string& fieldName) const {
    auto it = _fieldInfo.find(fieldName);
    if (it != _fieldInfo.end()) {
      return it->second.type();
    }

    throw std::runtime_error("Field not found: " + fieldName);
  }

  int Schema::fieldLength(const std::string& fieldName) const {
    auto it = _fieldInfo.find(fieldName);
    if (it != _fieldInfo.end()) {
      return it->second.length();
    }

    throw std::runtime_error("Field not found: " + fieldName);
  }
}
