# pragma once

namespace record {
  class FieldInfo {
    public:
      FieldInfo() = default;
        FieldInfo(int type, int length) : _type(type), _length(length) {}
        int type() const;
        int length() const;

    private:
      int _type;
      int _length;
  };
}
