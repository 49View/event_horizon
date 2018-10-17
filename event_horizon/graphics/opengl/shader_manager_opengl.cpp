#include <regex>

#include "shader_manager_opengl.h"
#include "../shader_list.h"
#include "core/zlib_util.h"
#include "core/tar_util.h"

bool ShaderBuilder::makeDirect( DependencyMaker& _md, const ucchar_p& _data, const std::string& _nameWithExtension ) {

    ShaderManager& sm = static_cast<ShaderManager&>(_md);

    std::string urca( reinterpret_cast<const char *>(_data.first), _data.second);

    std::string fkey = (getFileNameExt(_nameWithExtension) == ".glsl") ? name.value : _nameWithExtension;
    sm.createInjection( fkey, urca );

    return true;
}

std::string ShaderManager::injectIncludes( std::string& sm ) {
    size_t includeFind = 0;
    while (( includeFind = sm.find( "#include" )) != std::string::npos ) {
        auto fi = sm.find_first_of( '"', includeFind );
        auto fe = sm.find_first_of( '"', fi + 1 );
        auto filename = sm.substr( fi + 1, fe - fi - 1 );
        filename = string_trim_upto( filename, "." );

        sm.erase( sm.begin() + includeFind, sm.begin() + fe + 1 );
        sm.insert( includeFind, shaderInjection[filename] );
    }

    return sm;
}

std::string ShaderManager::parsePreprocessorMacro( std::string& sm ) {

    for ( const auto& [k,v] : ccShaderMap ) {
        size_t pfind = 0;
        while (( pfind = sm.find( k )) != std::string::npos ) {
            auto fe = pfind + k.length();
            sm.erase( sm.begin() + pfind, sm.begin() + fe );
            sm.insert( pfind, v );

        }
    }

    std::string defStartString = "#define_code ";
    std::string defEndString = "#end_code";

    size_t defineFind = 0;
    while (( defineFind = sm.find( defStartString )) != std::string::npos ) {
        std::istringstream iss( sm.substr( defineFind, sm.size() - defineFind ));
        std::string defineName;
        iss >> defineName;
        iss >> defineName;

        auto definenNameCut = sm.find( defineName );
        auto defineNameCurEnd = sm.find( defEndString );

        ASSERT( definenNameCut != std::string::npos );
        ASSERT( defineNameCurEnd != std::string::npos );

        definenNameCut += defineName.size();

        mDefineMap[defineName] = sm.substr( definenNameCut, defineNameCurEnd - definenNameCut );

        sm.erase( sm.begin() + defineFind, sm.begin() + defineNameCurEnd + defEndString.size() + 1 );
    }

    return sm;
}

std::string ShaderManager::injectPreprocessorMacro( std::string& sm ) {
    // Inject global macros
//    auto vi = sm.find( "#version" );
//    ASSERT( vi != std::string::npos );
//    auto viendline = sm.find( "\n", vi );
//
//    for ( auto& d : mGlobalMacros ) {
//        sm.insert( viendline + 1, "#define " + d.first + " " + d.second + "\r\n" );
//    }

    size_t fi = 0;
    for ( auto& w : mDefineMap ) {
        if (( fi = sm.find( w.first )) != std::string::npos ) {
            auto fe = fi + w.first.size();
            sm.erase( sm.begin() + fi, sm.begin() + fe + 1 );
            sm.insert( fi, w.second );
        }
    }

    return sm;
}

std::string ShaderManager::openFileWithIncludeParsing( const std::string& filename ) {
    std::string sb = shaderInjection[filename];
//    FileManager::writeLocalFile( filename, sb.c_str(), sb.length(), true );
    std::string includeResolved = injectIncludes( sb );
    parsePreprocessorMacro( includeResolved );
    return injectPreprocessorMacro( includeResolved );
}

void ShaderManager::addShader( const std::string& id, Shader::Type stype ) {
    //LOGI("Adding vertex shader: %s", id);
    if ( id.empty()) return;

    std::string source;

    switch ( stype ) {
        case Shader::TYPE_VERTEX_SHADER:
            if ( mVertexShaders.find( id ) == mVertexShaders.end() || mNumReloads ) {
                source = openFileWithIncludeParsing( id + ".vsh" );
                mVertexShaders[id] = std::make_shared<Shader>( stype, id, source.c_str());
            }
            break;
        case Shader::TYPE_TESSELATION_CONTROL_SHADER:
            if ( mTesselationControlShaders.find( id ) == mTesselationControlShaders.end() || mNumReloads) {
                source = openFileWithIncludeParsing( id + ".tch" );
                mTesselationControlShaders[id] = std::make_shared<Shader>( stype, id, source.c_str());
            }
            break;
        case Shader::TYPE_TESSELATION_EVALUATION_SHADER:
            if ( mTesselationEvaluationShaders.find( id ) == mTesselationEvaluationShaders.end() || mNumReloads ) {
                source = openFileWithIncludeParsing( id + ".teh" );
                mTesselationEvaluationShaders[id] = std::make_shared<Shader>( stype, id, source.c_str());
            }
            break;
        case Shader::TYPE_GEOMETRY_SHADER:
            if ( mGeometryShaders.find( id ) == mGeometryShaders.end() || mNumReloads ) {
                source = openFileWithIncludeParsing( id + ".gsh" );
                mGeometryShaders[id] = std::make_shared<Shader>( stype, id, source.c_str());
            }
            break;
        case Shader::TYPE_FRAGMENT_SHADER:
            if ( mFragmentShaders.find( id ) == mFragmentShaders.end() || mNumReloads) {
                source = openFileWithIncludeParsing( id + ".fsh" );
                mFragmentShaders[id] = std::make_shared<Shader>( stype, id, source.c_str());
            }
            break;
        case Shader::TYPE_COMPUTE_SHADER:
            if ( mComputeShaders.find( id ) == mComputeShaders.end() || mNumReloads) {
                source = openFileWithIncludeParsing( id + ".csh" );
                mComputeShaders[id] = std::make_shared<Shader>( stype, id, source.c_str());
            }
            break;
        default:
            break;
    }
}

bool ShaderManager::loadProgram( const ShaderBuilder& sb ) {

    if ( mPrograms.find( sb.name ) == mPrograms.end() ) {
        mPrograms[sb.name] = std::make_shared<ProgramOpenGL>( sb.name,
                                                              sb.vertexShader,
                                                              sb.tessControlShader,
                                                              sb.tessEvaluationShader,
                                                              sb.geometryShader,
                                                              sb.fragmentShader,
                                                              sb.computeShader );
    }

    addShader( sb.vertexShader, Shader::TYPE_VERTEX_SHADER );
    addShader( sb.tessControlShader, Shader::TYPE_TESSELATION_CONTROL_SHADER );
    addShader( sb.tessEvaluationShader, Shader::TYPE_TESSELATION_EVALUATION_SHADER );
    addShader( sb.geometryShader, Shader::TYPE_GEOMETRY_SHADER );
    addShader( sb.fragmentShader, Shader::TYPE_FRAGMENT_SHADER );
    addShader( sb.computeShader, Shader::TYPE_COMPUTE_SHADER );

    auto ret = createProgram( mPrograms[sb.name], "", "" );
    return ret;
}

void ShaderManager::init( const std::string& cacheFolder, const std::string& cacheLabel ) {
    // This will clear all the handles for all the shaders
    for ( auto& s : mVertexShaders ) s.second->clear();
    for ( auto& s : mTesselationControlShaders ) s.second->clear();
    for ( auto& s : mTesselationEvaluationShaders ) s.second->clear();
    for ( auto& s : mGeometryShaders ) s.second->clear();
    for ( auto& s : mFragmentShaders ) s.second->clear();
    for ( auto& s : mComputeShaders ) s.second->clear();

    for ( auto& s : mPrograms ) s.second->clear();

    // We now need to compile the shaders
    LOGR( "Compiling %d programs", mPrograms.size());
    for ( ProgramMap::iterator it = mPrograms.begin(); it != mPrograms.end(); ++it ) {
        createProgram( it->second, cacheFolder, cacheLabel );
    }
}

void ShaderManager::addToShaderList( const std::vector<std::string>& shadersFileNames, Shader::Type st ) {
    for ( auto& vsh : shadersFileNames ) {
        size_t extPos = vsh.find_last_of( "." );
        ASSERT( extPos != std::string::npos );
        std::string vshName = vsh.substr( 0, extPos );
        ShaderList::addShader( ShaderSource( vshName, vsh, st ));
    }
}

bool ShaderManager::loadShaders() {

    createCCInjectionMap();

    if ( !loadProgram( ShaderBuilder{ S::WIREFRAME } ) ) return false;
    if ( !loadProgram( ShaderBuilder{ S::SKYBOX } )) return false;
    if ( !loadProgram( ShaderBuilder{ S::SHADOW_MAP } )) return false;
    if ( !loadProgram( ShaderBuilder{ S::FONT_2D }.vsh( "vertex_shader_2d_font" ).fsh( "plain_font" ))) return false;
    if ( !loadProgram( ShaderBuilder{ S::FONT }.vsh( "vertex_shader_3d_font" ).fsh( "plain_font" ))) return false;
    if ( !loadProgram(
            ShaderBuilder{ S::BLUR_HORIZONTAL }.vsh( "vertex_shader_blitcopy" ).fsh( "plain_blur_horizontal" ))) return false;
    if ( !loadProgram( ShaderBuilder{ S::BLUR_VERTICAL }.vsh( "vertex_shader_blitcopy" ).fsh( "plain_blur_vertical" ))) return false;
    if ( !loadProgram( ShaderBuilder{ S::FINAL_COMBINE }.vsh( "vertex_shader_blitcopy" ).fsh( "plain_final_combine" ))) return false;
    if ( !loadProgram( ShaderBuilder{ S::COLOR_2D }.vsh( "vertex_shader_2d_c" ).fsh( "fragment_shader_color" ))) return false;
    if ( !loadProgram( ShaderBuilder{ S::COLOR_3D }.vsh( "vertex_shader_3d_c" ).fsh( "fragment_shader_color" ))) return false;
    if ( !loadProgram( ShaderBuilder{ S::TEXTURE_2D }.vsh( "vertex_shader_2d_t" ).fsh( "fragment_shader_texture" ))) return false;
    if ( !loadProgram( ShaderBuilder{ S::TEXTURE_3D }.vsh( "vertex_shader_3d_t" ).fsh( "fragment_shader_texture" ))) return false;
    if ( !loadProgram( ShaderBuilder{ S::EQUIRECTANGULAR }.vsh( "equirectangular" ).fsh( "equirectangular" ))) return false;
    if ( !loadProgram( ShaderBuilder{ S::PLAIN_CUBEMAP }.vsh( "plain_cubemap" ).fsh( "plain_cubemap" ))) return false;
    if ( !loadProgram( ShaderBuilder{ S::CONVOLUTION }.vsh( "plain_cubemap" ).fsh( "irradiance_convolution" ))) return false;
    if ( !loadProgram( ShaderBuilder{ S::IBL_SPECULAR }.vsh( "plain_cubemap" ).fsh( "ibl_specular_prefilter" ))) return false;
    if ( !loadProgram( ShaderBuilder{ S::IBL_BRDF }.vsh( "vertex_shader_brdf" ).fsh( "plain_brdf" ))) return false;
    if ( !loadProgram( ShaderBuilder{ S::SH }.vsh( "vertex_shader_3d_sh" ).fsh( "plain_sh" ))) return false;
    if ( !loadProgram(
            ShaderBuilder{ S::SH_NOTEXTURE }.vsh( "vertex_shader_3d_sh_notexture" ).fsh( "plain_sh_notexture" ))) return false;

    mNumReloads++;
    return true;
}

std::shared_ptr<ProgramOpenGL> ShaderManager::P( const std::string& id ) const {
    ProgramMap::const_iterator it = mPrograms.find( id );

    if ( it != mPrograms.end()) {
        return it->second;
    }
    LOGE( "Could not find program: %s", id.c_str());
    ASSERT( false );
    return nullptr;
}

bool ShaderManager::createProgram( std::shared_ptr<ProgramOpenGL> program, const std::string& cacheFolder,
                                   const std::string& cacheLabel ) {
    auto vsIt = mVertexShaders.find( program->getVertexShaderId());
    auto tcsIt = mTesselationControlShaders.find( program->getTesselationControlShaderId());
    auto tesIt = mTesselationEvaluationShaders.find( program->getTesselationEvaluationShaderId());
    auto gsIt = mGeometryShaders.find( program->getGeometryShaderId());
    auto fsIt = mFragmentShaders.find( program->getFragmentShaderId());
    auto csIt = mComputeShaders.find( program->getComputeShaderId());

    auto ret =
    program->createOrUpdate( vsIt != mVertexShaders.end() ? vsIt->second : nullptr,
                             tcsIt != mTesselationControlShaders.end() ? tcsIt->second : nullptr,
                             tesIt != mTesselationEvaluationShaders.end() ? tesIt->second : nullptr,
                             gsIt != mGeometryShaders.end() ? gsIt->second : nullptr,
                             fsIt != mFragmentShaders.end() ? fsIt->second : nullptr,
                             csIt != mComputeShaders.end() ? csIt->second : nullptr,
                             cacheFolder, cacheLabel, CreateUpdateFlag::Create );

    if ( ret ) {
        mProgramsIds.push_back( program );
        return true;
    }

    return false;
}

int ShaderManager::getProgramCount() const {
    return static_cast<int>( mPrograms.size());
}

void ShaderManager::setCacheData( const std::string& cacheFolder, const std::string& cacheLabel ) {
    mCacheFolder = cacheFolder;
    mCacheLabel = cacheLabel;
}

const std::vector<std::shared_ptr<ProgramOpenGL>>& ShaderManager::Programs() const { return mProgramsIds; }
