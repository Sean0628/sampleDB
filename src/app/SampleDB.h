#pragma once

#include <string>
#include <filesystem>
#include <iostream>
#include "file/FileMgr.h"

using namespace std;

namespace app {
  class SampleDB {
    public:
      SampleDB(string dirname, int blocksize, int buffsize);
      file::FileMgr& fileMgr();
    private:
      unique_ptr<file::FileMgr> _fm;
  };
}
