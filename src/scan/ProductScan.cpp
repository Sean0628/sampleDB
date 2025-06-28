#include "ProductScan.h"

namespace scan {
  ProductScan::ProductScan(std::shared_ptr<Scan> scan1, std::shared_ptr<Scan> scan2)
      : _scan1(scan1), _scan2(scan2) {
        scan1->next();
  }

  void ProductScan::beforeFirst() {
    _scan1->beforeFirst();
    _scan1->next();
    _scan2->beforeFirst();
  }

  bool ProductScan::next() {
    if (_scan2->next()) return true;

    _scan2->beforeFirst();
    return _scan1->next() && _scan2->next();
  }

  int ProductScan::getInt(const std::string& fieldName) {
    if (_scan1->hasField(fieldName)) return _scan1->getInt(fieldName);

    return _scan2->getInt(fieldName);
  }

  std::string ProductScan::getString(const std::string& fieldName) {
    if (_scan1->hasField(fieldName)) return _scan1->getString(fieldName);

    return _scan2->getString(fieldName);
  }

  Constant ProductScan::getValue(const std::string& fieldName) {
    if (_scan1->hasField(fieldName)) return _scan1->getValue(fieldName);

    return _scan2->getValue(fieldName);
  }

  bool ProductScan::hasField(const std::string& fieldName) {
    return _scan1->hasField(fieldName) || _scan2->hasField(fieldName);
  }

  void ProductScan::close() {
    _scan1->close();
    _scan2->close();
  }
}
