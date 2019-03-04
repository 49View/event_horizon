#pragma once

#include <string>
#include <vector>
#include "core/image_constants.h"

enum UniformFormat {
	UF_FLOAT,
	UF_BOOL,
	UF_INT,
	UF_VEC2F,
	UF_VEC3F,
	UF_VEC4F,
	UF_VEC2I,
	UF_VEC3I,
	UF_VEC4I,
	UF_VEC2B,
	UF_VEC3B,
	UF_VEC4B,
	UF_MAT2F,
	UF_MAT3F,
	UF_MAT4F,
	UF_SAMPLER_2D,
	UF_SAMPLER_CUBE,
	UF_SAMPLER_2D_SHADOW,
};

enum CullMode {
	CULL_NONE = 0,
	CULL_FRONT,
	CULL_BACK,
	CULL_FRONT_AND_BACK,
};

enum DepthFunction {
	GREATER,
	GEQUAL,
	LESS,
	LEQUAL
};

enum TextureProcessing {
	TEXTURE_PROCESSING_NONE,
	TEXTURE_PROCESSING_QUADRANTS
};

enum TextureSlots {
	TSLOT_COLOR = 0,
	TSLOT_IBL_IRRADIANCE,
	TSLOT_IBL_PREFILTER,
	TSLOT_IBL_BRDFLUT,
	TSLOT_NORMAL,
	TSLOT_SHADOWMAP,
	TSLOT_BLOOM,
	TSLOT_U_YUV,
	TSLOT_V_YUV,
	TSLOT_AO,
	TSLOT_ROUGHNESS,
	TSLOT_METALLIC,
	TSLOT_HEIGHT,
	TSLOT_CUBEMAP,
	TSLOT_LIGHTMAP,
};

enum class GridAxis {
	All,
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight
};

static const std::string staticvpss = "static";

enum CommandListType {
	vpPre2d,
	vp2d,
	vpPost2d,
	vp2dExtraDebug,
	vpUI2d,
	vpUI3d,
	vpUI2dTransparent,
	vpUI3dTransparent,
	vp3d,
	vp3dTransparent,
	vp3dShadowMap
};

enum class CameraProjectionType {
	Perspective,
	Orthogonal
};

enum CameraMode {
	Edit2d = 0,
	Doom,
	CameraMode_max
};

enum CameraState {
	Active,
	InActive
};

enum CubemapFaces {
	Front = 0,
	Back,
	Left,
	Right,
	Top,
	Bottom
};

enum ReservedGraphicsTags {
	GT_None = 0,
	GT_Generic = 1,
    GT_SkyboxCubeEnvMap,
    GT_PrefilterSpecular,
    GT_PrefilterBRDF,
	GT_Grid,
};

namespace ViewportToggles {
	const static int None          		= 0;
	const static int DrawWireframe 		= 1 << 0;
	const static int DrawGrid      		= 1 << 1;
}

using ViewportTogglesT = int;

std::string cubeMapFace( const std::string& filename, CubemapFaces cf );
std::string cubeMapTName( const std::string& filename );

class Framebuffer;
using cubeMapFrameBuffers = std::vector<std::shared_ptr<Framebuffer>>;
