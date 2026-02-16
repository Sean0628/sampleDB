#include <iostream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <cctype>

#include "interface/Driver.h"
#include "interface/Connection.h"
#include "interface/Statement.h"
#include "interface/ResultSet.h"
#include "interface/MetaData.h"
#include "record/Schema.h"

static std::string trim(const std::string& s) {
  auto start = s.find_first_not_of(" \t\r\n");
  if (start == std::string::npos) return "";
  auto end = s.find_last_not_of(" \t\r\n");
  return s.substr(start, end - start + 1);
}

static std::string toLower(const std::string& s) {
  std::string result = s;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return result;
}

static void printResultSet(interface::ResultSet& rs) {
  auto md = rs.getMetaData();
  int colCount = md.getColumnCount();

  // Print header
  std::vector<std::string> colNames;
  std::vector<int> colTypes;
  std::vector<int> colWidths;

  for (int i = 1; i <= colCount; i++) {
    std::string name = md.getColumnName(i);
    colNames.push_back(name);
    colTypes.push_back(md.getColumnType(i));
    colWidths.push_back(md.getColumnDisplaySize(i));
  }

  // Print separator
  auto printSep = [&]() {
    std::cout << "+";
    for (int i = 0; i < colCount; i++) {
      std::cout << std::string(colWidths[i] + 2, '-') << "+";
    }
    std::cout << "\n";
  };

  // Print header row
  printSep();
  std::cout << "|";
  for (int i = 0; i < colCount; i++) {
    std::cout << " " << std::left << std::setw(colWidths[i]) << colNames[i] << " |";
  }
  std::cout << "\n";
  printSep();

  // Print data rows
  int rowCount = 0;
  while (rs.next()) {
    std::cout << "|";
    for (int i = 0; i < colCount; i++) {
      std::string name = colNames[i];
      if (colTypes[i] == record::Schema::INTEGER) {
        std::cout << " " << std::right << std::setw(colWidths[i]) << rs.getInt(name) << " |";
      } else if (colTypes[i] == record::Schema::BOOLEAN) {
        std::string bval = rs.getBool(name) ? "true" : "false";
        std::cout << " " << std::left << std::setw(colWidths[i]) << bval << " |";
      } else {
        std::cout << " " << std::left << std::setw(colWidths[i]) << rs.getString(name) << " |";
      }
    }
    std::cout << "\n";
    rowCount++;
  }
  printSep();
  std::cout << rowCount << " row(s)\n";
}

int main(int argc, char* argv[]) {
  std::string dbName = "sampledb";
  if (argc >= 2) {
    dbName = argv[1];
  }
  dbName = "data/" + dbName;

  std::cout << "sampleDB - connecting to '" << dbName << "'\n";
  std::cout << "Type SQL statements terminated with ';'. Type 'exit' to quit.\n\n";

  interface::Driver driver;
  std::unique_ptr<interface::Connection> conn;
  try {
    conn = driver.connect(dbName);
  } catch (const std::exception& e) {
    std::cerr << "Failed to connect: " << e.what() << "\n";
    return 1;
  }

  auto stmt = conn->createStatement();
  std::string buffer;

  while (true) {
    if (buffer.empty()) {
      std::cout << "sampledb> ";
    } else {
      std::cout << "       -> ";
    }
    std::cout << std::flush;

    std::string line;
    if (!std::getline(std::cin, line)) {
      std::cout << "\n";
      break;
    }

    std::string trimmedLine = trim(line);

    // Handle exit/quit on empty buffer
    if (buffer.empty()) {
      std::string lower = toLower(trimmedLine);
      if (lower == "exit" || lower == "quit" || lower == "\\q") {
        break;
      }
    }

    if (trimmedLine.empty()) {
      continue;
    }

    buffer += (buffer.empty() ? "" : " ") + line;

    // Check if statement is complete (ends with semicolon)
    std::string trimmedBuf = trim(buffer);
    if (trimmedBuf.back() != ';') {
      continue;
    }

    // Remove trailing semicolon and trim
    std::string sql = trim(trimmedBuf.substr(0, trimmedBuf.size() - 1));
    buffer.clear();

    if (sql.empty()) {
      continue;
    }

    try {
      std::string lowerSql = toLower(sql);
      if (lowerSql.substr(0, 6) == "select") {
        auto rs = stmt->executeQuery(sql);
        printResultSet(rs);
        rs.close();
      } else {
        int affected = stmt->executeUpdate(sql);
        std::string cmd = lowerSql.substr(0, lowerSql.find(' '));
        if (cmd == "create") {
          std::cout << "OK\n";
        } else {
          std::cout << affected << " row(s) affected\n";
        }
      }
    } catch (const std::exception& e) {
      std::cerr << "Error: " << e.what() << "\n";
    }
  }

  conn->close();
  std::cout << "Bye\n";
  return 0;
}
