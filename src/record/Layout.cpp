#include "file/Page.h"
#include "record/Layout.h"

namespace record {
  Layout::Layout(const Schema& schema) : _schema(schema) {
    int pos = static_cast<int>(sizeof(int));
    for (const auto& fieldName : _schema.fieldNames()) {
      _offsets[fieldName] = pos;
      pos += lengthInBytes(fieldName);
    }
    _slotSize = pos;
  }

  Layout::Layout(const Schema& schema, std::map<std::string, int>& offsets, int slotSize)
    : _schema(schema), _offsets(offsets), _slotSize(slotSize) {}

  Layout& Layout::operator=(const Layout& layout) {
    if (this != &layout) {
      _schema = layout._schema;
      _offsets = layout._offsets;
      _slotSize = layout._slotSize;
    }
    return *this;
  }

  Schema Layout::schema() const {
    return _schema;
  }

  int Layout::offset(const std::string& fieldName) const {
    auto it = _offsets.find(fieldName);
    if (it != _offsets.end()) {
      return it->second;
    }
    throw std::runtime_error("Field not found: " + fieldName);
  }

  int Layout::slotSize() const {
    return _slotSize;
  }

  int Layout::lengthInBytes(const std::string& fieldName) const {
    int fieldType = _schema.fieldType(fieldName);
    if (fieldType == Schema::INTEGER) {
      return static_cast<int>(sizeof(int));
    } else if (fieldType == Schema::BOOLEAN) {
      return static_cast<int>(sizeof(int));
    } else if (fieldType == Schema::VARCHAR) {
      return file::Page::maxLength(_schema.fieldLength(fieldName));
    }

    throw std::runtime_error("Unknown field type: " + std::to_string(fieldType));
  }
}
