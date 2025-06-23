#pragma once
#include "../Component.h"
#include "../../Core/eigen-3.4.0/Eigen/Eigen"
#include "../../Core/eigen-3.4.0/Eigen/src/Geometry/AngleAxis.h"
#include "../../Core/eigen-3.4.0/Eigen/src/Geometry/Transform.h"

class Transform : public Component {
public:
  
  Transform(
    Eigen::Vector3f pos = Eigen::Vector3f(0.f, 0.f, 0.f),
    Eigen::Vector3f rot = Eigen::Vector3f(0.f, 0.f, 0.f),
    Eigen::Vector3f sca = Eigen::Vector3f(1.f, 1.f, 1.f)
    ) : _Position(pos), _Rotation(rot), _Scale(sca)
    { }
  virtual ~Transform() { }

  Eigen::Vector3f& position() {
    return _Position;
  }

  Eigen::Vector3f& rotation() {
      return _Rotation;
  }

  Eigen::Vector3f& scale() {
      return _Scale;
  }

  Eigen::Matrix4f getModelMatrix() {
    Eigen::Affine3f t = Eigen::Affine3f::Identity();
    t.scale(_Scale);
    t.rotate(
      Eigen::AngleAxisf(_Rotation.x(), Eigen::Vector3f::UnitX()) * 
      Eigen::AngleAxisf(_Rotation.y(), Eigen::Vector3f::UnitY()) * 
      Eigen::AngleAxisf(_Rotation.z(), Eigen::Vector3f::UnitZ())
    );
    t.translate(_Position);
    return t.matrix();
  }
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Transform, _Position, _Rotation, _Scale)
private:
  Eigen::Vector3f _Position;
  Eigen::Vector3f _Rotation;
  Eigen::Vector3f _Scale;
};