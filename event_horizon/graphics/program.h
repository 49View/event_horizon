#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <core/descriptors/material.h>
#include "graphic_constants.h"

struct ProgramUniformDesc {
    std::string   name;
    UniformFormat type;
    unsigned int  location;
};

class Program {
public:
    Program() = default;

    const std::string& getId() const {
        return mId;
    }

    unsigned int handle() const {
        return mHandle;
    }

    bool hasUniform( const std::string& _name ) {
        return uniforms.find(_name) != uniforms.end();
    }

    std::shared_ptr<Material> getDefaultUniforms() const { return uniformDefaults; }
protected:
    std::string  mId; // Human readible Id
    unsigned int mHandle = 0; // handle for the program (might vary from implementation to implementation)
    std::unordered_map<std::string, ProgramUniformDesc> uniforms;
    std::shared_ptr<Material> uniformDefaults;
};

#ifdef _OPENGL

#include "opengl/program_opengl.h"

#endif
