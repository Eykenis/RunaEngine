#pragma once

#include <crossguid/guid.hpp>
#include <nlohmann/json.hpp>

class BaseObject {
public:
  xg::Guid guid;
  BaseObject() {
    guid = xg::newGuid();
  }
  virtual ~BaseObject() = default;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(BaseObject, guid)
};