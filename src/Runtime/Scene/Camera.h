#pragma once
#include "Components/Components.h"
#include "GameObject.h"
#include "../Core/Memory/MemoryManager.h"
#include "Components/Transform.h"
#include <crossguid/guid.hpp>

class Camera : public GameObject {
private:
public:
    Eigen::Vector3f camDirection;
    Eigen::Vector3f camUp;
    float zNear;
    float zFar;
    float fovY, aspect;
    Camera(float dx, float dy, float dz, float px = 0, float py = 0,float pz = 0, float ratio = 60.0f) {
        camDirection[0] = dx;
        camDirection[1] = dy;
        camDirection[2] = dz;
        camUp[0] = 0.0f;
        camUp[1] = 1.0f;
        camUp[2] = 0.0f;
        zNear = 0.1f, zFar = 30.0f;
        fovY = 3.141593f * ratio / 180.0f;
        aspect = 9.0f / 16.0f;
        // AddComponent<Transform>(new Transform(Eigen::Vector3f(0.0f, 0.0f, 3.0f)));
        auto x= dynamic_cast<Transform*>(this->GetComponent<Transform>());
        x->position().x() = px;
        x->position().y() = py;
        x->position().z() = pz;
    }
    ~Camera() {}
    void setDir(float dx, float dy, float dz) {
        camDirection[0] = dx;
        camDirection[1] = dy;
        camDirection[2] = dz;
    }
    Eigen::Vector3f camRight() {
        return camDirection.cross(camUp);
    }
};