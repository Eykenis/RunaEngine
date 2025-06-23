#pragma once
#include "../Component.h"
#include "../../Core/eigen-3.4.0/Eigen/Eigen"
#include "../../Core/eigen-3.4.0/Eigen/src/Geometry/AngleAxis.h"
#include "../../Core/eigen-3.4.0/Eigen/src/Geometry/Transform.h"

class Camera : public Component {
public:
    Eigen::Vector3f camDirection;
    Eigen::Vector3f camUp;
    float zNear;
    float zFar;
    float fovY, aspect;
    Camera(float dx = 0.0f, float dy = 0.0f, float dz = -1.0f, float ratio = 90.0f) {
        camDirection[0] = dx;
        camDirection[1] = dy;
        camDirection[2] = dz;
        camUp[0] = 0.0f;
        camUp[1] = 1.0f;
        camUp[2] = 0.0f;
        zNear = 0.1f, zFar = 30.0f;
        fovY = 3.141593f * ratio / 180.0f;
        aspect = 16.0f / 9.0f;
    }
    void setDir(float dx, float dy, float dz) {
        camDirection[0] = dx;
        camDirection[1] = dy;
        camDirection[2] = dz;
    }
    Eigen::Vector3f camRight() {
        return camDirection.cross(camUp);
    }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Camera, camDirection, camUp, zNear, zFar, fovY, aspect)
};