#pragma once
#include "BaseObject.h"

class Component : public BaseObject {
public:
  Component() {
    guid = xg::newGuid();
  }
  virtual ~Component() { };
};