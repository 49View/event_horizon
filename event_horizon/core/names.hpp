//
// Created by Dado on 2019-02-04.
//
#pragma once

#include <string>

namespace S {

    const std::string COLOR_2D = "PN_2D_COLOR";
    const std::string TEXTURE_2D = "PN_2D_TEXTURE";

    const std::string COLOR_3D = "PN_3D_COLOR";
    const std::string TEXTURE_3D = "PN_3D_TEXTURE";

    const std::string YUV = "PN_YUV";
    const std::string YUV_GREENSCREEN = "PN_YUV_GREENSCREEN";

    const std::string PLAIN_CUBEMAP = "PN_PLAIN_CUBEMAP";
    const std::string SKYBOX_CUBEMAP = "SKYBOX_CUBEMAP";
    const std::string EQUIRECTANGULAR = "PN_EQUIRECTANGULAR";
    const std::string CONVOLUTION = "PN_PLAIN_CONVOLUTION";
    const std::string IBL_SPECULAR = "IBL_SPECULAR";
    const std::string IBL_BRDF = "IBL_BRDF";
    const std::string SKYBOX = "skybox";
    const std::string LOADING_SCREEN = "loading_screen";
    const std::string WIREFRAME = "wireframe";
    const std::string FONT_2D = "PN_2D_FONT";
    const std::string FONT = "PN_FONT";

    const std::string SH = "PN_SH";
    const std::string SH_NOTEXTURE = "PN_SH_NOTEXTURE";

    const std::string SHOWNORMAL = "PN_SHOWNORMAL";
    const std::string BLUR_HORIZONTAL = "PN_BLUR_HORIZONTAL";
    const std::string BLUR_VERTICAL = "PN_BLUR_VERTICAL";
    const std::string FINAL_COMBINE = "PN_FINAL_COMBINE";
    const std::string SSAO = "PN_SSAO";
    const std::string SHADOW_MAP = "shadowmap";
    const std::string DEPTH_MAP = "depthmap";
    const std::string NORMAL_MAP = "normalmap_buffer";

    const std::string WHITE = "white";
    const std::string BLACK = "black";
    const std::string NORMAL = "normal";
    const std::string NOISE4x4 = "noise4x4";
    const std::string DEBUG_UV = "debug_uv";
    const std::string LUT_3D_TEST = "lut3dTexture";

    const std::string WHITE_PBR = "white_pbr";
    const std::string SQUARE = "square";

    const std::string DEFAULT_FONT = "amaranth";

    const std::string shadowmap = "shadowMap_d";
    const std::string depthmap = "depthmap_d";
    const std::string normalmap = "normalmap_n";
    const std::string ssaomap = "ssaomap_n";
    const std::string lightmap = "lightMap_t";
    const std::string sceneprobe = "sceneprobe";
    const std::string blur_horizontal = "blur_horizontal_b";
    const std::string blur_vertical = "blur_vertical_b";
    const std::string colorFinalFrameBuffer = "colorFinalFrameBuffer";
    const std::string offScreenFinalFrameBuffer = "offScreenFinalFrameBuffer";

};
