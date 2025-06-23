#pragma once
#include "../GraphicsManagerModule.h"
#include "../ShaderModule.h"
#include <cstdint>
#include <glad/glad.h>
#include <memory>
#include <vector>

struct RenderableGL {
    std::vector<uint32_t> VBOs;
    std::vector<uint32_t> vsize;
    Eigen::Matrix4f matModel; 
    // std::vector<float> model;
    uint32_t shader_idx;
    uint32_t tex;
    uint32_t isize;
    uint32_t EBO;
    uint32_t VAO;
    // uniform value position which need to be set while rendering
    std::map<std::string, Eigen::Vector3f> uniform_vec3f;
    std::map<std::string, float> uniform_float;
};

class GraphicsManagerGL : public GraphicsManagerModule {
    public:
    virtual int Init(uint32_t width, uint32_t height);
    virtual uint32_t CreateRenderable(Mesh* mesh, uint32_t shader_idx);
    virtual std::shared_ptr<VertexBufferModule> CreateVertexBuffer(void* data, uint32_t count, uint32_t vertex_size, uint32_t index);
    virtual std::shared_ptr<ShaderModule> CreateShader(const char* vs_path, const char* fs_path);
    virtual std::shared_ptr<TextureModule> CreateTexture2D(const char* tex_path);
    virtual std::shared_ptr<IndexBufferModule> CreateIndexBuffer(void* data, uint32_t count, uint32_t stride_size, uint32_t index);
    virtual void ReleaseRenderable(uint32_t renderable_idx);
    virtual void Draw(uint32_t idx);
    virtual void DrawIndexed(uint32_t idx);
    virtual void DrawAll();
    virtual void ReleaseAll();
    virtual void Clear();
    virtual void UseShader(std::shared_ptr<ShaderModule> shader);
    virtual void setModel(uint32_t idx, Eigen::Matrix4f mat);
    virtual void setVec3(uint32_t idx, const std::string& name, Eigen::Vector3f vec);
    virtual void setFloat(uint32_t idx, const std::string& name, float value);
    virtual void setGlobalMat4(const std::string& name, Eigen::Matrix4f mat);
    virtual void setGlobalVec3(const std::string& name, Eigen::Vector3f vec);
    virtual void setGlobalFloat(const std::string& name, float value);
    virtual void setView(Eigen::Matrix4f);
    virtual void setPerspective(Eigen::Matrix4f);
    private:
    std::vector<bool> renderable_active;
    std::vector<RenderableGL> renderable;
    std::vector<std::shared_ptr<ShaderModule>> shaders;
    Eigen::Matrix4f view, perspective, viewport;
};