#pragma once
// #include "OpenGL/RenderGL.h"
// #include "OpenGL/ShaderGL.h"
// #include "OpenGL/VertexBufferGL.h"
#include "IndexBufferModule.h"
#include "TextureModule.h"
#include "VertexBufferModule.h"
#include "ShaderModule.h"
#include "../Scene/Mesh.h"
#include "../../Runtime/Core/eigen-3.4.0/Eigen/src/Core/Matrix.h"
#include <memory>

enum GraphicsAPI{
    opengl,
    d3d11,
};

class GraphicsManagerModule {
    public:
    // bool registered = 0;
    virtual int Init(uint32_t width, uint32_t height) = 0;
    virtual uint32_t CreateRenderable(Mesh* mesh, uint32_t shader_idx) = 0;
    virtual void ReleaseRenderable(uint32_t renderable_idx) = 0;
    virtual std::shared_ptr<VertexBufferModule> CreateVertexBuffer(void* data, uint32_t count, uint32_t vertex_size, uint32_t index) = 0;
    virtual std::shared_ptr<IndexBufferModule> CreateIndexBuffer(void* data, uint32_t count, uint32_t stride_size, uint32_t index) = 0;
    virtual std::shared_ptr<ShaderModule> CreateShader(const char* vs_path, const char* fs_path) = 0;
    virtual std::shared_ptr<TextureModule> CreateTexture2D(const char* tex_path) = 0;
    virtual void Draw(uint32_t idx) = 0;
    virtual void DrawIndexed(uint32_t idx) = 0;
    virtual void DrawAll() = 0;
    virtual void ReleaseAll() = 0;
    virtual void Clear() = 0;
    virtual void UseShader(std::shared_ptr<ShaderModule> shader) = 0;
    virtual void setModel(uint32_t idx, Eigen::Matrix4f mat) = 0; // for renderable
    virtual void setVec3(uint32_t idx, const std::string& name, Eigen::Vector3f vec) = 0; // for shader
    virtual void setFloat(uint32_t idx, const std::string& name, float value) = 0; // for shader
    virtual void setGlobalMat4(const std::string& name, Eigen::Matrix4f mat) = 0;
    virtual void setGlobalVec3(const std::string& name, Eigen::Vector3f vec) = 0;
    virtual void setGlobalFloat(const std::string& name, float value) = 0;
    virtual void setView(Eigen::Matrix4f) = 0;
    virtual void setPerspective(Eigen::Matrix4f) = 0;
};