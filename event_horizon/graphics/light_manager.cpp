#include "light_manager.h"

#include "core/math/spherical_harmonics.h"
#include "core/suncalc/sun_builder.h"
#include "shadowmap_manager.h"
#include "program_uniform_set.h"

void LightManager::init() {
    mbGlobalOnOffSwitch = true;
    mDirectionalLightIntensity = std::make_shared<AnimType<float>>( 1.0f );

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
    mLigthingUniform->setUBOStructure( UniformNames::mvpShadowMap, 64 );
    mLigthingUniform->setUBOStructure( UniformNames::mvpMatrixDepthBias, 64 );
    mLigthingUniform->setUBOStructure( UniformNames::timeOfTheDay, 16 );
    mLigthingUniform->setUBOStructure( UniformNames::numPointLights, 16 );
    mLigthingUniform->setUBOStructure( UniformNames::shLightCoeffs, 48 );
}

void LightManager::generateUBO( ShaderManager& sm ) {
    mLigthingUniform->generateUBO( sm, "LightingUniforms");
}

void LightManager::addPointLight( const Vector3f& pos, float intensity, const Vector3f& attenuation ) {
    mPointLights.push_back( { pos, 50.0f, intensity, attenuation } );
}

void LightManager::removePointLight( const size_t index ) {
    ASSERT( index < mPointLights.size());
    mPointLights.erase( mPointLights.begin() + index );
}

void LightManager::removeAllPointLights() {
    mPointLights.clear();
}

void LightManager::switchLightsOn( float animTime ) {
    for ( auto& pl : mPointLights ) {
        animateTo( pl.IntensityAnim(), 1.0f, animTime );
    }
    animateTo( mDirectionalLightIntensity, 1.0f, animTime );
}

void LightManager::switchLightsOff( float animTime ) {
    for ( auto& pl : mPointLights ) {
        animateTo( pl.IntensityAnim(), 0.0f, animTime );
    }
    animateTo( mDirectionalLightIntensity, 0.0f, animTime );
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

void LightManager::setUniforms( const Vector3f& _cameraPos, const std::unique_ptr<ShadowMapManager>& smm ) {
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

    lpos.push_back(Vector3f::ONE);
    int numLightsClamped = 0;
    for ( auto& pl : mPointLights ) {
        lpos.push_back(  pl.Pos());
        ldir.push_back( Vector3f::Y_AXIS );
        lintensity.push_back( Vector3f{ pl.Intensity() } );
        lattn.push_back( pl.Attenuation());
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
    mLigthingUniform->setUBOData( UniformNames::numPointLights, numLightsClamped );
    mLigthingUniform->setUBOData( UniformNames::timeOfTheDay, 1.0f ); //SB.GoldenHour()
    mLigthingUniform->setUBOData( UniformNames::sunDirection,  uSunDirection);// );
    mLigthingUniform->setUBOData( UniformNames::sunPosition, uSunDirection * 100000.0f );// );
    mLigthingUniform->setUBOData( UniformNames::mvpMatrixDepthBias, smm->ShadowMapMVPBias( true ));
    mLigthingUniform->setUBOData( UniformNames::mvpShadowMap, smm->ShadowMapMVP() );
    mLigthingUniform->setUBOData( UniformNames::shLightCoeffs, Matrix3f( SSH.LightCoeffs()) );
}

void LightManager::setUniforms_r() {
    mLigthingUniform->submitUBOData();
}

void LightManager::update( float /*timeStamp*/ ) {
//	setLightsIntensity( 1.0f - SB.GoldenHour() );
}

