#include "light_manager.h"

#include <core/math/spherical_harmonics.h>
#include <core/descriptors/uniform_names.h>
#include <graphics/shadowmap_manager.h>
#include <graphics/program_uniform_set.h>

LightManager::LightManager() {
    mbGlobalOnOffSwitch = true;
    mDirectionalLightIntensity = std::make_shared<AnimType<float>>( 1.0f, "LightDirectionalIntensity" );

    // Preallocating and gnerating hemisphere kernel size
    for (int i = 0; i < mNumHemiKernelSize; ++i) {
        V3f sample{ signedUnitRand(), signedUnitRand(), unitRand() };
        sample = normalize(sample);
        // Distribuite within the hemisphere not just on the surface
        sample *= unitRand();
        // Accelerate toward center so we have a more meaningful distribution
        float scale = static_cast<float>(i) / static_cast<float>(mNumHemiKernelSize);
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        mHemisphereKernelSamples.emplace_back(sample);
    }

    // Lights UBO
    mLigthingUniform = std::make_unique<ProgramUniformSet>();

    mLigthingUniform->setUBOStructure( UniformNames::pointLightPos, ( 16 * mMaxLights ) );
    mLigthingUniform->setUBOStructure( UniformNames::pointLightDir, ( 16 * mMaxLights ) );
    mLigthingUniform->setUBOStructure( UniformNames::pointLightIntensity, ( 16 * mMaxLights ) );
    mLigthingUniform->setUBOStructure( UniformNames::pointLightAttenuation, ( 16 * mMaxLights ) );
    mLigthingUniform->setUBOStructure( UniformNames::spotLightBeamDir, ( 16 * mMaxLights ) );
    mLigthingUniform->setUBOStructure( UniformNames::outerCutOff, ( 16 * mMaxLights ) );
    mLigthingUniform->setUBOStructure( UniformNames::lightType, ( 16 * mMaxLights ) );
    mLigthingUniform->setUBOStructure( UniformNames::sunDirection, 16 );
    mLigthingUniform->setUBOStructure( UniformNames::sunPosition, 16 );
    mLigthingUniform->setUBOStructure( UniformNames::sunRadiance, 16 );
    mLigthingUniform->setUBOStructure( UniformNames::mvpShadowMap, 64 );
    mLigthingUniform->setUBOStructure( UniformNames::mvpMatrixDepthBias, 64 );
    mLigthingUniform->setUBOStructure( UniformNames::numPointLights, 16 );
    mLigthingUniform->setUBOStructure( UniformNames::hdrExposures, 16 );
    mLigthingUniform->setUBOStructure( UniformNames::shadowParameters, 16 );
    mLigthingUniform->setUBOStructure( UniformNames::shLightCoeffs, 48 );
    mLigthingUniform->setUBOStructure( UniformNames::hemisphereKernelSize, 16 );
    mLigthingUniform->setUBOStructure( UniformNames::hemisphereKernel, 16 * mNumHemiKernelSize );
    mLigthingUniform->setUBOStructure( UniformNames::ssaoParameters, 16 );
}

void LightManager::generateUBO( std::shared_ptr<ShaderManager> sm ) {
    mLigthingUniform->generateUBO( sm, "LightingUniforms");
}

void LightManager::addPointLight( const Vector3f& pos, float _wattage, float intensity, const Vector3f& attenuation ) {
    mPointLights.emplace_back( pos, _wattage, intensity, attenuation );
}

void LightManager::removePointLight( const size_t index ) {
    ASSERT( index < mPointLights.size());
    mPointLights.erase( mPointLights.begin() + index );
}

void LightManager::removeAllPointLights() {
    mPointLights.clear();
}

void LightManager::switchLightsOn( float animTime, TimelineGroupCCF _ccf ) {
    for ( auto& pl : mPointLights ) {
        Timeline::play( pl.IntensityAnim(), 2, KeyFramePair{animTime, 1.0f }, _ccf );
    }
//    animateTo( mDirectionalLightIntensity, 1.0f, animTime );
}

void LightManager::setPointLightWattages( float _watt ) {
    for ( auto& pl : mPointLights ) {
        pl.setWattage( _watt );
    }
//    animateTo( mDirectionalLightIntensity, 1.0f, animTime );
}

void LightManager::switchLightsOff( float animTime, TimelineGroupCCF _ccf ) {
    for ( auto& pl : mPointLights ) {
        Timeline::play( pl.IntensityAnim(), 2, KeyFramePair{animTime, 0.0f }, _ccf );
    }
//    animateTo( mDirectionalLightIntensity, 0.0f, animTime );
}

void LightManager::setLightsIntensity( float _intensity ) {
    for ( auto& pl : mPointLights ) {
        pl.IntensityAnim()->value = _intensity;
    }
}

void LightManager::toggleLightsOnOff() {
    mbGlobalOnOffSwitch = !mbGlobalOnOffSwitch;
    float animTime = 2.0f;
    if ( mbGlobalOnOffSwitch ) {
        switchLightsOn( animTime );
    } else {
        switchLightsOff( animTime );
    }
}

void LightManager::setUniforms( const Vector3f& _cameraPos,
                                std::shared_ptr<ShadowMapManager> smm,
                                const V4f& _sunRadiance ) {
    std::sort( mPointLights.begin(), mPointLights.end(), [_cameraPos]( const auto& a, const auto& b ) -> bool {
        return distance( a.Pos(), _cameraPos ) < distance( b.Pos(), _cameraPos );
    } );

    std::vector<Vector3f> lpos;
    std::vector<Vector3f> ldir;
    std::vector<Vector3f> lintensity;
    std::vector<Vector3f> lattn;
    std::vector<Vector3f> lbeamdir;
    std::vector<float> lbeamAngle;
    std::vector<int> lType;

    int numLightsClamped = 0;
    for ( auto& pl : mPointLights ) {
        lpos.push_back(  pl.Pos() );
        ldir.push_back( Vector3f::Y_AXIS );
        lintensity.emplace_back( pl.Intensity() );
        lattn.push_back( pl.Attenuation() );
        lbeamdir.push_back( Vector3f::Z_AXIS );
        lbeamAngle.push_back( 60.0f );
        lType.push_back( 0 );
        numLightsClamped++;
        if ( numLightsClamped == mMaxLights ) break;
    }

    auto uSunDirection = smm->SunDirection();
    mLigthingUniform->setUBODatav( UniformNames::pointLightPos, lpos );
    mLigthingUniform->setUBODatav( UniformNames::pointLightDir, ldir );
    mLigthingUniform->setUBODatav( UniformNames::pointLightIntensity, lintensity );
    mLigthingUniform->setUBODatav( UniformNames::pointLightAttenuation, lattn );
    mLigthingUniform->setUBODatav( UniformNames::spotLightBeamDir, lbeamdir );
    mLigthingUniform->setUBODatav( UniformNames::outerCutOff, lbeamAngle );
    mLigthingUniform->setUBODatav( UniformNames::lightType, lType );

    mLigthingUniform->setUBOData( UniformNames::sunDirection,  uSunDirection);// );
    mLigthingUniform->setUBOData( UniformNames::sunPosition, uSunDirection * 100000.0f );// );
    mLigthingUniform->setUBOData( UniformNames::sunRadiance, _sunRadiance );// );
    mLigthingUniform->setUBOData( UniformNames::mvpMatrixDepthBias, smm->ShadowMapMVPBias( true ));
    mLigthingUniform->setUBOData( UniformNames::mvpShadowMap, smm->ShadowMapMVP() );
    mLigthingUniform->setUBOData( UniformNames::numPointLights, numLightsClamped );
    mLigthingUniform->setUBOData( UniformNames::hdrExposures, hdrExposures );
    mLigthingUniform->setUBOData( UniformNames::shadowParameters, shadowParameters );
    mLigthingUniform->setUBOData( UniformNames::shLightCoeffs, Matrix3f( SSH.LightCoeffs()) );
    mLigthingUniform->setUBOData( UniformNames::hemisphereKernelSize, mNumHemiKernelSize );
    mLigthingUniform->setUBODatav( UniformNames::hemisphereKernel, mHemisphereKernelSamples );
    mLigthingUniform->setUBOData( UniformNames::ssaoParameters, mSSAOParameters );
}

void LightManager::setUniforms_r() {
    mLigthingUniform->submitUBOData();
}

void LightManager::update( float /*timeStamp*/ ) {
//	setLightsIntensity( 1.0f - SB.GoldenHour() );
}

void LightManager::setShadowOverBurnCofficient( float _overBurn ) {
    shadowParameters[1] = _overBurn;
}

void LightManager::setIndoorSceneCoeff( float _value ) {
    shadowParameters[2] = _value;
}

void LightManager::setShadowZFightCofficient( float _value ) {
    shadowParameters[0] = _value;
}

void LightManager::setSSAOKernelRadius( float _value ) {
    mSSAOParameters[0] = _value;
}

void LightManager::setSSAOFalloffRadius( float _value ) {
    mSSAOParameters[2] = _value;
}

void LightManager::setSSAONumRealTimeSamples( float _value ) {
    mSSAOParameters[1] = _value;
}
