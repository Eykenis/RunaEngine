#include "TextureGL.h"
#include <cstdint>
#include <glad/glad.h>

int TextureGL::Init(const char* data, uint32_t width, uint32_t height, uint32_t channels) {
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (data) {
        if (channels == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else if (channels == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        else {
            return 0;
        }
        glGenerateMipmap(GL_TEXTURE_2D);
        return 1;
    }
    else {
        return 0;
    }
    return 1;
}

void TextureGL::Use() {
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

uint32_t TextureGL::getIndex() {
    return m_texture;
}