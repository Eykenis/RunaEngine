#pragma once
#include <string>
#include "../Core/eigen-3.4.0/Eigen/Eigen"

class ShaderModule {
    public:
    virtual ~ShaderModule() { }
    virtual int Init(const char* vs, const char* fs) = 0;
    virtual void Use() = 0;
    virtual void setMat4(const std::string &name, const Eigen::Matrix4f &mat) = 0;
};