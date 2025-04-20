#pragma once
#include "BaseObject.h"
#define RUNAENGINE_LIGHTSOURCE_TYPE_PARALLEL    0
#define RUNAENGINE_LIGHTSOURCE_TYPE_POINT       1
#define RUNAENGINE_LIGHTSOURCE_TYPE_UNDEFINED   0x3f3f3f3f

class LightSource : public BaseObject {
public:
  LightSource(float _intensity = 1.0f, uint32_t _lightSourceType = RUNAENGINE_LIGHTSOURCE_TYPE_UNDEFINED)
  : BaseObject() {
    intensity = _intensity;
    lightSourceType = _lightSourceType;
  }
  virtual ~LightSource() { };
  
protected:
  float intensity;
  uint32_t lightSourceType;
};