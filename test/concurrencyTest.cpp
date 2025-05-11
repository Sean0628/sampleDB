#include "catch.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include "file/BlockId.h"
#include "file/Page.h"
#include "file/FileMgr.h"
#include "file/LogMgr.h"
#include "file/BufferMgr.h"
#include "tx/Transaction.h"

void runA(file::FileMgr& fm, file::LogMgr& lm, file::BufferMgr& bm) {
  try {
    tx::Transaction txA(fm, lm, bm);
    file::BlockId blk1("testfile", 1);
    file::BlockId blk2("testfile", 2);
    txA.pin(blk1);
    txA.pin(blk2);
    std::cout << "Tx A: request slock 1" << std::endl;
    txA.getInt(blk1, 0);
    std::cout << "Tx A: receive slock 1" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Tx A: request slock 2" << std::endl;
    txA.getInt(blk2, 0);
    std::cout << "Tx A: receive slock 2" << std::endl;
    txA.commit();
  } catch (const std::exception& e) {
    std::cerr << "Transaction A error: " << e.what() << std::endl;
  }
}

void runB(file::FileMgr& fm, file::LogMgr& lm, file::BufferMgr& bm) {
  try {
    tx::Transaction txB(fm, lm, bm);
    file::BlockId blk1("testfile", 1);
    file::BlockId blk2("testfile", 2);
    txB.pin(blk1);
    txB.pin(blk2);
    std::cout << "Tx B: request xlock 2" << std::endl;
    txB.setInt(blk2, 0, 10, true);
    std::cout << "Tx B: receive xlock 2" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Tx B: request slock 1" << std::endl;
    txB.getInt(blk1, 0);
    std::cout << "Tx B: receive slock 1" << std::endl;
    txB.commit();
  } catch (const std::exception& e) {
    std::cerr << "Transaction B error: " << e.what() << std::endl;
  }
}

void runC(file::FileMgr& fm, file::LogMgr& lm, file::BufferMgr& bm) {
  try {
    tx::Transaction txC(fm, lm, bm);
    file::BlockId blk1("testfile", 1);
    file::BlockId blk2("testfile", 2);
    txC.pin(blk1);
    txC.pin(blk2);
    std::cout << "Tx C: request xlock 1" << std::endl;
    txC.setInt(blk1, 0, 2, true);
    std::cout << "Tx C: receive xlock 1" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Tx C: request slock 2" << std::endl;
    txC.getInt(blk2, 0);
    std::cout << "Tx C: receive slock 2" << std::endl;
    txC.commit();
  } catch (const std::exception& e) {
    std::cerr << "Transaction C error: " << e.what() << std::endl;
  }
}

TEST_CASE("Concurrency control under contention", "[Concurrency]") {
  std::string fileName = "concurrencyTest";
  std::string logFileName = "simpledb.log";
  int blockSize = 400;
  auto path = std::filesystem::current_path() / fileName;

  file::FileMgr fm(path, blockSize);
  file::LogMgr lm(fm, logFileName);
  file::BufferMgr bm(fm, lm, 8);

  std::thread t1(runA, std::ref(fm), std::ref(lm), std::ref(bm));
  std::thread t2(runB, std::ref(fm), std::ref(lm), std::ref(bm));
  std::thread t3(runC, std::ref(fm), std::ref(lm), std::ref(bm));

  t1.join();
  t2.join();
  t3.join();

  REQUIRE(true); // Ensure test completes
}
