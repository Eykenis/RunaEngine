#pragma once
#include "../Component.h"
#include "../../Core/eigen-3.4.0/Eigen/Eigen"
#define DEFAULT_VSHADER_PATH "D:/Github Repo/RunaEngine/assets/shaders/default_shader.vs"
#define DEFAULT_FSHADER_PATH "D:/Github Repo/RunaEngine/assets/shaders/default_shader.ps"

class ShaderReference : public Component {
public:
    ShaderReference(const char* vpath = DEFAULT_VSHADER_PATH, const char* fpath = DEFAULT_FSHADER_PATH) : vpath(vpath), fpath(fpath) {}
    virtual ~ShaderReference() { }

    bool isSame(ShaderReference& another) {
        return vpath == another.vpath && fpath == another.fpath;
    }
    std::string vpath, fpath;
private:
};