#include <vector>
#include "tx/LogRecord/SetStringRecord.h"

namespace tx {
  SetStringRecord::SetStringRecord(file::Page* p) {
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
    _str = p->getString(vpos);
  }

  std::string SetStringRecord::toString() const {
    return "<SETSTRING, " + std::to_string(_txNum) + ", " + _blkId.toString() + ", " +
           std::to_string(_offset) + ", " + _str + ">";
  }

  void SetStringRecord::undo(Transaction& tx) {
    tx.pin(_blkId);
    tx.setString(_blkId, _offset, _str, false);
    tx.unpin(_blkId);
  }

  int SetStringRecord::writeToLog(file::LogMgr& lm, int txNum, file::BlockId& blkId, int offset, const std::string& str) {
    int tpos = sizeof(int);
    int fpos = tpos + sizeof(int);
    int bpos = fpos + file::Page::maxLength(blkId.fileName().size());
    int opos = bpos + sizeof(int);
    int vpos = opos + file::Page::maxLength(str.size());
    int recSize = vpos + file::Page::maxLength(str.size());
    
    auto bytes = std::make_shared<std::vector<char>>(recSize, 0);
    auto page = file::Page(bytes);
    
    page.setInt(0, SETSTRING);
    page.setInt(tpos, txNum);
    page.setString(fpos, blkId.fileName());
    page.setInt(bpos, blkId.number());
    page.setInt(opos, offset);
    page.setString(vpos, str);

    return lm.append(*bytes);
  }
}
