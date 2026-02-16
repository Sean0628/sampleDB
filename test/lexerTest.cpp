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

TEST_CASE("Lexer recognizes bool, true, and false as keywords", "[LexerTest]") {
  SECTION("bool is a keyword, not an identifier") {
    parse::Lexer lex("bool");
    REQUIRE(lex.matchKeyword("bool"));
    REQUIRE_FALSE(lex.matchIdentifier());
  }

  SECTION("true is a keyword, not an identifier") {
    parse::Lexer lex("true");
    REQUIRE(lex.matchKeyword("true"));
    REQUIRE_FALSE(lex.matchIdentifier());
  }

  SECTION("false is a keyword, not an identifier") {
    parse::Lexer lex("false");
    REQUIRE(lex.matchKeyword("false"));
    REQUIRE_FALSE(lex.matchIdentifier());
  }
}
