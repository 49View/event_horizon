#pragma once

#include <string>
#include <unordered_map>

#include "shader_opengl.h"
#include "program_opengl.h"
#include "core/file_manager.h"
#include "core/property.h"
#include "core/heterogeneous_map.hpp"

struct ShaderProgramDesc {
    explicit ShaderProgramDesc( const std::string& _name ) {
        name = _name;
        vertexShader = _name;
        fragmentShader = _name;
    }

    ShaderProgramDesc& vsh( const std::string& _name ) {
        vertexShader = _name;
        return *this;
    }

    ShaderProgramDesc& fsh( const std::string& _name ) {
        fragmentShader = _name;
        return *this;
    }

    ShaderProgramDesc& tec( const std::string& _name ) {
        tessControlShader = _name;
        return *this;
    }

    ShaderProgramDesc& tev( const std::string& _name ) {
        tessEvaluationShader = _name;
        return *this;
    }

    ShaderProgramDesc& gsh( const std::string& _name ) {
        geometryShader = _name;
        return *this;
    }

    ShaderProgramDesc& csh( const std::string& _name ) {
        computeShader = _name;
        return *this;
    }

    Property<std::string> name;

    Property<std::string> vertexShader;
    Property<std::string> fragmentShader;
    Property<std::string> tessControlShader;
    Property<std::string> tessEvaluationShader;
    Property<std::string> geometryShader;
    Property<std::string> computeShader;
};

class ShaderManager {
public:
    ShaderManager();
    virtual ~ShaderManager() = default;

public:
    bool addShader( const std::string& id, Shader::Type stype );
    bool loadProgram( const ShaderProgramDesc& sb );

    // OpenGL init function
    bool loadShaders( bool _performCompileOnly = false );
    std::shared_ptr<ProgramOpenGL> P( const std::string& id ) const;

    int getProgramCount() const;
    std::vector<GLuint> ProgramsHandles() const;

    void inject( const std::string& key, const std::string& text );
    bool checkShaderChanged( const std::string& key, const std::string& text ) const;
    bool injectIfChanged( const std::string& key, const std::string& text );
    void createCCInjectionMap();

private:

    std::shared_ptr<ProgramOpenGL> initProgram( const ShaderProgramDesc& sb );
    void allocateProgram( const ShaderProgramDesc& _pd );
    void allocateShader( const std::string& id, Shader::Type stype );
    bool injectShadersWithCode();

    std::string openFileWithIncludeParsing( const std::string& filename );
    std::string parsePreprocessorMacro( std::string& source );
    std::string injectPreprocessorMacro( std::string& source );
    std::string injectIncludes( std::string& sm );

    std::shared_ptr<Shader> vshForProgram( std::shared_ptr<ProgramOpenGL> program );
    std::shared_ptr<Shader> tchForProgram( std::shared_ptr<ProgramOpenGL> program );
    std::shared_ptr<Shader> tehForProgram( std::shared_ptr<ProgramOpenGL> program );
    std::shared_ptr<Shader> gshForProgram( std::shared_ptr<ProgramOpenGL> program );
    std::shared_ptr<Shader> fshForProgram( std::shared_ptr<ProgramOpenGL> program );
    std::shared_ptr<Shader> cshForProgram( std::shared_ptr<ProgramOpenGL> program );

    std::unordered_map<std::string, HeterogeneousMap> uniformsMap;

private:

    typedef std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderMap;
    typedef std::unordered_map<std::string, std::shared_ptr<ProgramOpenGL>> ProgramMap;

    ShaderMap mVertexShaders;
    ShaderMap mTesselationControlShaders;
    ShaderMap mTesselationEvaluationShaders;
    ShaderMap mGeometryShaders;
    ShaderMap mFragmentShaders;
    ShaderMap mComputeShaders;

    std::unordered_map<std::string, std::string> ccShaderMap;
    std::unordered_map<std::string, std::string> mDefineMap;

    std::unordered_map<std::string, std::string> shaderSourcesMap;

    ProgramMap mPrograms;

    std::vector<ShaderProgramDesc> programDescs;

    int mNumReloads = 0;
};
