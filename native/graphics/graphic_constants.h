#pragma once

#include <string>
#include <memory>
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
    UF_SAMPLER_3D,
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

enum ReservedGraphicsTags {
	GT_None = 0,
	GT_Generic = 1,
    GT_SkyboxCubeEnvMap,
    GT_PrefilterSpecular,
    GT_PrefilterBRDF,
	GT_Grid,
};

class Framebuffer;

enum class BlitType {
    OnScreen,
    OffScreen
};

using cubeMapFrameBuffers = std::vector<std::shared_ptr<Framebuffer>>;

struct TextureUniformDesc {
    unsigned int handle;
    unsigned int slot;
    int 		 target;
};
