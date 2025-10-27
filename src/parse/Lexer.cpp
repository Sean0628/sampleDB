#include "parse/Lexer.h"

namespace parse {
  static std::string unquote(const std::string& str) {
    if (str.length() >= 2 && str[0] == Word::QUOTE && str[str.length() - 1] == Word::QUOTE) {
      return str.substr(1, str.length() - 2);
    }
    return str;
  }

  Lexer::Lexer(const std::string& str) {
    initKeywords();
    _tokenizer = std::make_unique<StreamTokenizer>(str);
  }

  Lexer &Lexer::operator=(const Lexer& other) {
    return *this;
  }

  bool Lexer::matchDelimiter(const char delim) const {
    return delim == _tokenizer->ttype()[0];
  }

  bool Lexer::matchInteger() const {
    return _tokenizer->ttype() == StreamTokenizer::TT_NUMBER;
  }

  bool Lexer::matchString() const {
    return _tokenizer->ttype()[0] == Word::QUOTE;
  }

  bool Lexer::matchKeyword(const std::string& keyword) const {
    return _tokenizer->ttype() == StreamTokenizer::TT_WORD &&
           _tokenizer->sval() == keyword;
  }

  bool Lexer::matchIdentifier() const {
    if (_tokenizer->ttype() != StreamTokenizer::TT_WORD) return false;

    auto itr = std::find(_keywords.begin(), _keywords.end(), _tokenizer->sval());
    return itr == _keywords.end();
  }

  void Lexer::eatDelimiter(const char delim) {
    if (!matchDelimiter(delim)) throw std::runtime_error("Expected delimiter: " + delim);

    nextToken();
  }

  int Lexer::eatInteger() const {
    if (!matchInteger()) throw std::runtime_error("Expected integer");

    int intValue = _tokenizer->nval();
    nextToken();

    return intValue;
  }

  std::string Lexer::eatString() const {
    if (!matchString()) throw std::runtime_error("Expected string");

    std::string strValue = _tokenizer->sval();
    nextToken();

    return unquote(strValue);
  }

  void Lexer::eatKeyword(const std::string& keyword) {
    if (!matchKeyword(keyword)) throw std::runtime_error("Expected keyword: " + keyword);

    nextToken();
  }

  std::string Lexer::eatIdentifier() const {
    if (!matchIdentifier()) throw std::runtime_error("Expected identifier");

    std::string identifier = _tokenizer->sval();
    nextToken();

    return identifier;
  }

  void Lexer::nextToken() const {
    _tokenizer->nextToken();
  }

  void Lexer::initKeywords() {
    _keywords = std::vector<std::string>{
      Word::SELECT,
      Word::FROM,
      Word::WHERE,
      Word::AMPERSAND,
      Word::INSERT,
      Word::INTO,
      Word::VALUES,
      Word::DELETE,
      Word::UPDATE,
      Word::SET,
      Word::CREATE,
      Word::TABLE,
      Word::VARCHAR,
      Word::INT,
      Word::VIEW,
      Word::AS,
      Word::INDEX,
      Word::ON,
    };
  }
}
