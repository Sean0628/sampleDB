#include "file/Page.h"

using namespace file;
using namespace std;

Page::Page(int blocksize) {
  _bb = make_shared<vector<char>>(blocksize);
}

vector<char> Page::getBytes(int offset) {
  int len = getInt(offset);
  if (offset + sizeof(int) + len > (*_bb).size()) exit(1);

  vector<char> byte_vector(len);
  memcpy(&byte_vector[0], &(*_bb)[offset + sizeof(int)], len);

  return byte_vector;
}

void Page::setBytes(int offset, const vector<char>& b) {
  int len = b.size();
  if (offset + sizeof(int) + len > (*_bb).size()) exit(1);

  setInt(offset, len);
  memcpy(&(*_bb)[offset + sizeof(int)], &b[0], len);
}

int Page::getInt(int offset) {
  if (offset + sizeof(int) > (*_bb).size()) exit(1);

  int n;
  memcpy(&n, &(*_bb)[offset], sizeof(int));
  return n;
}

void Page::setInt(int offset, int n) {
  if (offset + sizeof(int) > (*_bb).size()) exit(1);

  memcpy(&(*_bb)[offset], &n, sizeof(int));
}

string Page::getString(int offset) {
  vector<char> bytes = getBytes(offset);
  string s(bytes.begin(), bytes.end());

  return s;
}

void Page::setString(int offset, string s) {
  vector<char> bytes(s.begin(), s.end());
  setBytes(offset, bytes);
}

int Page::maxLength(int strlen) {
  return sizeof(int) + strlen * sizeof(char);
}

shared_ptr<vector<char>> Page::contents() {
  return _bb;
}
