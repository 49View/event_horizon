#include <regex>

#include "shader_manager_opengl.h"
#include "shaders.hpp"
#include "../shader_list.h"
#include "core/zlib_util.h"
#include "core/tar_util.h"
#include "core/http/basen.hpp"

ShaderManager::ShaderManager() {

//    std::string loadingScreen = R"(#version #opengl_version
//
//    #include "animation_uniforms.glsl"
//    in vec2 v_texCoord;
//    out vec4 FragColor;
//
//    void main() {
//
//        float iTime = u_deltaAnimTime.y;
//        vec2 iResolution = vec2(1280.0, 720.0);
//
//        // Loading Screen
//        vec3 col = vec3(.2,.2,.2);
//
//        vec2 p = (-1.0+2.0*v_texCoord)*vec2(iResolution.x/iResolution.y,1.0);
//        vec2 q = p + 0.2;
//
//        q *= 1.0 + 0.3*q.x;
//        float r = length(q);
//        float a = atan(q.y,q.x) + 1.0*iTime;
//        col = mix( col, vec3(0.1), (1.0-smoothstep( 0.0,0.24,abs(r-0.5)))*smoothstep(-1.0,1.0,sin(17.0*a))   );
//
//        p *= 1.0 + 0.3*p.x;
//        r = length(p);
//        a = atan(p.y,p.x) + 1.0*iTime;
//        col = mix( col, vec3(1.0,1.0,1.0), (1.0-smoothstep( 0.10,0.14,abs(r-0.5)))*smoothstep(0.4,1.0,sin(17.0*a))   );
//
//        float gl = 0.6 + 0.4*sin(0.5*6.2831*iTime);
//        col += gl * vec3(1.0,0.5,0.2) * (1.0-smoothstep( 0.10,0.20,abs(r-0.5)))*smoothstep(-1.0,0.5,sin(17.0*a))   ;
//
//        FragColor = vec4(col,1.0);
//    }
//    )";

//std::string loadingScreen = R"(#version #opengl_version
///* ----------------
//*	zlnimda wrote this file and is under license CC-BY-SA-4.0
//* 	( see legal notice: https://creativecommons.org/licenses/by-sa/4.0/legalcode )
///* ----------------
//* 	Animation of loading
//*/
//#include "animation_uniforms.glsl"
//in vec2 v_texCoord;
//out vec4 fragColor;
//vec2 iResolution = vec2(640.0, 360.0);
//
//// AA
//#define	LOW_RPX	(4./max(iResolution.x,iResolution.y))
//
//#define RATIO	(iResolution.x/iResolution.y)
//#define FONT_R	(16.)
//#define FONT_S	(1./FONT_R)
//
//const float M_PI = 3.1415926535897932384626433832795;
//
//const vec4	YELOW  = vec4(vec3(0xD3, 0xE7, 0x14) / vec3(0xFF), 1.);
//const vec4 	RED    = vec4(vec3(0xFF, 0x38, 0x67) / vec3(0xFF), 1.);
//const vec4	BLUE   = vec4(vec3(0x46, 0x2D, 0xFE) / vec3(0xFF), 1.);
//const vec4	CYAN   = vec4(vec3(0x09, 0xDE, 0xAF) / vec3(0xFF), 1.);
//const vec4 	WHITE  = vec4(1.);
//const vec4 	BBLACK = vec4(0., 0., 0., 1.);
//
//
//vec2 rot(vec2 uv, float a)
//{
//    return (mat2(cos(a), -sin(a), sin(a), cos(a)) * mat2(RATIO, 0., 0., 1.)) * uv;
//}
//
//// almost signed dist sq
//vec2 sdsq(vec2 uv, vec2 o, vec2 s, float a)
//{
//    // handle angle & ratio
//    uv = rot(o - uv, a);
//    return s/2.0 - abs(uv);
//}
//
//
//// almost signed dist sp
//float sdsp(vec2 uv, vec2 o, float s)
//{
//    // handle angle & ratio
//    uv = o - uv;
//    return length(uv) - s*.5;
//}
//
//// sq color
//vec4 sq(vec2 uv, vec2 o, vec2 s, vec4 c, float a)
//{
//    // AA
//    s *= 1.+LOW_RPX;
//    vec2 d = sdsq(uv, o, s, a);
//    // AA
//    float dd = min(d.x, d.y);
//    vec3 rc = mix(vec3(.0), c.xyz,
//               // step(.0, dd)
//                clamp(smoothstep(.0, LOW_RPX, dd) / LOW_RPX, .0, 1.)
//              );
//    return vec4(rc, mix(.0, c.a, step(.0, dd)));
//}
//
//// sp color
//vec4 sp(vec2 uv, vec2 o, float s, vec4 c)
//{
//    // AA
//    s *= 1.+LOW_RPX;
//    float d = sdsp(uv, o, s);
//    //return mix(c, vec4(0.0), step(0.0, d));
//
//    // AA
//    vec3 rc = mix(c.xyz, vec3(.0),
//               // step(.0, dd)
//                clamp(smoothstep(.0, LOW_RPX, d) / LOW_RPX, .0, 1.)
//              );
//    return vec4(rc, mix(c.a, .0, step(.0, d)));
//}
//
//float BezierDeg4(float p0, float p1, float p2, float p3, float t)
//{
//    // bezier formula following bernstein Bez(t) = E(i->n)Bern(t).Pi with t=[0,1]
//    return p0*pow(1.-t, 3.) + p1*3.*t*pow(1.-t, 2.) + p2*3.*t*t*(1.-t) + p3*t*t*t;
//}
//
//float easingBezier(vec2 r, vec2 p0, vec2 p1, float t)
//{
//    t = BezierDeg4(0., p0.x, p1.x, 1., t);
//    return BezierDeg4(r.x, p0.y, p1.y, r.y, t);
//}
//
//float easingInOutBack(float t)
//{
//    return easingBezier(vec2(0., 1.), vec2(0.265, -0.55), vec2(0.68, 1.55), t);
//}
//
//vec4 anim(vec2 uv, float t, vec4 c, float i)
//{
//    float ta = t + i *0.03 * mix(-1., 1., step(.5, t));
//    ta = mix(1.-easingInOutBack(ta*2.),
//            easingBezier(vec2(0., 1.), vec2(0.1, 0.75), vec2(0.67, 3.6), (ta-.5)*2.),
//            step(.5, ta));
//    vec2 s = vec2(ta * .25, .015);
//    float e = step(1.8, ta);
//    return sq(uv, vec2(s.x*.5, s.y*i*4.)/RATIO, s, mix(WHITE, c, e), .0)
//        + e * sp(uv, vec2(.315, s.y*i*2.3), .011, c);
//}
//
//vec4 drawsq(vec2 uv, float t, vec4 c)
//{
//    vec4 ec;
//    ec += anim(uv, t, c, 0.);
//    ec += anim(uv, t, c, 1.);
//    ec += anim(uv, t, c, 2.);
//    return ec;
//}
//
//vec4 animDrawSq(vec2 uv, float t)
//{
//    vec4 c;
//    vec2 uvfk;
//    float r = easingInOutBack(t)+.125;
//
//    const vec2 off  = vec2(.0, .030);
//    const vec2 cent = vec2(.5, .55);
//    uvfk = rot(uv-cent + off, r *2.* M_PI) - off;
//    c += drawsq(uvfk, t, YELOW);
//    uvfk = rot(uv-cent + off, (r + 0.25) *2.* M_PI) - off;
//    c += drawsq(uvfk, t, CYAN);
//    uvfk = rot(uv-cent + off, (r + 0.50) *2.* M_PI) - off;
//    c += drawsq(uvfk, t, BLUE);
//    uvfk = rot(uv-cent + off, (r + 0.75) *2.* M_PI) - off;
//    c += drawsq(uvfk, t, RED);
//    return c;
//}
//
//float inRect(vec2 pos, vec2 topLeft, vec2 rightBottom) {
//	return step(topLeft.x, pos.x) * step(rightBottom.y, pos.y) * step(-rightBottom.x, -pos.x) * step(-topLeft.y, -pos.y);
//}
//
//float inBetween(float x, float a, float b) {
//    return step(a, x) * step(-b, -x);
//}
//
//vec3 loadingColor( vec2 uv, float progress ) {
//    vec2 inv_resolution = 1.0 / iResolution.xy;
//	float sWidth = iResolution.x * inv_resolution.y;
//    const float barWidthRatio = 0.7;
//    float inv_barWidth = 1.0 / (barWidthRatio * sWidth);
//    float barHeight = 0.0070;
//    float twice_inv_barHeight = 2.0 / barHeight;
//    uv.x = uv.x * sWidth;
//
//    mat3 T_bar2s = mat3(
//        vec3(inv_barWidth, 0.0, 0.0),
//        vec3(0.0, inv_barWidth, 0.0),
//        vec3((1.0 - sWidth * inv_barWidth) * 0.5, -0.15 * inv_barWidth, 1.0)
//    );
//
//    vec2 uv_bar = (T_bar2s * vec3(uv.xy, 1.0)).xy;
//    float isInBaseRect = inRect(uv_bar, vec2(0.0, 0.5 * barHeight), vec2(1.0, -0.5 * barHeight));
//    float isInActiveRect = inRect(uv_bar, vec2(0.0, 0.5 * barHeight), vec2(progress, -0.5 * barHeight));
//    vec3 baseColor = vec3(0.12941, 0.13725, 0.17647);
//    vec3 activeColor = mix(vec3(0.2, 0.35294, 0.91373), vec3(0.43529, 0.43529, 0.96078), uv_bar.x);
//    vec3 color = vec3(0.0, 0.0, 0.0);
//    color = mix(color, baseColor, isInBaseRect);
//    color = mix(color, activeColor, isInActiveRect);
//
//	return color;
//}
//
//void main() {
//
//    float iTime = u_deltaAnimTime.y;
//    float progress = u_deltaAnimTime.w;
//    vec2 uv = v_texCoord;
//
//    float t = mod(iTime, 2.)/2.;
//
//    vec4 c;
//
//    c += animDrawSq(uv, t);
//
//    const float stp = 4.;		// 20 step
//    const float dt = 15./60.; 	// 5 frames
//    float mm = 1.;
//    for (float it = 1.; it < stp; ++it)
//    {
//        float imp = it/stp;
//        imp = pow(1.-imp, 7.);
//    	c += animDrawSq(uv, t-dt*(it/stp)) * imp;
//        mm += imp * imp;
//    }
//    c /= mm*2.0;
//
//    float lowerLimit = 0.2;
//    float higherLimit = 1.0f - lowerLimit;
//    float alphaing = 1.0;
//
//    if ( progress < lowerLimit ) {
//    	alphaing = smoothstep(0.0f, lowerLimit, progress);
//    } else if ( progress > higherLimit ) {
//    	alphaing = 1.0 - smoothstep(higherLimit, 1.0f, progress);
//    }
//
//    c.xyz += loadingColor(uv, progress);
//    c *= alphaing;
//
//    fragColor = c;
//}
//)";
//
//    std::string loadingScreen = R"(#version #opengl_version
///* ----------------
//*	zlnimda wrote this file and is under license CC-BY-SA-4.0
//* 	( see legal notice: https://creativecommons.org/licenses/by-sa/4.0/legalcode )
///* ----------------
//* 	Animation of loading
//*/
//#include "animation_uniforms.glsl"
//in vec2 v_texCoord;
//out vec4 fragColor;
//vec2 iResolution = vec2(640.0, 360.0);
//
//float inRect(vec2 pos, vec2 topLeft, vec2 rightBottom) {
//	return step(topLeft.x, pos.x) * step(rightBottom.y, pos.y) * step(-rightBottom.x, -pos.x) * step(-topLeft.y, -pos.y);
//}
//
//float inBetween(float x, float a, float b) {
//    return step(a, x) * step(-b, -x);
//}
//
//vec3 loadingColor( vec2 uv, float progress ) {
//    vec2 inv_resolution = 1.0 / iResolution.xy;
//	float sWidth = iResolution.x * inv_resolution.y;
//    const float barWidthRatio = 0.7;
//    float inv_barWidth = 1.0 / (barWidthRatio * sWidth);
//    float barHeight = 0.0070;
//    float twice_inv_barHeight = 2.0 / barHeight;
//    uv.x = uv.x * sWidth;
//
//    mat3 T_bar2s = mat3(
//        vec3(inv_barWidth, 0.0, 0.0),
//        vec3(0.0, inv_barWidth, 0.0),
//        vec3((1.0 - sWidth * inv_barWidth) * 0.5, -0.48 * inv_barWidth, 1.0)
//    );
//
//    vec2 uv_bar = (T_bar2s * vec3(uv.xy, 1.0)).xy;
//    float isInBaseRect = inRect(uv_bar, vec2(0.0, 0.5 * barHeight), vec2(1.0, -0.5 * barHeight));
//    float isInActiveRect = inRect(uv_bar, vec2(0.0, 0.5 * barHeight), vec2(progress, -0.5 * barHeight));
//    vec3 baseColor = vec3(0.12941, 0.13725, 0.17647);
//    vec3 activeColor = mix(vec3(0.2, 0.35294, 0.91373), vec3(0.43529, 0.43529, 0.96078), uv_bar.x);
//    vec3 color = vec3(0.0, 0.0, 0.0);
//    color = mix(color, baseColor, isInBaseRect);
//    color = mix(color, activeColor, isInActiveRect);
//
//	return color;
//}
//
//void main() {
//
//    float iTime = u_deltaAnimTime.y;
//    float progress = u_deltaAnimTime.w;
//    vec2 uv = v_texCoord;
//
//    float lowerLimit = 0.2;
//    float higherLimit = 1.0f - lowerLimit;
//    float alphaing = progress < lowerLimit ? progress : 1.0;
//
//    if ( progress < lowerLimit ) {
//    	alphaing = smoothstep(0.0f, lowerLimit, progress);
//    } else if ( progress > higherLimit ) {
//    	alphaing = 1.0 - smoothstep(higherLimit, 1.0f, progress);
//    }
//
//    vec4 c = vec4(loadingColor(uv, progress), 0.0);
//    c *= alphaing;
//
//    fragColor = c;
//}
//)";

//    gShaderInjection["loading_screen.fsh"] = bn::encode_b64(loadingScreen);
    for ( const auto& [k,v] : gShaderInjection ) {
        shaderSourcesMap.insert( {k, v } );
    }

    createCCInjectionMap();

    allocateProgram( ShaderProgramDesc{ S::LOADING_SCREEN }.vsh( "vertex_shader_blitcopy" ).fsh( "loading_screen" ) );
    allocateProgram( ShaderProgramDesc{ S::WIREFRAME } );
    allocateProgram( ShaderProgramDesc{ S::SHADOW_MAP } );
    allocateProgram( ShaderProgramDesc{ S::NORMAL_MAP } );
    allocateProgram( ShaderProgramDesc{ S::DEPTH_MAP }.vsh( "vertex_shader_3d_c" ).fsh( "depthmap" ) );
    allocateProgram( ShaderProgramDesc{ S::SKYBOX } );
    allocateProgram( ShaderProgramDesc{ S::FONT_2D }.vsh( "vertex_shader_2d_font" ).fsh( "plain_font" ));
    allocateProgram( ShaderProgramDesc{ S::FONT }.vsh( "vertex_shader_3d_font" ).fsh( "plain_font" ));
    allocateProgram( ShaderProgramDesc{ S::BLUR_HORIZONTAL }.vsh( "vertex_shader_blitcopy" ).fsh( "plain_blur_horizontal" ));
    allocateProgram( ShaderProgramDesc{ S::BLUR_VERTICAL }.vsh( "vertex_shader_blitcopy" ).fsh( "plain_blur_vertical" ));
    allocateProgram( ShaderProgramDesc{ S::FINAL_COMBINE }.vsh( "vertex_shader_blitcopy_viewspace" ).fsh( "plain_final_combine" ));
    allocateProgram( ShaderProgramDesc{ S::SSAO }.vsh( "vertex_shader_blitcopy_viewspace" ).fsh( "ssao" ));
    allocateProgram( ShaderProgramDesc{ S::COLOR_2D }.vsh( "vertex_shader_2d_c" ).fsh( "fragment_shader_color" ));
    allocateProgram( ShaderProgramDesc{ S::COLOR_3D }.vsh( "vertex_shader_3d_c" ).fsh( "fragment_shader_color" ));
    allocateProgram( ShaderProgramDesc{ S::TEXTURE_2D }.vsh( "vertex_shader_2d_t" ).fsh( "fragment_shader_texture" ));
    allocateProgram( ShaderProgramDesc{ S::TEXTURE_3D }.vsh( "vertex_shader_3d_t" ).fsh( "fragment_shader_texture" ));
    allocateProgram( ShaderProgramDesc{ S::YUV_GREENSCREEN }.vsh( "vertex_shader_3d_t" ).fsh( "plain_yuv_to_rgb_greenscreen" ));
    allocateProgram( ShaderProgramDesc{ S::EQUIRECTANGULAR }.vsh( "equirectangular" ).fsh( "equirectangular" ));
    allocateProgram( ShaderProgramDesc{ S::PLAIN_CUBEMAP }.vsh( "plain_cubemap" ).fsh( "plain_cubemap" ));
    allocateProgram( ShaderProgramDesc{ S::SKYBOX_CUBEMAP }.vsh( "skybox_cubemap" ).fsh( "plain_cubemap" ));
    allocateProgram( ShaderProgramDesc{ S::CONVOLUTION }.vsh( "plain_cubemap" ).fsh( "irradiance_convolution" ));
    allocateProgram( ShaderProgramDesc{ S::IBL_SPECULAR }.vsh( "plain_cubemap" ).fsh( "ibl_specular_prefilter" ));
    allocateProgram( ShaderProgramDesc{ S::IBL_BRDF }.vsh( "vertex_shader_brdf" ).fsh( "plain_brdf" ));
    allocateProgram( ShaderProgramDesc{ S::SH }.vsh( "vertex_shader_3d_sh" ).fsh( "plain_sh" ));
    allocateProgram( ShaderProgramDesc{ S::SH_NOTEXTURE }.vsh( "vertex_shader_3d_sh" ).fsh( "plain_sh_notexture" ));
    allocateProgram( ShaderProgramDesc{ S::SH_DIRECT_LIGHTING }.vsh( "vertex_shader_3d_sh" ).fsh( "plain_sh_direct_lighting" ));
}

void ShaderManager::allocateProgram( const ShaderProgramDesc& _pd ) {
    programDescs.emplace_back( _pd );

    allocateShader( _pd.vertexShader, Shader::TYPE_VERTEX_SHADER );
    allocateShader( _pd.tessControlShader, Shader::TYPE_TESSELATION_CONTROL_SHADER );
    allocateShader( _pd.tessEvaluationShader, Shader::TYPE_TESSELATION_EVALUATION_SHADER );
    allocateShader( _pd.geometryShader, Shader::TYPE_GEOMETRY_SHADER );
    allocateShader( _pd.fragmentShader, Shader::TYPE_FRAGMENT_SHADER );
    allocateShader( _pd.computeShader, Shader::TYPE_COMPUTE_SHADER );
}


std::string ShaderManager::injectIncludes( std::string& sm ) {
    size_t includeFind = 0;
    while (( includeFind = sm.find( "#include" )) != std::string::npos ) {
        auto fi = sm.find_first_of( '"', includeFind );
        auto fe = sm.find_first_of( '"', fi + 1 );
        auto filename = sm.substr( fi + 1, fe - fi - 1 );
        filename = string_trim_upto( filename, "." );

        sm.erase( sm.begin() + includeFind, sm.begin() + fe + 1 );
        sm.insert( includeFind, bn::decode_b64(shaderSourcesMap[filename]) );
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

    size_t fi = 0;
    for ( auto& w : mDefineMap ) {
        std::istringstream smStream(sm);
        size_t numCharRead = 0;
        for (std::string line; std::getline(smStream, line); ) {
            numCharRead += line.size();
            if (( fi = line.find( w.first )) != std::string::npos ) {
                size_t fe = line.find( "//" );
                if ( fe == std::string::npos || fe > fi ) {
                    fi = sm.find( w.first, numCharRead );
                    fe = fi + w.first.size();
                    sm.erase( sm.begin() + fi, sm.begin() + fe + 1 );
                    sm.insert( fi, w.second );
                }
            }
        }
    }

    return sm;
}

std::string ShaderManager::openFileWithIncludeParsing( const std::string& filename ) {
    std::string sb = bn::decode_b64(shaderSourcesMap[filename]);
//    FileManager::writeLocalFile( filename, sb.c_str(), sb.length(), true );
    std::string includeResolved = injectIncludes( sb );
    parsePreprocessorMacro( includeResolved );
    return injectPreprocessorMacro( includeResolved );
}

void ShaderManager::allocateShader( const std::string& id, Shader::Type stype ) {
    //LOGI("Adding vertex shader: %s", id);
    if ( id.empty() ) return;

    switch ( stype ) {
        case Shader::TYPE_VERTEX_SHADER:
            if ( mVertexShaders.find( id ) == mVertexShaders.end() ) {
                mVertexShaders[id] = std::make_shared<Shader>( stype, id);
            }
            break;
        case Shader::TYPE_TESSELATION_CONTROL_SHADER:
            if ( mTesselationControlShaders.find( id ) == mTesselationControlShaders.end() ) {
                mTesselationControlShaders[id] = std::make_shared<Shader>( stype, id);
            }
            break;
        case Shader::TYPE_TESSELATION_EVALUATION_SHADER:
            if ( mTesselationEvaluationShaders.find( id ) == mTesselationEvaluationShaders.end() ) {
                mTesselationEvaluationShaders[id] = std::make_shared<Shader>( stype, id);
            }
            break;
        case Shader::TYPE_GEOMETRY_SHADER:
            if ( mGeometryShaders.find( id ) == mGeometryShaders.end() ) {
                mGeometryShaders[id] = std::make_shared<Shader>( stype, id);
            }
            break;
        case Shader::TYPE_FRAGMENT_SHADER:
            if ( mFragmentShaders.find( id ) == mFragmentShaders.end() ) {
                mFragmentShaders[id] = std::make_shared<Shader>( stype, id);
            }
            break;
        case Shader::TYPE_COMPUTE_SHADER:
            if ( mComputeShaders.find( id ) == mComputeShaders.end() ) {
                mComputeShaders[id] = std::make_shared<Shader>( stype, id);
            }
            break;
        default:
            break;
    }
}

bool ShaderManager::addShader( const std::string& id, Shader::Type stype ) {
    //LOGI("Adding vertex shader: %s", id);
    if ( id.empty() ) return true;

    std::shared_ptr<Shader> sid = nullptr;

    switch ( stype ) {
        case Shader::TYPE_VERTEX_SHADER:
            mVertexShaders[id]->setSource( openFileWithIncludeParsing( id + ".vsh" ) );
            sid = mVertexShaders[id];
            break;
        case Shader::TYPE_TESSELATION_CONTROL_SHADER:
            mTesselationControlShaders[id]->setSource( openFileWithIncludeParsing( id + ".tch" ) );
            sid =  mTesselationControlShaders[id];
            break;
        case Shader::TYPE_TESSELATION_EVALUATION_SHADER:
            mTesselationEvaluationShaders[id]->setSource(  openFileWithIncludeParsing( id + ".teh" ) );
            sid = mTesselationEvaluationShaders[id];
            break;
        case Shader::TYPE_GEOMETRY_SHADER:
            mGeometryShaders[id]->setSource( openFileWithIncludeParsing( id + ".gsh" ) );
            sid = mGeometryShaders[id];
            break;
        case Shader::TYPE_FRAGMENT_SHADER:
            mFragmentShaders[id]->setSource( openFileWithIncludeParsing( id + ".fsh" ) );
            sid = mFragmentShaders[id];
            break;
        case Shader::TYPE_COMPUTE_SHADER:
            mComputeShaders[id]->setSource( openFileWithIncludeParsing( id + ".csh" ) );
            sid = mComputeShaders[id];
            break;
        default:
            return false;
    }

    return sid->compile();

}

std::shared_ptr<ProgramOpenGL> ShaderManager::initProgram( const ShaderProgramDesc& sb ) {
    if ( mPrograms.find( sb.name ) == mPrograms.end() ) {
        mPrograms[sb.name] = std::make_shared<ProgramOpenGL>( sb.name,
                                                              sb.vertexShader,
                                                              sb.tessControlShader,
                                                              sb.tessEvaluationShader,
                                                              sb.geometryShader,
                                                              sb.fragmentShader,
                                                              sb.computeShader );
    }

    return mPrograms[sb.name];
}

bool ShaderManager::loadProgram( const ShaderProgramDesc& sb ) {

    auto program = initProgram(sb);

    return program->createOrUpdate( vshForProgram(program),
                                    tchForProgram(program),
                                    tehForProgram(program),
                                    gshForProgram(program),
                                    fshForProgram(program),
                                    cshForProgram(program) );
}

bool ShaderManager::injectDefines( Shader::Type stype, const std::string& id, const std::string& _define, const std::string& _value ) {

    std::shared_ptr<Shader> sid = nullptr;
    switch ( stype ) {
        case Shader::TYPE_VERTEX_SHADER:
            sid = mVertexShaders[id];
            break;
        case Shader::TYPE_TESSELATION_CONTROL_SHADER:
            sid = mTesselationControlShaders[id];
            break;
        case Shader::TYPE_TESSELATION_EVALUATION_SHADER:
            sid = mTesselationEvaluationShaders[id];
            break;
        case Shader::TYPE_GEOMETRY_SHADER:
            sid = mGeometryShaders[id];
            break;
        case Shader::TYPE_FRAGMENT_SHADER:
            sid = mFragmentShaders[id];
            break;
        case Shader::TYPE_COMPUTE_SHADER:
            sid = mComputeShaders[id];
            break;
        default:
            return false;
            break;
    }
    sid->setPreprocessDefine( _define, _value);
    sid->compile();
    return true;
}

bool ShaderManager::injectShadersWithCode() {

    bool successfullChain = true;
    for ( const auto& shader : mVertexShaders ) {
        successfullChain = addShader( shader.first, Shader::TYPE_VERTEX_SHADER );
        if ( !successfullChain ) return false;
    }
    for ( const auto& shader : mTesselationControlShaders ) {
        successfullChain = addShader( shader.first, Shader::TYPE_TESSELATION_CONTROL_SHADER );
        if ( !successfullChain ) return false;
    }
    for ( const auto& shader : mTesselationEvaluationShaders ) {
        successfullChain = addShader( shader.first, Shader::TYPE_TESSELATION_EVALUATION_SHADER );
        if ( !successfullChain ) return false;
    }
    for ( const auto& shader : mGeometryShaders ) {
        successfullChain = addShader( shader.first, Shader::TYPE_GEOMETRY_SHADER );
        if ( !successfullChain ) return false;
    }
    for ( const auto& shader : mFragmentShaders ) {
        successfullChain = addShader( shader.first, Shader::TYPE_FRAGMENT_SHADER );
        if ( !successfullChain ) return false;
    }
    for ( const auto& shader : mComputeShaders ) {
        successfullChain = addShader( shader.first, Shader::TYPE_COMPUTE_SHADER );
        if ( !successfullChain ) return false;
    }
    return true;
}

bool ShaderManager::loadShaders( bool _performCompileOnly ) {

    bool compilationResult = injectShadersWithCode();

    if ( _performCompileOnly || !compilationResult ) return compilationResult;

    for ( const auto& pd : programDescs ) {
        compilationResult = loadProgram( pd );
        if ( !compilationResult ) return false;
    }

    mNumReloads++;
    return true;
}

void ShaderManager::touchProgram( const ShaderProgramDesc& sb ) {
    mProgramsToReload.emplace_back( sb );
}

bool ShaderManager::reloadDirtyPrograms() {
    if ( mProgramsToReload.empty() ) return false;

    for ( const auto& pd : mProgramsToReload ) {
        loadProgram( pd );
    }

    mProgramsToReload.clear();
    return true;
}

bool ShaderManager::injectDefine( const std::string& _shaderName, Shader::Type stype, const std::string& _id, const std::string& _define, const std::string& _value ) {

    bool compilationResult = injectDefines( stype, _id, _define, _value );

    if ( !compilationResult ) return false;

    for ( const auto& pd : programDescs ) {
        if ( pd.name.value == _shaderName ) {
            touchProgram(pd);
        }
    }

    return true;
}

std::shared_ptr<ProgramOpenGL> ShaderManager::P( const std::string& id ) const {
    auto it = mPrograms.find( id );

    if ( it != mPrograms.end()) {
        return it->second;
    }
    LOGE( "Could not find program: %s", id.c_str());
    ASSERT( false );
    return nullptr;
}

std::shared_ptr<Shader> ShaderManager::vshForProgram( std::shared_ptr<ProgramOpenGL> program ) {
    auto vsIt = mVertexShaders.find( program->getVertexShaderId());
    return vsIt != mVertexShaders.end() ? vsIt->second : nullptr;
}

std::shared_ptr<Shader> ShaderManager::tchForProgram( std::shared_ptr<ProgramOpenGL> program ) {
    auto vsIt = mTesselationControlShaders.find( program->getTesselationControlShaderId());
    return vsIt != mTesselationControlShaders.end() ? vsIt->second : nullptr;
}

std::shared_ptr<Shader> ShaderManager::tehForProgram( std::shared_ptr<ProgramOpenGL> program ) {
    auto vsIt = mTesselationEvaluationShaders.find( program->getTesselationEvaluationShaderId());
    return vsIt != mTesselationEvaluationShaders.end() ? vsIt->second : nullptr;
}

std::shared_ptr<Shader> ShaderManager::gshForProgram( std::shared_ptr<ProgramOpenGL> program ) {
    auto vsIt = mGeometryShaders.find( program->getGeometryShaderId());
    return vsIt != mGeometryShaders.end() ? vsIt->second : nullptr;
}

std::shared_ptr<Shader> ShaderManager::fshForProgram( std::shared_ptr<ProgramOpenGL> program ) {
    auto vsIt = mFragmentShaders.find( program->getFragmentShaderId());
    return vsIt != mFragmentShaders.end() ? vsIt->second : nullptr;
}

std::shared_ptr<Shader> ShaderManager::cshForProgram( std::shared_ptr<ProgramOpenGL> program ) {
    auto vsIt = mComputeShaders.find( program->getComputeShaderId());
    return vsIt != mComputeShaders.end() ? vsIt->second : nullptr;
}

int ShaderManager::getProgramCount() const {
    return static_cast<int>( mPrograms.size());
}

std::vector<GLuint> ShaderManager::ProgramsHandles() const {
    std::vector<GLuint> ret;

    for ( const auto& [k,v] : mPrograms ) {
        ret.emplace_back(v->handle());
    }

    return ret;
}



