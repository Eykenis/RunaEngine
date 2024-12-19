#include "IndexBufferGL.h"
#include "glad/glad.h"

int IndexBufferGL::Init(void* data, uint32_t count, uint32_t stride_size, uint32_t index) {
    glGenBuffers(1, &mEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count, data, GL_STATIC_DRAW);
    return 1;
}

uint32_t IndexBufferGL::getIndex() {
    return mEBO;
}