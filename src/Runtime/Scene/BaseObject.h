#pragma once
#include <crossguid/guid.hpp>

class BaseObject {
protected:
  xg::Guid guid;
public:
  BaseObject() {
    guid = xg::newGuid();
  }
  virtual ~BaseObject() { }
};