#include <vector>
#include "tx/LogRecord/SetIntRecord.h"

namespace tx {
  SetIntRecord::SetIntRecord(file::Page* p) {
    int tpos = sizeof(int);
    _txNum = p->getInt(tpos);
    int fpos = tpos + sizeof(int);
    std::string fileName = p->getString(fpos);
    int bpos = fpos + file::Page::maxLength(fileName.size());
    int blkNum = p->getInt(bpos);
    _blkId = file::BlockId(fileName, blkNum);
    int opos = bpos + sizeof(int);
    _offset = p->getInt(opos);
    int vpos = opos + sizeof(int);
    _val = p->getInt(vpos);
  }

  std::string SetIntRecord::toString() const {
    return "<SETINT, " + std::to_string(_txNum) + ", " + _blkId.toString() + ", " +
           std::to_string(_offset) + ", " + std::to_string(_val) + ">";
  }

  void SetIntRecord::undo(Transaction& tx) {
    tx.pin(_blkId);
    tx.setInt(_blkId, _offset, _val, false);
    tx.unpin(_blkId);
  }

  int SetIntRecord::writeToLog(logging::LogMgr& lm, int txNum, file::BlockId& blkId, int offset, int val) {
    int tpos = sizeof(int);
    int fpos = tpos + sizeof(int);
    int bpos = fpos + file::Page::maxLength(blkId.fileName().size());
    int opos = bpos + sizeof(int);
    int vpos = opos + sizeof(int);
    int recSize = vpos + sizeof(int);
    auto bytes = std::make_shared<std::vector<char>>(recSize, 0);
    auto page = file::Page(bytes);
    page.setInt(0, SETINT);
    page.setInt(tpos, txNum);
    page.setString(fpos, blkId.fileName());
    page.setInt(bpos, blkId.number());
    page.setInt(opos, offset);
    page.setInt(vpos, val);
    return lm.append(*bytes);
  }
}
