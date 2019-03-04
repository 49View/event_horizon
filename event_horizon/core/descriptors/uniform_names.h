#pragma once

#include <string>

namespace UniformNames {

	const std::string mvpMatrix = "u_mvpMatrix";
	const std::string mvpMatrixDepth = "u_mvpMatrixDepth";
	const std::string mvpMatrixDepthBias = "u_depthBiasMVP";
	const std::string mvpShadowMap = "u_mvpShadowMap";
	const std::string modelMatrix = "u_modelMatrix";
	const std::string viewMatrix = "u_viewMatrix";
	const std::string screenSpaceMatrix = "u_screenSpaceMatrix";
	const std::string projMatrix = "u_projMatrix";
	const std::string sunDirection = "u_sunDirection";
	const std::string sunPosition = "u_sunPosition";
	const std::string sunRadiance = "u_sunRadiance";

	const std::string pointLightPos = "u_pointLightPos";
	const std::string pointLightDir = "u_pointLightDir";
	const std::string pointLightIntensity = "u_pointLightIntensity";
	const std::string pointLightAttenuation = "u_pointLightAttenuation";
	const std::string spotLightBeamDir = "u_spotLightBeamDir";
	const std::string outerCutOff = "u_outerCutOff";
	const std::string lightType = "u_lightType";
	const std::string numPointLights = "u_numPointLights";

	const std::string deltaAnimTime = "u_deltaAnimTime";

	const std::string eyeDir = "u_eyeDir";
	const std::string eyePos = "u_eyePos";
	const std::string timeOfTheDay = "u_timeOfTheDay";
	const std::string sunHRDMult = "u_sunHDRMult";

	const std::string interpolateDayNightDelta = "u_interpolateDayNightDelta";
	const std::string colorTexture = "colorTexture";
	const std::string yTexture = "yTexture";
	const std::string uTexture = "uTexture";
	const std::string vTexture = "vTexture";
	const std::string shadowMapTexture = "shadowMapTexture";
	const std::string colorFBTexture = "colorFBTexture";
	const std::string bloomTexture = "bloomTexture";
	const std::string aoTexture = "aoTexture";
	const std::string diffuseTexture = "diffuseTexture";
	const std::string normalTexture = "normalTexture";
	const std::string roughnessTexture = "roughnessTexture";
	const std::string metallicTexture = "metallicTexture";
	const std::string heightTexture = "heightTexture";
	const std::string lightmapTexture = "lightmapTexture";
	const std::string cubeMapTexture = "cubeMapTexture";
	const std::string ibl_irradianceMap = "ibl_irradianceMap";
	const std::string ibl_specularMap = "ibl_specularMap";
	const std::string ibl_brdfLUTMap = "ibl_brdfLUTMap";
	const std::string reflectionTexture = "reflectionTexture";
	const std::string alpha = "alpha";
	const std::string ui_z = "ui_z";
	const std::string clip_plane = "clip_plane";
	const std::string screenSizeInv = "screenSizeInv";
	const std::string reflectionScreenSizeInv = "reflectionScreenSizeInv";

	const std::string diffuseColor = "diffuseColor";
	const std::string ambient = "ambient";
	const std::string incandescenceColor = "incandescenceColor";
	const std::string incandescenceFactor = "incandescenceFactor";
	const std::string transparency = "transparency";
	const std::string opacity = "opacity";
	const std::string diffuse = "diffuse";

	const std::string shLightCoeffs = "shLightCoeffs";

	const std::string specularColor = "specularColor";
	const std::string cosinePower = "cosinePower";
	const std::string specular = "specular";
	const std::string reflectivity = "reflectivity";
	const std::string reflectedColor = "reflectedColor";
	const std::string metallic = "metallicV";
	const std::string roughness = "roughnessV";
	const std::string ao = "aoV";
};
