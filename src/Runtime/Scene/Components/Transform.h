#pragma once
#include "../Component.h"
#include "../../Core/eigen-3.4.0/Eigen/Eigen"

class Transform : public Component {
public:
  
  Transform(
    Eigen::Vector3f pos = Eigen::Vector3f(0.f, 0.f, 0.f),
    Eigen::Vector3f rot = Eigen::Vector3f(0.f, 0.f, 0.f),
    Eigen::Vector3f sca = Eigen::Vector3f(1.f, 1.f, 1.f)
    ) : _Position(pos), _Rotation(rot), _Scale(sca)
    { }
  virtual ~Transform() { }

  Eigen::Vector3f& position();
  Eigen::Vector3f& rotation();
  Eigen::Vector3f& scale();
private:
  Eigen::Vector3f _Position;
  Eigen::Vector3f _Rotation;
  Eigen::Vector3f _Scale;
};