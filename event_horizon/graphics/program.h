#pragma once

#include <string>
#include <unordered_map>
#include "graphic_constants.h"

struct ProgramUniformDesc {
    std::string   name;
    UniformFormat type;
    unsigned int  location;
};

class Program {
public:
    Program() {}

    const std::string& getId() const {
        return mId;
    }

    unsigned int handle() const {
        return mHandle;
    }

    bool hasUniform( const std::string& _name ) {
        return uniforms.find(_name) != uniforms.end();
    }

protected:
    std::string  mId; // Human readible Id
    unsigned int mHandle; // handle for the program (might vary from implementation to implementation)
    std::unordered_map<std::string, ProgramUniformDesc> uniforms;
};

#ifdef _OPENGL

#include "opengl/program_opengl.h"

#endif
