#include "VertexBufferGL.h"
#include <cstdint>
#include <glad/glad.h>

int VertexBufferGL::Init(void* data, uint32_t count, uint32_t vertex_size, uint32_t index) {
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, count, data, GL_STATIC_DRAW);
    glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)0);
    glEnableVertexAttribArray(index);
    return 1;
}

uint32_t VertexBufferGL::getVertices() {
    return mVBO;
}