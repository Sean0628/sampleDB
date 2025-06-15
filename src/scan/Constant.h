#pragma once

#include <string>
#include <memory>

namespace scan {
  class Constant {
    public:
      friend bool operator==(const Constant& lhs, const Constant& rhs);
      friend bool operator!=(const Constant& lhs, const Constant& rhs);
      friend bool operator<(const Constant& lhs, const Constant& rhs);
      friend bool operator>(const Constant& lhs, const Constant& rhs);
      friend bool operator<=(const Constant& lhs, const Constant& rhs);
      friend bool operator>=(const Constant& lhs, const Constant& rhs);
      friend void operator+=(const Constant& lhs, const Constant& rhs);

      Constant() {};
      Constant(const Constant& other);
      Constant(int ival);
      Constant(const std::string& sval);
      Constant &operator=(const Constant& other);
      int asInt() const;
      std::string asString() const;
      int hashCode() const;
      std::string toString() const;
      bool isNull() const;
    private:
      std::unique_ptr<int> _ival;
      std::unique_ptr<std::string> _sval;
  };
}
