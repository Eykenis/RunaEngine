#pragma once
#include "LightSource.h"
#include "../Core/eigen-3.4.0/Eigen/Eigen"

class ParallelLightSource : public LightSource {
public:
  ParallelLightSource(float _intensity = 1.0f, Eigen::Vector3f _lightDirection = Eigen::Vector3f(0.0f, 0.0f, 0.0f))
  : LightSource(_intensity, RUNAENGINE_LIGHTSOURCE_TYPE_PARALLEL) {
    lightDirection = _lightDirection;
  }
  virtual ~ParallelLightSource() { };
  Eigen::Vector3f lightDirection;
};