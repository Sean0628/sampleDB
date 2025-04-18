#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstring>

using namespace std;

namespace file {
  class Page {
    public:
      Page(int blocksize);
      vector<char> getBytes(int offset);
      void setBytes(int offset, vector<char>& b);
      string getString(int offset);
      void setString(int offset, string s);
      int getInt(int offset);
      void setInt(int offset, int n);
      static int maxLength(int strlen);
      shared_ptr<vector<char>> contents();
    private:
      shared_ptr<vector<char>> _bb;
  };
}
