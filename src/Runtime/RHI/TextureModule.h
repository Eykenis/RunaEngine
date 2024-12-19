#pragma once
#include <cstdint>

class TextureModule {
    public:
    virtual int Init(const char* texdata, uint32_t width, uint32_t height, uint32_t channels) = 0;
    virtual void Use() = 0;
    virtual uint32_t getIndex() = 0;
    virtual ~TextureModule() { }
};