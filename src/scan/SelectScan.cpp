#include "SelectScan.h"

namespace scan {
  SelectScan::SelectScan(const std::shared_ptr<Scan>& scan, const Predicate& pred)
      : _scan(scan), _predicate(pred) {
  }

  void SelectScan::beforeFirst() {
    _scan->beforeFirst();
  }

  bool SelectScan::next() {
    while (_scan->next()) {
      if (_predicate.isSatisfied(_scan.get())) return true;
    }

    return false;
  }

  int SelectScan::getInt(const std::string& fieldName) {
    return _scan->getInt(fieldName);
  }

  bool SelectScan::getBool(const std::string& fieldName) {
    return _scan->getBool(fieldName);
  }

  std::string SelectScan::getString(const std::string& fieldName) {
    return _scan->getString(fieldName);
  }

  Constant SelectScan::getValue(const std::string& fieldName) {
    return _scan->getValue(fieldName);
  }

  bool SelectScan::hasField(const std::string& fieldName) {
    return _scan->hasField(fieldName);
  }

  void SelectScan::close() {
    _scan->close();
  }

  void SelectScan::setInt(const std::string& fieldName, int value) {
    std::shared_ptr<UpdateScan> updateScan = std::dynamic_pointer_cast<UpdateScan>(_scan);
    if (updateScan) {
      updateScan->setInt(fieldName, value);
    } else {
      throw std::runtime_error("Cannot set value on a non-update scan");
    }
  }

  void SelectScan::setString(const std::string& fieldName, const std::string& value) {
    std::shared_ptr<UpdateScan> updateScan = std::dynamic_pointer_cast<UpdateScan>(_scan);
    if (updateScan) {
      updateScan->setString(fieldName, value);
    } else {
      throw std::runtime_error("Cannot set value on a non-update scan");
    }
  }

  void SelectScan::setValue(const std::string& fieldName, const Constant& value) {
    std::shared_ptr<UpdateScan> updateScan = std::dynamic_pointer_cast<UpdateScan>(_scan);
    if (updateScan) {
      updateScan->setValue(fieldName, value);
    } else {
      throw std::runtime_error("Cannot set value on a non-update scan");
    }
  }

  void SelectScan::insert() {
    std::shared_ptr<UpdateScan> updateScan = std::dynamic_pointer_cast<UpdateScan>(_scan);
    if (updateScan) {
      updateScan->insert();
    } else {
      throw std::runtime_error("Cannot insert on a non-update scan");
    }
  }

  void SelectScan::remove() {
    std::shared_ptr<UpdateScan> updateScan = std::dynamic_pointer_cast<UpdateScan>(_scan);
    if (updateScan) {
      updateScan->remove();
    } else {
      throw std::runtime_error("Cannot remove on a non-update scan");
    }
  }

  record::RID SelectScan::getRid() {
    std::shared_ptr<UpdateScan> updateScan = std::dynamic_pointer_cast<UpdateScan>(_scan);
    if (updateScan) {
      return updateScan->getRid();
    } else {
      throw std::runtime_error("Cannot get RID on a non-update scan");
    }
  }

  void SelectScan::moveToRid(const record::RID& rid) {
    std::shared_ptr<UpdateScan> updateScan = std::dynamic_pointer_cast<UpdateScan>(_scan);
    if (updateScan) {
      updateScan->moveToRid(rid);
    } else {
      throw std::runtime_error("Cannot move to RID on a non-update scan");
    }
  }
}
