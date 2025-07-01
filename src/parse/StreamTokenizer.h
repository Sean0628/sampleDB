#pragma once

#include <string>
#include <memory>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include "parse/Word.h"

namespace parse {
  typedef boost::tokenizer<boost::char_separator<char>> Tokenizer;

  class StreamTokenizer {
    public:
      StreamTokenizer() = default;
      StreamTokenizer(const std::string& str);
      std::string ttype() const;
      double nval() const;
      std::string sval() const;
      void nextToken();
      std::string getToken() const;
      static const std::string TT_EOF;
      static const std::string TT_WORD;
      static const std::string TT_NUMBER;
    private:
      std::unique_ptr<Tokenizer> _tokenizer;
      Tokenizer::iterator _currentToken;
  };
}
