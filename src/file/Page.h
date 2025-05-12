#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstring>

namespace file {
  class Page {
    public:
      Page(int blocksize);
      Page(std::shared_ptr<std::vector<char>>& byte_buffer) {
        _bb = byte_buffer;
      }
      std::vector<char> getBytes(int offset);
      void setBytes(int offset, const std::vector<char>& b);
      std::string getString(int offset);
      void setString(int offset, std::string s);
      int getInt(int offset);
      void setInt(int offset, int n);
      static int maxLength(int strlen);
      std::shared_ptr<std::vector<char>> contents();
    private:
      std::shared_ptr<std::vector<char>> _bb;
  };
}
