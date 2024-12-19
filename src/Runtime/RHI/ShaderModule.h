#pragma once
#include <string>

class ShaderModule {
    public:
    virtual ~ShaderModule() { }
    virtual int Init(const char* vs, const char* fs) = 0;
    virtual void Use() = 0;
};