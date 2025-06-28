#pragma once

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include "record/FieldInfo.h"

namespace record {
  class Schema {
    public:
      Schema() {};
      Schema &operator=(const Schema &schema);
      void addField(const std::string& fieldName, int fieldType, int length);
      void addIntField(const std::string& fieldName);
      void addStringField(const std::string& fieldName, int length);
      void add(const std::string& fieldName, const Schema& schema);
      void addAll(const Schema& schema);
      std::vector<std::string> fieldNames() const;
      bool hasField(const std::string& fieldName) const;
      int fieldType(const std::string& fieldName) const;
      int fieldLength(const std::string& fieldName) const;

      static const int INTEGER = 0, VARCHAR = 1;
    private:
      std::vector<std::string> _fields;
      std::map<std::string, FieldInfo> _fieldInfo;
  };
}
