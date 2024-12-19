#include "GraphicsManagerGL.h"
#include "../../AssetsImport/ImageImporter.h"
#include "IndexBufferGL.h"
#include "KHR/khrplatform.h"
#include "ShaderGL.h"
#include "VertexBufferGL.h"
#include "TextureGL.h"
#include "../../AssetsImport/AssetsManager.h"
#include <cstdint>
#include <fstream>
#include <memory>
#include <sstream>


int GraphicsManagerGL::Init(uint32_t width, uint32_t height) {
    int ret = gladLoadGL();
    if (!ret) return 0;
    glViewport(0, 0, width, height);
    glClearColor(0, 0.2f, 0.4f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    return ret;
}

std::shared_ptr<ShaderModule> GraphicsManagerGL::CreateShader(const char* vs_path, const char* fs_path) {
    auto ret = std::make_shared<ShaderGL>();
    std::string vs, fs;
    std::ifstream is(vs_path);
    if (!is.is_open()) {
        return nullptr;
    }
    std::stringstream ss, ss2;
    ss << is.rdbuf();
    vs = ss.str();

    is.close();
    is.clear();
    is.open(fs_path);
    if (!is.is_open()) {
        return nullptr;
    }
    
    ss2 << is.rdbuf();
    fs = ss2.str();
    ss.clear();
    is.close();
    if(ret->Init(vs.c_str(), fs.c_str())) {
        return ret;
    }
    return nullptr;
}

std::shared_ptr<VertexBufferModule> GraphicsManagerGL::CreateVertexBuffer(void* data, uint32_t count, uint32_t vertex_size, uint32_t index) {
    auto ret = std::make_shared<VertexBufferGL>();
    if (ret->Init(data, count, vertex_size, index)) {
        return ret;
    }
    std::cerr << "Vertex Buffer Initialization failed!" << std::endl;
    return nullptr;
}

void GraphicsManagerGL::Clear() {
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
}

uint32_t GraphicsManagerGL::CreateRenderable(Mesh* mesh) {
    RenderableGL ret;
    glGenVertexArrays(1, &ret.VAO);
    glBindVertexArray(ret.VAO);
    auto face = CreateIndexBuffer(mesh->Face.data(), mesh->Face.size() * sizeof(int), sizeof(int) * 3, 0);
    auto pos = CreateVertexBuffer(mesh->Position.data(), mesh->Position.size() * sizeof(float), 3 * sizeof(float), 0);
    auto normal = CreateVertexBuffer(mesh->Normal.data(), mesh->Normal.size() * sizeof(float), 3 * sizeof(float), 1);
    auto texcoord = CreateVertexBuffer(mesh->TexCoord[0].data(), mesh->TexCoord[0].size() * sizeof(float), sizeof(float) * 3, 2);
    // printf("%s\n", mesh->material.diffuse[0].path.c_str());
    if (!mesh->material.diffuse.empty()) {
        auto tex = CreateTexture2D(mesh->material.diffuse[0].path.c_str());
        ret.tex = tex.get()->getIndex();
    }
    else {
        auto tex = CreateTexture2D("D:/Github Repo/RunaEngine/assets/models/empty.png");
        ret.tex = tex.get()->getIndex();
    }

    ret.EBO = face.get()->getIndex();
    ret.VBOs.push_back(pos.get()->getVertices());
    ret.VBOs.push_back(normal.get()->getVertices());
    ret.VBOs.push_back(texcoord.get()->getVertices());
    ret.vsize.push_back(mesh->Position.size());
    ret.isize = mesh->Face.size();
    renderable.push_back(ret);
    glBindVertexArray(0);
    return renderable.size() - 1;
}

std::shared_ptr<IndexBufferModule> GraphicsManagerGL::CreateIndexBuffer(void* data, uint32_t count, uint32_t stride_size, uint32_t index) {
    auto ret = std::make_shared<IndexBufferGL>();
    if (ret->Init(data, count, stride_size, index)) {
        return ret;
    }
    std::cerr << "Index Buffer Initialization failed!" << std::endl;
    return nullptr;
}

std::shared_ptr<TextureModule> GraphicsManagerGL::CreateTexture2D(const char* tex_path) {
    AssetsManager am;
    ImageBufferHeader* data = reinterpret_cast<ImageBufferHeader*>(am.LoadAsset(tex_path));
    auto ret = std::make_shared<TextureGL>();
    ret->Init((const char*)data->data, data->width, data->height, uint32_t(data->color_type));
    if (ret)
        return ret;
    std::cerr << "Texture Initialization failed!" << std::endl;
    return nullptr;
}

void GraphicsManagerGL::Draw(uint32_t idx) {
    // glClear(GL_COLOR_BUFFER_BIT);
    // glBindVertexArray(mVAO);
    // glDrawArrays(GL_TRIANGLES, 0, buffersize);
    // glBindVertexArray(0);
    // if (idx >= renderable.size()) {
    //     std::cerr << "Illegal render index!" << std::endl;
    //     return;
    // }
    // auto ret = renderable[idx];
    // glBindVertexArray(mVAO);
    // glBindBuffer(GL_ARRAY_BUFFER, ret.VBOs[0]);
    // glDrawArrays(GL_TRIANGLES, 0, ret.vsize[0]);
    // glBindVertexArray(0);
}

void GraphicsManagerGL::DrawIndexed(uint32_t idx) {
    // glClear(GL_COLOR_BUFFER_BIT);
    // glBindVertexArray(renderable[idx].VAO);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx.get()->getIndex());
    // glDrawElements(GL_TRIANGLES, buffersize, GL_UNSIGNED_INT, 0);
    // glBindVertexArray(0);
    if (idx >= renderable.size()) {
        std::cerr << "Illegal render index!" << std::endl;
        return;
    }
    auto ret = renderable[idx];
    
    glBindVertexArray(renderable[idx].VAO);
    glBindBuffer(GL_ARRAY_BUFFER, ret.VBOs[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ret.EBO);
    glDrawElements(GL_TRIANGLES, ret.isize, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void GraphicsManagerGL::DrawAll() {
    uint32_t n = renderable.size();
    for (int i = 0; i < n; ++i) {
        if (renderable[i].tex) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, renderable[i].tex);
        }
        glBindVertexArray(renderable[i].VAO);
        // printf("vsize and isize and VBO and EBO ord: %d %d %d %d\n", renderable[i].vsize[0], renderable[i].isize, renderable[i].VBOs[0], renderable[i].EBO);
        glBindBuffer(GL_ARRAY_BUFFER, renderable[i].VBOs[0]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderable[i].EBO);
        glDrawElements(GL_TRIANGLES, renderable[i].isize, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

void GraphicsManagerGL::ReleaseAll() {
    glBindVertexArray(0);
    for (auto &x : renderable) {
        for (auto y : x.VBOs) {
            glDeleteBuffers(1, &y);
        }
        glDeleteBuffers(1, &x.EBO);
        glDeleteBuffers(1, &x.VAO);
    }
    while (!renderable.empty()) {
        renderable.pop_back();
    }
}

void GraphicsManagerGL::UseShader(std::shared_ptr<ShaderModule> shader) {
    if (shader) {
        shader->Use();
    }
    else {
        // TODO: Assertion
    }
}