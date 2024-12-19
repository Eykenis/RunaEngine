#pragma once
#include <cstdint>

class VertexBufferModule {
    public:
    virtual int Init(void* data, uint32_t count, uint32_t vertex_size, uint32_t index) = 0; // vertex data, vertex count, single vertex size
    virtual uint32_t getVertices() = 0;
};