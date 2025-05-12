#include "file/Page.h"

namespace file {
  Page::Page(int blocksize) {
    _bb = std::make_shared<std::vector<char>>(blocksize);
  }

  std::vector<char> Page::getBytes(int offset) {
    int len = getInt(offset);
    if (offset + sizeof(int) + len > (*_bb).size()) exit(1);

    std::vector<char> byte_vector(len);
    std::memcpy(&byte_vector[0], &(*_bb)[offset + sizeof(int)], len);

    return byte_vector;
  }

  void Page::setBytes(int offset, const std::vector<char>& b) {
    int len = b.size();
    if (offset + sizeof(int) + len > (*_bb).size()) exit(1);

    setInt(offset, len);
    std::memcpy(&(*_bb)[offset + sizeof(int)], &b[0], len);
  }

  int Page::getInt(int offset) {
    if (offset + sizeof(int) > (*_bb).size()) exit(1);

    int n;
    std::memcpy(&n, &(*_bb)[offset], sizeof(int));
    return n;
  }

  void Page::setInt(int offset, int n) {
    if (offset + sizeof(int) > (*_bb).size()) exit(1);

    std::memcpy(&(*_bb)[offset], &n, sizeof(int));
  }

  std::string Page::getString(int offset) {
    std::vector<char> bytes = getBytes(offset);
    std::string s(bytes.begin(), bytes.end());

    return s;
  }

  void Page::setString(int offset, std::string s) {
    std::vector<char> bytes(s.begin(), s.end());
    setBytes(offset, bytes);
  }

  int Page::maxLength(int strlen) {
    return sizeof(int) + strlen * sizeof(char);
  }

  std::shared_ptr<std::vector<char>> Page::contents() {
    return _bb;
  }
}
