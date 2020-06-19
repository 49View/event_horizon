//
// Created by Dado on 2019-05-27.
//

#pragma once

#include <memory>
#include <unordered_set>
#include <core/names.hpp>

class Renderer;
class RenderMaterial;
class ShaderManager;
class RenderLightManager;
class RenderOrchestrator;
class StreamingMediator;
class RLTarget;
class ProgramUniformSet;
class Program;
class CommandBufferEntry;
class CommandBufferList;
struct CommandBufferListVector;
class ShaderMaterial;
class RenderMaterialManager;
struct cpuVBIB;
class GPUVData;
class GPUVDataManager;
class VPList;
using VPListSP = std::shared_ptr<VPList>;

namespace CommandBufferLimits {
    const static int CoreStart =         0;
    const static int CoreEnd =         999;
    const static int UnsortedStart =  1000;
    const static int UnsortedEnd =    1999;
    const static int UIStart   =      2000;
    const static int UIEnd     =      9999;
    const static int PBRStart  =     10000;
    const static int PBREnd    =  99999999;
    const static int UI2dStart = 100000000;
    const static int CameraLocator =     UI2dStart+1;
    const static int UI2dEnd   = 100999999;
    const static int GridStart = 101000000;
    const static int GridEnd   = 101010000;
}

namespace FBNames {

    const static std::string shadowmap                  = S::shadowmap                ;
    const static std::string depthmap                   = S::depthmap                 ;
    const static std::string normalmap                  = S::normalmap                ;
    const static std::string ssaomap                    = S::ssaomap                  ;
    const static std::string lightmap                   = S::lightmap                 ;
    const static std::string sceneprobe                 = S::sceneprobe               ;
    const static std::string blur_horizontal            = S::blur_horizontal          ;
    const static std::string blur_vertical              = S::blur_vertical            ;
    const static std::string colorFinalFrameBuffer      = S::colorFinalFrameBuffer    ;
    const static std::string offScreenFinalFrameBuffer  = S::offScreenFinalFrameBuffer;

    static std::unordered_set<std::string> mFBNames;

    bool isPartOf( const std::string& _val );
}
