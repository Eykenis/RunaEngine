#pragma once
#include "../ShaderModule.h"

class ShaderGL : public ShaderModule {
    public:
    virtual ~ShaderGL();
    virtual int Init(const char* vs, const char* fs);
    virtual void Use();
    uint32_t m_vertexShader, m_fragmentShader, m_shaderProgram;
};