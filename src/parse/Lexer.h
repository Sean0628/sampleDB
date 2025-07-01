#pragma once

#include <vector>
#include <string>
#include <memory>

#include "StreamTokenizer.h"
#include "Word.h"

namespace parse {
  class Lexer {
    public:
      Lexer() = default;
      Lexer(const std::string& str);
      Lexer &operator=(const Lexer& other);
      bool matchDelimiter(const char delim) const;
      bool matchInteger() const;
      bool matchString() const;
      bool matchKeyword(const std::string& keyword) const;
      bool matchIdentifier() const;
      void eatDelimiter(const char delim);
      int eatInteger() const;
      std::string eatString() const;
      void eatKeyword(const std::string& keyword);
      std::string eatIdentifier() const;
    private:
      void nextToken() const;
      void initKeywords();
      std::vector<std::string> _keywords;
      std::unique_ptr<StreamTokenizer> _tokenizer;
  };
}
