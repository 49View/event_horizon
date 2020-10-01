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
    constexpr static int CoreStart =         0;
    constexpr static int CoreEnd =         999;
    constexpr static int UnsortedStart =  1000;
    constexpr static int UnsortedCustom = 1499;
    constexpr static int UnsortedEnd =    1999;
    constexpr static int UIStart   =      2000;
    constexpr static int UIEnd     =      9999;
    constexpr static int PBRStart  =     10000;
    constexpr static int PBREnd    =  PBRStart+ 10000000;
    constexpr static int PBRStartUnsorted  =  PBREnd+1;
    constexpr static int PBREndUnsorted    =  PBRStartUnsorted+10000000;
    constexpr static int PBRStartFar  =  PBREndUnsorted+1;
    constexpr static int PBREndFar    =  PBRStartFar + 10000000;
    constexpr static int PBRStartFarUnsorted  =  PBREndFar+1;
    constexpr static int PBREndFarUnsorted    =  PBRStartFarUnsorted + 10000000;
    constexpr static int UI2dStart = PBREndFarUnsorted+1;
    constexpr static int CameraLocatorIM         = UI2dStart+1;
    constexpr static int CameraMousePointers     = UI2dStart+2;
    constexpr static int UI2dEnd   = UI2dStart + 999999;
    constexpr static int GridStart = UI2dEnd+1;
    constexpr static int GridEnd   = GridStart+10000;
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
