#pragma once

namespace parse {
  class Object {
    public:
      virtual ~Object() = default;
      virtual int getObjectId() = 0;
  };
}
