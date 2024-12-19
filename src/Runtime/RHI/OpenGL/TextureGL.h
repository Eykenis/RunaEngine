#pragma once
#include "../TextureModule.h"
#include <cstdint>

class TextureGL : public TextureModule {
    uint32_t m_texture;
    public:
    virtual int Init(const char* texdata, uint32_t width, uint32_t height, uint32_t channels);
    virtual void Use();
    virtual uint32_t getIndex();
    virtual ~TextureGL() { }
};