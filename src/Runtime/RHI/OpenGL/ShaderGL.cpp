#include "ShaderGL.h"
#include <glad/glad.h>
#include <iostream>

int ShaderGL::Init(const char* vs, const char* fs) {
    int success;
    char logs[512];


    m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_vertexShader, 1, &vs, 0);
    glCompileShader(m_vertexShader);
    glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(m_vertexShader, 512, NULL, logs);
        std::cerr << "ERROR: Shader Vertex Complication failed\n" << logs << std::endl;
    }

    m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_fragmentShader, 1, &fs, 0);
    glCompileShader(m_fragmentShader);
    glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(m_fragmentShader, 512, NULL, logs);
        std::cerr << "ERROR: Shader Fragment Complication failed\n" << logs << std::endl;
    }

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, m_vertexShader);
    glAttachShader(m_shaderProgram, m_fragmentShader);
    glLinkProgram(m_shaderProgram);

    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);
    return 1;
}

void ShaderGL::Use() {
    glUseProgram(m_shaderProgram);
}

void ShaderGL::setMat4(const std::string &name, const Eigen::Matrix4f &mat) {
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, name.c_str()), 1, GL_FALSE, mat.data());
}

void ShaderGL::setVec3(const std::string &name, const Eigen::Vector3f &vec) {
    glUniform3f(glGetUniformLocation(m_shaderProgram, name.c_str()), vec.x(), vec.y(), vec.z());
}

ShaderGL::~ShaderGL() {

}