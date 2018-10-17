#pragma once

#include <string>
#include <unordered_map>

#include "shader_opengl.h"
#include "program_opengl.h"
#include "../program_list.h"
#include "core/file_manager.h"
#include "core/callback_dependency.h"
#include "core/property.h"


struct ShaderBuilder {
    ShaderBuilder( const std::string& _name ) {
        name = _name;
        vertexShader = _name;
        fragmentShader = _name;
    }

    ShaderBuilder& vsh( const std::string& _name ) {
        vertexShader = _name;
        return *this;
    }

    ShaderBuilder& fsh( const std::string& _name ) {
        fragmentShader = _name;
        return *this;
    }

    ShaderBuilder& tec( const std::string& _name ) {
        tessControlShader = _name;
        return *this;
    }

    ShaderBuilder& tev( const std::string& _name ) {
        tessEvaluationShader = _name;
        return *this;
    }

    ShaderBuilder& gsh( const std::string& _name ) {
        geometryShader = _name;
        return *this;
    }

    ShaderBuilder& csh( const std::string& _name ) {
        computeShader = _name;
        return *this;
    }

    bool makeDirect( DependencyMaker& _md, const ucchar_p& _data, const std::string&_nameWithExtension );

    Property<std::string> name;

    Property<std::string> vertexShader;
    Property<std::string> fragmentShader;

    Property<std::string> tessControlShader;
    Property<std::string> tessEvaluationShader;
    Property<std::string> geometryShader;
    Property<std::string> computeShader;
};

class ShaderManager : public DependencyMaker {
public:
    DEPENDENCY_MAKER_EXIST(mPrograms);
    ShaderManager() {}
    virtual ~ShaderManager() {}

public:
    void addShader( const std::string& id, Shader::Type stype );
    bool loadProgram( const ShaderBuilder& sb );

    // OpenGL init function
    void init( const std::string& cacheFolder, const std::string& cacheLabel );
    bool loadShaders();
    std::shared_ptr<ProgramOpenGL> P( const std::string& id ) const;

    int getProgramCount() const;
    void setCacheData( const std::string& cacheFolder, const std::string& cacheLabel );
    const std::vector<std::shared_ptr<ProgramOpenGL>>& Programs() const;

    void createInjection( const std::string& key, const std::string& text );
    void createCCInjectionMap();

private:
    void createInjections();
    void addUniformsInjections();
    void addLayoutInjections();
    void addMacroInjections();
    void addVertexShaderInjections();
    void addFragmentShaderInjections();

    bool createProgram( std::shared_ptr<ProgramOpenGL>, const std::string& cacheFolder, const std::string& cacheLabel );
    void addToShaderList( const std::vector<std::string>& vertexShadersFileNames, Shader::Type st );

    std::string openFileWithIncludeParsing( const std::string& filename );
    std::string parsePreprocessorMacro( std::string& source );
    std::string injectPreprocessorMacro( std::string& source );
    std::string injectIncludes( std::string& sm );

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
    std::unordered_map<std::string, std::string> shaderInjection;

    std::vector<std::shared_ptr<ProgramOpenGL>> mProgramsIds;

    ProgramMap mPrograms;

    int mNumReloads = 0;
    std::string mCacheFolder;
    std::string mCacheLabel;
};
