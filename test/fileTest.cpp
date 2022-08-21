#include "app/SampleDB.cpp"
#include "file/BlockId.cpp"
#include "file/FileMgr.cpp"
#include "file/Page.cpp"

int main() {
  app::SampleDB db("filetest", 400, 8);
  file::FileMgr& fm = db.fileMgr();
  file::BlockId blk("testfile", 2);
  int pos1 = 88;

  file::Page p1(fm.blockSize());
  string s = "abcdefghijklm";
  p1.setString(pos1, s);
  int size = file::Page::maxLength(s.size());

  int pos2 = pos1 + size;
  p1.setInt(pos2, 345);
  fm.write(blk, p1);

  file::Page p2(fm.blockSize());
  fm.read(blk, p2);

  cout << "offset " << pos2 << " contains " << p2.getInt(pos2) << endl;
  cout << "offset " << pos1 << " contains " << p2.getString(pos1) << endl;

  return 0;
}
