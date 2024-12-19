#pragma once
#include <cstdint>

class IndexBufferModule {
    public:
    virtual int Init(void* data, uint32_t count, uint32_t stride_size, uint32_t index) = 0; // vertex data, vertex count, single vertex size
    virtual uint32_t getIndex() = 0;
    virtual ~IndexBufferModule() {}
};