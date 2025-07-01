#include "catch.hpp"

#include <iostream>
#include <string>
#include <vector>
#include "parse/StreamTokenizer.h"
#include "parse/Lexer.h"

TEST_CASE("Lexer handles identifier and integer equality expressions", "[LexerTest]") {
  std::vector<std::string> inputs = {
    "A = 1",
    "3 = B"
  };

  for (const auto& str : inputs) {
    parse::Lexer lex(str);
    std::string x = "";
    int y = 0;

    if (lex.matchIdentifier()) {
      x = lex.eatIdentifier();
      lex.eatDelimiter('=');
      y = lex.eatInteger();
    } else {
      y = lex.eatInteger();
      lex.eatDelimiter('=');
      x = lex.eatIdentifier();
    }

    // You can use INFO to show diagnostic output
    INFO("Parsed input: " << str);
    INFO("Extracted: " << x << " = " << y);

    // Basic expectations (adjust as needed)
    REQUIRE(!x.empty());
    REQUIRE(y > 0);
  }
}
