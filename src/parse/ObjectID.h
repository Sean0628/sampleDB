#pragma once

namespace parse {
  class ObjectID {
    public:
      static const int INSERT;
      static const int DELETE;
      static const int MODIFY;
      static const int CREATE_TABLE;
      static const int CREATE_VIEW;
      static const int CREATE_INDEX;
  };
}
