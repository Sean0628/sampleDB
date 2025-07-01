#include "StreamTokenizer.h"

namespace parse {
  const std::string StreamTokenizer::TT_EOF = "EOF";
  const std::string StreamTokenizer::TT_WORD = "WORD";
  const std::string StreamTokenizer::TT_NUMBER = "NUMBER";

  StreamTokenizer::StreamTokenizer(const std::string& str) {
    boost::char_separator<char> sep{" ", ","};
    _tokenizer = std::make_unique<Tokenizer>(str, sep);
    _currentToken = _tokenizer->begin();
  }

  std::string StreamTokenizer::ttype() const {
    if (_currentToken == _tokenizer->end()) return TT_EOF;

    bool isNumber = true;
    bool isAlnumeric = true;
    std::string token = *_currentToken;

    for (char c : token) {
      if (!isdigit(c)) isNumber = false;
      if (!isalnum(c)) isAlnumeric = false;
    }

    if (isNumber) return TT_NUMBER;
    if (isAlnumeric) return TT_WORD;
    if (token[0] == Word::QUOTE) {
      std::string q(1, Word::QUOTE);
      return q;
    }

    return boost::algorithm::to_lower_copy(token);
  }

  double StreamTokenizer::nval() const {
    try {
      return std::stod(*_currentToken);
    } catch (std::exception& e) {
      throw std::runtime_error("Invalid number format: " + *_currentToken);
    }
  }
  
  std::string StreamTokenizer::sval() const {
    return boost::algorithm::to_lower_copy(*_currentToken);
  }

  void StreamTokenizer::nextToken() {
    if (_currentToken != _tokenizer->end()) _currentToken++;
  }

  std::string StreamTokenizer::getToken() const {
    if (_currentToken == _tokenizer->end()) return TT_EOF;

    return boost::algorithm::to_lower_copy(*_currentToken);
  }
}
