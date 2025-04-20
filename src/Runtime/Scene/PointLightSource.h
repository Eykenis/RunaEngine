#pragma once
#include "LightSource.h"
#include "../Core/eigen-3.4.0/Eigen/Eigen"

class PointLightSource : public LightSource {
public:
  PointLightSource(float _intensity = 1.0f, Eigen::Vector3f _lightPosition = Eigen::Vector3f(0.0f, 0.0f, 0.0f))
  : LightSource(_intensity, RUNAENGINE_LIGHTSOURCE_TYPE_POINT) {
    lightPosition = _lightPosition;
  }
  virtual ~PointLightSource() { };
  Eigen::Vector3f lightPosition;
};