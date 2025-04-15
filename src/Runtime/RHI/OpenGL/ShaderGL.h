#pragma once
#include "../ShaderModule.h"

class ShaderGL : public ShaderModule {
    public:
    virtual ~ShaderGL();
    virtual int Init(const char* vs, const char* fs);
    virtual void Use();
    virtual void setMat4(const std::string &name, const Eigen::Matrix4f &mat);
    uint32_t m_vertexShader, m_fragmentShader, m_shaderProgram;
};